#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/packet.h>

#ifdef __cplusplus
}
#endif

#include "queue_safe.hpp"

class QueueAVPacket : public QueueSafe<AVPacket*>
{
public:
    QueueAVPacket();
    virtual ~QueueAVPacket();

    void Push(AVPacket* value);
    void Abort();

public:
    std::atomic<bool> m_abort;
};
