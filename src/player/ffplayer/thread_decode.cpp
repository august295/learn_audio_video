#include <spdlog/spdlog.h>

#include "thread_decode.h"

ThreadDecode::ThreadDecode(QueueAVPacket* packet_queue, QueueAVFrame* frame_queue)
    : m_packet_queue(packet_queue)
    , m_frame_queue(frame_queue)
{
    memset(m_err2str, 0, 256);
    m_codec_ctx = nullptr;
}

ThreadDecode::~ThreadDecode()
{
    spdlog::info("~ThreadDecode: {}", m_codec_ctx->codec->name);
    Stop();
    if (m_codec_ctx)
    {
        avcodec_free_context(&m_codec_ctx);
    }
}

bool ThreadDecode::Init(AVCodecParameters* codec_params)
{
    int ret = 0;
    if (!codec_params)
    {
        spdlog::error("codec_params is nullptr");
        return false;
    }
    // 1. 分配编解码器上下文
    m_codec_ctx = avcodec_alloc_context3(nullptr);
    if (!m_codec_ctx)
    {
        spdlog::error("Failed to allocate codec context");
        return false;
    }
    // 2. 复制参数到上下文
    ret = avcodec_parameters_to_context(m_codec_ctx, codec_params);
    if (ret < 0)
    {
        av_strerror(ret, m_err2str, sizeof(m_err2str));
        spdlog::error("Failed to convert codec parameters to context: {}", m_err2str);
        return false;
    }
    // 3. 查找解码器
    // TODO: 可以添加硬件加速
    const AVCodec* codec = avcodec_find_decoder(m_codec_ctx->codec_id);
    if (!codec)
    {
        spdlog::error("Failed to find codec: {}", (int)m_codec_ctx->codec_id);
        return false;
    }
    // 4. 打开解码器
    ret = avcodec_open2(m_codec_ctx, codec, nullptr);
    if (ret != 0)
    {
        av_strerror(ret, m_err2str, sizeof(m_err2str));
        spdlog::error("Failed to open codec: {}", m_err2str);
        return false;
    }
    spdlog::info("codec_name: {}, codec_id: {}", m_codec_ctx->codec->name, (int)m_codec_ctx->codec_id);
    return true;
}

void ThreadDecode::Run()
{
    AVFrame*  frame = av_frame_alloc();
    AVPacket* pkt   = nullptr;
    int       ret   = 0;

    // 流控：控制解码帧队列大小
    if (m_frame_queue->Size() > 10)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return;
    }

    // 从包队列获取数据
    m_packet_queue->Pop(pkt, std::chrono::milliseconds(10));
    if (pkt)
    {
        // 发送包到解码器，立即释放包
        ret = avcodec_send_packet(m_codec_ctx, pkt);
        if (ret < 0 && ret != AVERROR(EAGAIN))
        {
            av_strerror(ret, m_err2str, sizeof(m_err2str));
            spdlog::error("Failed to send packet: {}", m_err2str);
            return;
        }

        // 接收解码帧（可能有多帧）
        while (true)
        {
            ret = avcodec_receive_frame(m_codec_ctx, frame);
            if (ret == 0)
            {
                m_frame_queue->Push(frame);
                spdlog::debug("receive frame: name: {}, size: {}", m_codec_ctx->codec->name, m_frame_queue->Size());
                continue;
            }
            else if (AVERROR(EAGAIN) == ret)
            {
                // 需要更多输入包
                break;
            }
            else if (AVERROR_EOF == ret)
            {
                // 视频流结束
                m_abort.store(true);
                m_frame_queue->m_abort.store(true);
                spdlog::info("End of file: {}", m_codec_ctx->codec->name);
                break;
            }
            else
            {
                m_abort.store(true);
                av_strerror(ret, m_err2str, sizeof(m_err2str));
                spdlog::error("Failed to receive frame: {}", m_err2str);
                break;
            }
        }
        av_packet_free(&pkt);
    }
    else
    {
        if (m_packet_queue->m_abort.load())
        {
            // 音频流结束
            m_abort.store(true);
            m_frame_queue->m_abort.store(true);
            spdlog::info("End of file: {}", m_codec_ctx->codec->name);
        }
    }
}
