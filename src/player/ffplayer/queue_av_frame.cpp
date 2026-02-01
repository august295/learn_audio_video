#include <spdlog/spdlog.h>

#include "queue_av_frame.h"

QueueAVFrame::QueueAVFrame()
{
    m_abort.store(false);
}

QueueAVFrame::~QueueAVFrame()
{
    spdlog::info("~QueueAVFrame: {}", m_name);
    Abort();
}

void QueueAVFrame::Push(AVFrame* value)
{
    AVFrame* tmp_frame = av_frame_alloc();
    av_frame_move_ref(tmp_frame, value); // 移动引用（避免拷贝）
    return QueueSafe::Push(tmp_frame);
}

void QueueAVFrame::Abort()
{
    m_abort.store(true);
    AVFrame* frame = NULL;
    while (!Empty())
    {
        if (Pop(frame, std::chrono::milliseconds(10)))
        {
            av_frame_free(&frame);
        }
    }
    m_cv.notify_all();
}