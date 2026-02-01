#include <spdlog/spdlog.h>

#include "queue_av_packet.h"

QueueAVPacket::QueueAVPacket()
{
    m_abort.store(false);
}

QueueAVPacket::~QueueAVPacket()
{
    spdlog::info("~QueueAVPacket: {}", m_name);
    Abort();
}

void QueueAVPacket::Push(AVPacket* pkt)
{
    AVPacket* tmp_pkt = av_packet_alloc();
    av_packet_move_ref(tmp_pkt, pkt);
    return QueueSafe::Push(tmp_pkt);
}

void QueueAVPacket::Abort()
{
    m_abort.store(true);
    AVPacket* pkt = NULL;
    while (!Empty())
    {
        if (Pop(pkt, std::chrono::milliseconds(10)))
        {
            av_packet_free(&pkt);
        }
    };

    m_cv.notify_all();
}