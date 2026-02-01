#include <spdlog/spdlog.h>

#include "thread_demux.h"

ThreadDemux::ThreadDemux(QueueAVPacket* video_packet_queue, QueueAVPacket* audio_packet_queue)
    : m_video_packet_queue(video_packet_queue)
    , m_audio_packet_queue(audio_packet_queue)
{
    m_url = "";
    memset(m_err2str, 0, 256);
    m_format_ctx         = nullptr;
    m_video_stream_index = -1;
    m_audio_stream_index = -1;
}

ThreadDemux::~ThreadDemux()
{
    spdlog::info("~ThreadDemux");
    Stop();
}

bool ThreadDemux::Open(const std::string& url)
{
    /**
     * @brief   解复用打开媒体文件
     * ⽂件解析: 解析容器格式，获取流信息
     */
    m_url        = url;
    int ret      = 0;
    m_format_ctx = avformat_alloc_context();
    ret          = avformat_open_input(&m_format_ctx, m_url.c_str(), nullptr, nullptr);
    if (ret != 0)
    {
        av_strerror(ret, m_err2str, sizeof(m_err2str));
        spdlog::error("Failed to open input file: {}", m_err2str);
        return false;
    }
    ret = avformat_find_stream_info(m_format_ctx, nullptr);
    if (ret < 0)
    {
        av_strerror(ret, m_err2str, sizeof(m_err2str));
        spdlog::error("Failed to find stream info: {}", m_err2str);
        return false;
    }
    av_dump_format(m_format_ctx, 0, m_url.c_str(), 0);
    m_video_stream_index = av_find_best_stream(m_format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    m_audio_stream_index = av_find_best_stream(m_format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (m_video_stream_index < 0 || m_audio_stream_index < 0)
    {
        spdlog::error("Failed to find video or audio stream");
        return false;
    }
    spdlog::info("video_stream_index: {}, audio_stream_index: {}", m_video_stream_index, m_audio_stream_index);
    return true;
}

void ThreadDemux::Run()
{
    /**
     * @brief   解复用分离数据
     * 流分离: 将交错的⾳视频数据分离到不同队列
     * 流控制: 控制读取速度，避免内存溢出
     */
    int       ret = 0;
    AVPacket* pkt = av_packet_alloc();
    if (m_video_packet_queue->Size() > 100 || m_audio_packet_queue->Size() > 100)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return;
    }

    ret = av_read_frame(m_format_ctx, pkt);
    if (ret < 0)
    {
        if (ret == AVERROR_EOF)
        {
            spdlog::info("End of file: {}", m_url);
            m_abort.store(true);
            m_video_packet_queue->m_abort.store(true);
            m_audio_packet_queue->m_abort.store(true);
        }
        else if (ret == AVERROR(EAGAIN))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return;
        }
        else
        {
            av_strerror(ret, m_err2str, sizeof(m_err2str));
            spdlog::error("Failed to read frame: {}", m_err2str);
            return;
        }
    }
    if (pkt->stream_index == m_video_stream_index)
    {
        m_video_packet_queue->Push(pkt);
    }
    else if (pkt->stream_index == m_audio_stream_index)
    {
        m_audio_packet_queue->Push(pkt);
    }
    else
    {
        av_packet_free(&pkt);
    }
}

void ThreadDemux::Stop()
{
    // 覆盖 ThreadSafe::Stop，需要自定义资源释放
    ThreadSafe::Stop();
    avformat_close_input(&m_format_ctx);
}

AVCodecParameters* ThreadDemux::AudioCodecParameters()
{
    if (m_audio_stream_index < 0)
    {
        return nullptr;
    }
    return m_format_ctx->streams[m_audio_stream_index]->codecpar;
}

AVCodecParameters* ThreadDemux::VideoCodecParameters()
{
    if (m_video_stream_index < 0)
    {
        return nullptr;
    }
    return m_format_ctx->streams[m_video_stream_index]->codecpar;
}

AVRational ThreadDemux::AudioStreamTimebase()
{
    if (m_audio_stream_index < 0)
    {
        return AVRational{0, 1};
    }
    return m_format_ctx->streams[m_audio_stream_index]->time_base;
}

AVRational ThreadDemux::VideoStreamTimebase()
{
    if (m_video_stream_index < 0)
    {
        return AVRational{0, 1};
    }
    return m_format_ctx->streams[m_video_stream_index]->time_base;
}