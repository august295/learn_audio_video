#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

/**
 * @brief   线程安全基类
 * 1. 线程系统⼀接⼝: 所有线程类都有相同的启动、停⽌接⼝
 * 2. ⽣命周期管理: 基类负责线程的创建、销毁和异常处理
 * 3. 优雅退出: 通过 m_abort 标志实现线程的协作式退出
 */
class ThreadSafe
{
public:
    ThreadSafe();
    virtual ~ThreadSafe();

    virtual bool Start();
    virtual void Stop();
    virtual void Pause();
    virtual void Resume();
    virtual void WaitIfPaused();
    // TODO: 纯虚函数，子类必须实现
    virtual void Run() = 0;

    bool IsRunning();
    bool IsAborted();
    bool IsPaused();

private:
    /**
     * @brief   线程入口，已实现循环，Run() 只需要实现循环体
     */
    void ThreadEntry();

protected:
    std::thread             m_thread;  // 线程
    std::atomic<bool>       m_running; // 线程状态
    std::atomic<bool>       m_abort;   // 线程退出
    std::atomic<bool>       m_paused;  // 线程暂停
    std::mutex              m_mutex;   // 线程锁
    std::condition_variable m_cv;      // 线程同步
};
