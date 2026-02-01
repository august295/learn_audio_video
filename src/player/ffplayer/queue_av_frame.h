#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <libavutil/frame.h>

#ifdef __cplusplus
}
#endif

#include "queue_safe.hpp"

class QueueAVFrame : public QueueSafe<AVFrame*>
{
public:
    QueueAVFrame();
    virtual ~QueueAVFrame();

    void Push(AVFrame* value);
    void Abort();

public:
    std::atomic<bool> m_abort;
};