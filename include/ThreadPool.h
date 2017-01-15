#ifndef DWARF_THREADPOOL_H_
#define DWARF_THREADPOOL_H_
#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique(Args&& ...args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

namespace Dwarf
{
    class Thread
    {
    public:
        Thread();
        virtual ~Thread();
        void addJob(std::function<void()> function);
        void wait();
        bool isJobQueueEmpty() const;
        bool shouldDestroy() const;

    private:
        void queueLoop();

        bool _toDestroy;
        std::thread _worker;
        std::queue<std::function<void()>> _jobQueue;
        std::mutex _queueMutex;
        std::condition_variable _condition;
    };

    class ThreadPool
    {
    public:
        ThreadPool();
        virtual ~ThreadPool();
        void setThreadCount(uint32_t threadCount);
        void addJobThread(const uint32_t &threadIndex, std::function<void()> function);
        void wait();

    private:
        std::vector<std::unique_ptr<Thread>> _threads;
        std::vector<std::unique_ptr<Thread>>::iterator _currentThreadIterator;
        std::vector<std::unique_ptr<Thread>>::iterator _beginThreadsIterator;
        std::vector<std::unique_ptr<Thread>>::iterator _endThreadsIterator;
    };
}

#endif // DWARF_THREADPOOL_H_
