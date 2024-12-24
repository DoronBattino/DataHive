#include <algorithm>            // for_each
#include <string>               // to_string
#include <mutex>                // std::mutex
#include <chrono>               // miliseconds
#include <condition_variable>   // condition var

#include "threadpool.hpp"

namespace ilrd
{

/************************ Public *************************/



ThreadPool::ThreadPool(size_t numOfThrds_) :
    m_logger(Singleton<Logger>::GetInstance()),
    m_numOfThrds(numOfThrds_),
    m_state(CurrState::RUNNING)
{
    Run();
}





ThreadPool::~ThreadPool()
{
    Stop();
}




void ThreadPool::Add(const std::shared_ptr<ITask> task_)
{
    // add task to waitQ
    m_waitQ.Push(task_);
}




void ThreadPool::Pause()
{
    // turn on pause flag
    m_state = CurrState::PAUSE;

}





void ThreadPool::Resume()
{
    // turn off pause flag
    m_state = CurrState::RUNNING;
}





void ThreadPool::SetThredNum(size_t num_)
{
    // Stop
    Stop();

    // resize pool
    m_pool.resize(num_);

    // update num of threads
    m_numOfThrds = num_;

    // Run again
    Run();
}




/************************ Private *************************/



void ThreadPool::Run()
{
    // make sure running mode
    m_state = CurrState::RUNNING;

    // start all threads
    for (size_t i = 0; i < m_numOfThrds; ++i)
    {
        m_pool.push_back(std::thread(&ThreadPool::ThrdRoutine, this));
    }
}






void ThreadPool::ThrdRoutine()
{
    // while no stop request
    while (CurrState::STOP != m_state)
    {
        std::mutex thrdLock;
        std::condition_variable waitVar;
        std::unique_lock<std::mutex> lock(thrdLock);
        waitVar.wait_for(lock, std::chrono::milliseconds(2));

        // if no pause request
        if (CurrState::PAUSE != m_state)
        {
            // timed pop a task
            std::shared_ptr<ITask> toDo(nullptr);
            if (true == m_waitQ.Pop(std::chrono::milliseconds(2), toDo))
            {
                // run thrd with a ITask::Execute()
                toDo->Execute();
            }
        }
    }   
}






void ThreadPool::Stop()
{
    // finish all tasks in queue
    while(!m_waitQ.Empty());

    // stop request
    m_state = CurrState::STOP;

    // join all threads
    try
    {
        std::for_each
        (
            m_pool.begin(), m_pool.end(), [this](std::thread& thrd_)
            {
                if (thrd_.joinable())
                {
                    thrd_.join();
                }
            }
        );
    }
    
    // handle error
    catch(const std::system_error& se_)
    {
        m_logger->Log(Logger::Level::ERROR, se_.what(), __FILE__, __LINE__);
    }
}



template <typename T>
bool CmpViaPtr<T>::operator()(const T& lhs_, const T& rhs_)
{
    return (*lhs_ < *rhs_);
}


}// namespace ilrd