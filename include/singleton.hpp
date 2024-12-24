/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       03/10/23.
*       Title:      Singleton Design Pattern.
*       Desc:       
*       Group:      HRD30.
***************************************************************************************/


#ifndef __ILRD_SINGLETON_HPP__
#define __ILRD_SINGLETON_HPP__

#include <mutex>                // mutex
#include <atomic>               // atomic_thread_fence
#include <cstdlib>              // atexit
#include <cstring>              // strcpy
#include <stdexcept>            // bad_alloc

namespace ilrd
{

// T must be uncopyable, unassignable, must have private default
// ctor and it must have singltone friend
template <typename T>
class Singleton
{
public:
    static T* GetInstance();
    
    Singleton()= delete;
    Singleton(const Singleton& o_) = delete;
    Singleton& operator=(const Singleton& o_) = delete;
    ~Singleton()= delete;

private:
    static void CleanUp(); //check how this is working- use at exit function
    static std::atomic<T*> s_instancePtr;
    static std::mutex s_mutex;
    
}; // Singleton

template <typename T>
std::atomic<T*> ilrd::Singleton<T>::s_instancePtr = nullptr;

template <typename T>
std::mutex ilrd::Singleton<T>::s_mutex;



/*********************** implementation ************************/



// may throw bad_alloc
template <typename T>
T* Singleton<T>::GetInstance()
{
    T* tmp = s_instancePtr.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);

    if (nullptr == tmp)
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        tmp = s_instancePtr.load(std::memory_order_relaxed);

        if (nullptr == tmp)
        {
            try
            {
                tmp = new T;
            }

            catch(const std::bad_alloc& ba_)
            {                                              
                throw std::bad_alloc();
            }

            std::atomic_thread_fence(std::memory_order_release);
            s_instancePtr.store(tmp, std::memory_order_relaxed);
            std::atexit(CleanUp);
        }
    }

    return tmp;
}


template <typename T>
void Singleton<T>::CleanUp()
{
    T *tmp = s_instancePtr;
    s_instancePtr.store(reinterpret_cast<T *>(0xDEADBEEF), std::memory_order_relaxed);
    delete tmp;
}


// class Test
// {
// public:
//     Test() = default;
//     Test(const Test& o_) = delete;
//     Test& operator=(const Test& o_) = delete;

//     void Do(){std::cout << "do called" << std::endl;}

// private:
//     friend class Singleton<Test>;
// };



} // namespace ilrd

#endif // __ILRD_SINGLETON_HPP__
