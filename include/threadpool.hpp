/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       03/10/23.
*       Title:      Thread Pool Design Pattern.
*       Desc:       
*       Group:      HRD30.
***************************************************************************************/


#ifndef __ILRD_THREADPOOL_HPP__
#define __ILRD_THREADPOOL_HPP__

#include <cstddef>                  // size_t
#include <vector>                   // std::vector
#include <thread>                   // std::thread
#include <condition_variable>       // std::condition_variable
#include <mutex>                    // std::mutex
#include <memory>                   // std::shared_ptr

#include "i_task.hpp"
#include "logger.hpp"
#include "waitable_queue.hpp"
#include "priority_queue.hpp"

namespace ilrd
{

enum class CurrState
{
    RUNNING = 0,
    PAUSE,
    STOP
};


template <typename T>
class CmpViaPtr
{
public:
    bool operator()(const T& lhs_, const T& rhs_);
};



class ThreadPool
{
public:

    explicit ThreadPool(size_t numOfthreads = std::thread::hardware_concurrency());
    ThreadPool(const ThreadPool& other_) = delete;
    ThreadPool(ThreadPool&& other_) = delete;
    ThreadPool& operator=(const ThreadPool& other_) = delete;
    ThreadPool& operator=(ThreadPool&& other_) = delete;
    ~ThreadPool() noexcept;

    void Add(const std::shared_ptr<ITask> task_);
    void Pause();
    void Resume();
    void SetThredNum(size_t num_);

private:

    void Run();
    void Stop();
    void ThrdRoutine();

    std::vector<std::thread> m_pool;
    WaitableQueue
    <
        std::shared_ptr<ITask>,
        PriorityQueue
        <
            std::shared_ptr<ITask>,
            std::vector<std::shared_ptr<ITask>>,
            CmpViaPtr<std::shared_ptr<ITask>>
        >
        
    > m_waitQ;

    Logger *m_logger;

    size_t m_numOfThrds;
    CurrState m_state;

}; // class ThreadPool



}; // namespace ilrd
#endif // __ILRD_THREADPOOL_HPP__
