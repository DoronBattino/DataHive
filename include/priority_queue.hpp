/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       03/10/23.
*       Title:      Priority Queue,
*       Desc:       Wrapper that adds front() to std::priority_queue
*    		    Concepts:
*    		    T should be copyable and assignable.
*       Group:      HRD30.
***************************************************************************************/


#ifndef __ILRD_PRIORITY_QUEUE_HPP__
#define __ILRD_PRIORITY_QUEUE_HPP__

#include <queue>        // priority_queue


namespace ilrd
{


template
<
    typename T,
    typename CONTAINER = std::vector<T>,
    typename COMPARE = std::less<typename CONTAINER::value_type>
>
class PriorityQueue : private std::priority_queue<T,CONTAINER,COMPARE>
{
public:

    PriorityQueue() = default;
    PriorityQueue(const PriorityQueue&) = default;
    PriorityQueue& operator=(const PriorityQueue&) = default;
    PriorityQueue(PriorityQueue&&) = default;
    PriorityQueue& operator=(PriorityQueue&&) = default;
    ~PriorityQueue() = default;

    const T& front() const;
    
    using priority_queue = std::priority_queue<T, CONTAINER, COMPARE>;
    using priority_queue::push;
    using priority_queue::pop;
    using priority_queue::empty;

};// class priority_queue


/**************************** Implementation *****************************/


template<typename T, typename CONTAINER, typename COMPARE>
const T& PriorityQueue<T, CONTAINER, COMPARE>::front() const
{
    return priority_queue::top();
}


} // namespace ilrd


#endif // __ILRD_PRIORITY_QUEUE_HPP__
