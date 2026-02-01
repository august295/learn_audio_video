#include <spdlog/spdlog.h>

#include "thread_safe.h"

ThreadSafe::ThreadSafe()
{
}

ThreadSafe::~ThreadSafe()
{
    spdlog::info("~ThreadSafe");
    Stop();
}

bool ThreadSafe::Start()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    /**
     * @brief   原子操作
     * = 相当于 store(flag, std::memory_order_seq_cst)，这种最安全，确保全局顺序一致性，但是较慢
     * memory_order_relaxed     只保证原子性
     * memory_order_consume     几乎不用（已废）
     * memory_order_acquire     读屏障
     * memory_order_release     写屏障
     * memory_order_acq_rel     读+写
     * memory_order_seq_cst     全局顺序一致（默认）
     * 
     * 发布订阅推荐使用: memory_order_release + memory_order_acquire
     */
    if (m_running.load())
    {
        return false;
    }
    m_abort.store(false);
    m_paused.store(false);
    m_running.store(true);
    m_thread = std::thread(&ThreadSafe::ThreadEntry, this);

    return true;
}

void ThreadSafe::Stop()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_abort.store(true);
        m_paused.store(false);
        m_running.store(false);
    }

    m_cv.notify_all();
    if (m_thread.joinable() && m_thread.get_id() != std::this_thread::get_id())
    {
        m_thread.join();
    }
}

void ThreadSafe::Pause()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_running.load())
    {
        m_paused.store(false);
    }
}

void ThreadSafe::Resume()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_running.load())
    {
        m_paused.store(true);
    }
}

void ThreadSafe::WaitIfPaused()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this] { return !m_paused.load(); });
}

bool ThreadSafe::IsRunning()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_running.load();
}

bool ThreadSafe::IsAborted()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_abort.load();
}

bool ThreadSafe::IsPaused()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_paused.load();
}

void ThreadSafe::ThreadEntry()
{
    while (!IsAborted())
    {
        WaitIfPaused();
        Run();
    }
    m_running.store(false);
}