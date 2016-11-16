#include "ThreadPool.h"

namespace Dwarf
{
    Thread::Thread()
        : _toDestroy(false)
    {
        this->_worker = std::thread(&Thread::queueLoop, this);
    }

    Thread::~Thread()
    {
        if (this->_worker.joinable())
        {
            this->wait();
            this->_queueMutex.lock();
            this->_toDestroy = true;
            this->_condition.notify_one();
            this->_queueMutex.unlock();
            this->_worker.join();
        }
    }

    void Thread::addJob(std::function<void()> function)
    {
        std::lock_guard<std::mutex> lock(this->_queueMutex);
        this->_jobQueue.push(std::move(function));
        this->_condition.notify_one();
    }

    void Thread::wait()
    {
        std::unique_lock<std::mutex> lock(this->_queueMutex);
        this->_condition.wait(lock, [this]() { return (this->isJobQueueEmpty()); });
    }

    bool Thread::isJobQueueEmpty() const
    {
        return (this->_jobQueue.empty());
    }

    bool Thread::shouldDestroy() const
    {
        return (this->_toDestroy);
    }

    void Thread::queueLoop()
    {
        std::function<void()> job;

        while (true)
        {
            {
                std::unique_lock<std::mutex> lock(this->_queueMutex);
                this->_condition.wait(lock, [this] { return (!this->isJobQueueEmpty() || this->shouldDestroy()); });
                if (this->_toDestroy)
                    break;
                job = this->_jobQueue.front();
            }
            job();
            {
                std::lock_guard<std::mutex> lock(this->_queueMutex);
                this->_jobQueue.pop();
                this->_condition.notify_one();
            }
        }
    }

    ThreadPool::ThreadPool()
    {
    }

    ThreadPool::~ThreadPool()
    {
    }

    void ThreadPool::setThreadCount(uint32_t threadCount)
    {
        uint32_t i = 0;
        
        this->_threads.clear();
        while (i < threadCount)
        {
            this->_threads.push_back(make_unique<Thread>());
            ++i;
        }
        this->_beginThreadsIterator = this->_threads.begin();
        this->_endThreadsIterator = this->_threads.end();
    }

    void ThreadPool::addJobThread(const uint32_t &threadIndex, std::function<void()> function)
    {
        this->_threads.at(threadIndex)->addJob(function);
    }

    void ThreadPool::wait()
    {
        this->_currentThreadIterator = this->_beginThreadsIterator;
        while (this->_currentThreadIterator != this->_endThreadsIterator)
        {
            (*this->_currentThreadIterator)->wait();
            ++this->_currentThreadIterator;
        }
    }
}
