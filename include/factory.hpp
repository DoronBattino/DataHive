/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       03/10/23.
*       Title:      Factory design pattern.
*       Desc:       class to create different tasks.
*       Group:      HRD30.
***************************************************************************************/


#ifndef __ILRD_FACTORY_HPP__
#define __ILRD_FACTORY_HPP__

#include <memory>           // shared_ptr
#include <stdexcept>        // runtime_error
#include <functional>       // std::function
#include <map>              // unordered map

#include "singleton.hpp"

namespace ilrd
{


template <typename BASE, typename KEY, typename... ARGS>
class Factory
{
public:
    using CreateTaskFunc_ty = std::function<std::shared_ptr<BASE>(ARGS ...)>;
    Factory(const Factory& o_) = delete;
    Factory& operator=(const Factory& o_) = delete;

    // products creation functions
    void Register(KEY key_, CreateTaskFunc_ty creator_);
    std::shared_ptr<BASE> CreateTask(KEY key_, ARGS... args_);

private:
    Factory() = default;
    ~Factory() = default;

    std::unordered_map<KEY, CreateTaskFunc_ty> m_creators;
    
    friend class Singleton<Factory<BASE, KEY, ARGS...>>;

}; // class Factory




class FactoryException : public std::runtime_error
{
public:
    FactoryException(const std::string& error_) : std::runtime_error(error_){}

}; // class FactoryException




/*********************** Implementation ************************/




template <typename BASE, typename KEY, typename... ARGS>
void Factory<BASE, KEY, ARGS...>::Register(KEY key_, std::function<std::shared_ptr<BASE>(ARGS...)> creator_)
{
    m_creators.insert(std::make_pair(key_, creator_));
}




template <typename BASE, typename KEY, typename... ARGS>
std::shared_ptr<BASE> Factory<BASE, KEY, ARGS...>::CreateTask(KEY key_, ARGS... args_)
{
    return m_creators[key_](args_...);
}


} // ilrd

#endif  // __ILRD_FACTORY_HPP__
