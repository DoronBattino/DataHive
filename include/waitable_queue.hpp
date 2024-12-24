/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       03/10/23.
*       Title:      Waitable Queue.
*       Desc:       Thread safe template for queue data structure.
*		    Concepts:
* 		    T should be copyable and assignable.
* 		    CONTAINER should support an API identical to std::queue's API:
*		    push(), pop(), front(), empty().
*       Group:      HRD30.
***************************************************************************************/


#ifndef ILRD_WAITABLE_QUEUE
#define ILRD_WAITABLE_QUEUE

#include <iostream>             // cout, endl
#include <queue>                // std::queue
#include <mutex>                // std::timed_mutex, std::unique_lock
#include <condition_variable>   // std:condition_variable
#include <chrono>               // chrono

namespace ilrd
{


template <typename T, typename CONTAINER = std::queue<T>>// template on prio que
class WaitableQueue
{
public:
    WaitableQueue() = default;
    WaitableQueue(const WaitableQueue &other) = delete;
    WaitableQueue &operator=(const WaitableQueue &other) = delete;
    WaitableQueue(WaitableQueue &&other) = delete;
    WaitableQueue &operator=(WaitableQueue &&other) = delete;

    void Push(const T &item);
    void Pop(T &out_item);
    bool Pop(std::chrono::milliseconds timeout_ms, T &out_item);
    bool Empty() const;

private:
    CONTAINER m_container;
    mutable std::timed_mutex m_mutex;
    std::condition_variable_any m_cond;

    inline std::chrono::milliseconds CurrnetTimeMiliseconds();

}; // calss WaitableQueue



    /****************************** Implementation *******************************/



template <typename T, typename CONTAINER>
std::chrono::milliseconds WaitableQueue<T, CONTAINER>::CurrnetTimeMiliseconds()
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    return ms;
}





template <typename T, typename CONTAINER>
void WaitableQueue<T, CONTAINER>::Push(const T &item)
{
    //  while !try lock
    std::unique_lock<std::timed_mutex> lock(m_mutex);

    // CONTAINER.push(item)
    m_container.push(item);

    // std::cout << item << " enqueued" << std::endl; // debug

    lock.unlock();

    // notify one thrd
    m_cond.notify_one();
}





template <typename T, typename CONTAINER>
void WaitableQueue<T, CONTAINER>::Pop(T &out_item)
{
    std::unique_lock lock(m_mutex);

    // while !try lock
    m_cond.wait(lock, [this]{return !m_container.empty();});

    // out_item = CONTAINER.pop()
    out_item = m_container.front();
    m_container.pop();

    // std::cout << out_item << " poped" << std::endl; // debug
}




template <typename T, typename CONTAINER>
bool WaitableQueue<T, CONTAINER>::Pop(std::chrono::milliseconds timeout_ms, T &out_item)
{
    std::chrono::milliseconds start_time = CurrnetTimeMiliseconds();
    std::unique_lock lock(m_mutex, timeout_ms);
    std::chrono::milliseconds remained = timeout_ms - (CurrnetTimeMiliseconds() - start_time);

    if (std::chrono::milliseconds::zero() >= remained)
    {
        return false;
    }

    if (false == m_cond.wait_for(lock, remained, [this]{return !m_container.empty();}))
    {
        return false;
    }
    out_item = m_container.front();
    m_container.pop();
    // std::cout << out_item << " poped" << std::endl; // debug
    return true;
}





template <typename T, typename CONTAINER>
bool WaitableQueue<T, CONTAINER>::Empty() const
{
    // lock with guard
    std::unique_lock<std::timed_mutex> lock(m_mutex);

    // return isEmpty
    return m_container.empty();
}


} // namespace ilrd

#endif // ILRD_WAITABLE_QUEUE
