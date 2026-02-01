#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>

#ifdef __cplusplus
}
#endif

#include "thread_safe.h"
#include "queue_av_packet.h"
#include "queue_av_frame.h"

class ThreadDecode : public ThreadSafe
{
public:
    ThreadDecode(QueueAVPacket* packet_queue, QueueAVFrame* frame_queue);
    virtual ~ThreadDecode();

    bool         Init(AVCodecParameters* codec_params);
    virtual void Run() override;

private:
    char            m_err2str[256]; // 错误信息缓冲区
    AVCodecContext* m_codec_ctx;    // 编解码器上下文
    QueueAVPacket*  m_packet_queue; // 输入包队列
    QueueAVFrame*   m_frame_queue;  // 输出帧队列
};