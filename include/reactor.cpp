/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       20/05/24.
*       Title:      Reactor.
*       Desc:       Recieving request.
*       Group:      HRD30.
***************************************************************************************/

#ifndef __ILRD_REACTOR_HPP__
#define __ILRD_REACTOR_HPP__

#include "factory.hpp"
#include "singleton.hpp"
#include "logger.hpp"
#include "threadpool.hpp"
#include "driver_data.hpp"

namespace
{

template <typename BASE, typename KEY, typename... ARGS>
class Reactor
{
public:

    // Add() using std::function
    void Add();

    // Remove()

    // Run() is a blocking function (need to hndle multiple calls)

    // Stop() callback functions not allowed after calling stop

private:

    friend class Singleton<Factory<BASE, KEY, ARGS...>>;

}; // class Reactor








} // namespace ilrd

#endif // __ILRD_REACTOR_HPP__