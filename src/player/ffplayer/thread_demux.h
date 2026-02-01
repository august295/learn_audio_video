#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <libavformat/avformat.h>
#include <libavutil/rational.h>

#ifdef __cplusplus
}
#endif

#include "thread_safe.h"
#include "queue_av_packet.h"

class ThreadDemux : public ThreadSafe
{
public:
    ThreadDemux(QueueAVPacket* video_packet_queue, QueueAVPacket* audio_packet_queue);
    virtual ~ThreadDemux();

    bool         Open(const std::string& url);
    virtual void Run() override;
    virtual void Stop() override;

    AVCodecParameters* AudioCodecParameters();
    AVCodecParameters* VideoCodecParameters();
    AVRational         AudioStreamTimebase();
    AVRational         VideoStreamTimebase();

private:
    QueueAVPacket* m_video_packet_queue;   // 视频包队列
    QueueAVPacket* m_audio_packet_queue;   // 音频包队列

    std::string      m_url;                // 媒体文件路径
    char             m_err2str[256];       // 错误信息缓冲区
    AVFormatContext* m_format_ctx;         // 格式上下文
    int              m_video_stream_index; // 视频流索引
    int              m_audio_stream_index; // 音频流索引
};