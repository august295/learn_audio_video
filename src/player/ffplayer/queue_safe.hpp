#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>

/**
 * @brief   安全队列基类
 * @tparam  T                                   存储类型
 * 代码复⽤: AVPacketQueue 和 AVFrameQueue 共享相同的队列逻辑
 * 类型安全: 模板确保编译时类型检查
 * 线程安全: 统⼀的互斥锁和条件变量机制
 * 阻塞控制: ⽀持超时和中⽌操作
 */
template <typename T>
class QueueSafe
{
public:
    QueueSafe()  = default;
    ~QueueSafe() = default;

    void Push(T value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(value);
        m_cv.notify_one();
    }

    bool Pop(T& value)
    {
        // 一直等待，直到有数据
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty())
            return false;
        value = m_queue.front();
        m_queue.pop();
        return true;
    }

    template <typename Rep, typename Period>
    bool Pop(T& value, const std::chrono::duration<Rep, Period>& timeout)
    {
        // 超时等待
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_cv.wait_for(lock, timeout, [this] { return !m_queue.empty(); }))
        {
            value = m_queue.front();
            m_queue.pop();
            return true;
        }
        return false;
    }

    bool Front(T& value)
    {
        // 获取队列头，同步时使用，预处理数是否被使用，确认使用后删除
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty())
            return false;
        value = m_queue.front();
        return true;
    }

    size_t Size()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    bool Empty()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    void SetName(const std::string& name)
    {
        m_name = name;
    }

protected:
    std::string             m_name;  // 队列名称
    std::queue<T>           m_queue; // 队列
    mutable std::mutex      m_mutex; // 互斥锁
    std::condition_variable m_cv;    // 条件变量
};
