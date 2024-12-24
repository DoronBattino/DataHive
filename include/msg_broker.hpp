/*******************************************************************************
* Author: Doron
* Date: 01.04.24
* Version: 0 
* Description: Publisher-Subscriber, Dispatcher-Callback API
*******************************************************************************/
#ifndef __ILRD_HRD32_MSG_BROKER_HPP__
#define __ILRD_HRD32_MSG_BROKER_HPP__

#include <iostream>             // cerr, endl
#include <cstddef>              // size_t 
#include <vector>               // std::vector
#include <algorithm>            // std::for_each


namespace ilrd
{

template<typename MSG>    
class ICallBack;

template <typename MSG>
class Dispatcher
{
public:
    explicit Dispatcher() = default;
    ~Dispatcher();

    Dispatcher(Dispatcher&) = delete;
    Dispatcher& operator=(Dispatcher&) = delete;
    Dispatcher(Dispatcher&&) = delete;
    Dispatcher& operator=(Dispatcher&&) = delete;

    void NotifyAll(const MSG& msg_);

private:
    void UnSubscribe(ICallBack<MSG>* cb_);
    void Subscribe(ICallBack<MSG>* cb_);
    void NotifyDiscAll();
    
    std::vector<ICallBack<MSG>*> m_subscribers;
    
    friend class ICallBack<MSG>;

}; // class Dispatcher






template <typename MSG>
class ICallBack
{
public:
    explicit ICallBack(Dispatcher<MSG>* disp_);
    ICallBack(const ICallBack& other_) = delete;
    ICallBack& operator=(const ICallBack& other_) = delete;
    virtual ~ICallBack();

protected:
    Dispatcher<MSG> *GetDisp();

private:
    virtual void Notify(const MSG& msg_) = 0;
    virtual void NotifyDisc() = 0;

    Dispatcher<MSG>* m_dispatcher;

    friend class Dispatcher<MSG>;

}; // class ICallBack





// SUBSRIBER must have Update method (for Notify)
// SUBSRIBER Disconnect method is optional (for NotifyDisc)
template <typename MSG, typename SUBSCRIBER>
class CallBack : public ICallBack<MSG>
{
public:
    // A way in cpp to enforce the function to be a member function of SUBSCRIBER
    using ActionMethod = void(SUBSCRIBER::*)(const MSG&);
    using DisconnectMethod = void(SUBSCRIBER::*)();

    explicit CallBack
    (
        Dispatcher<MSG>* disp_,
        SUBSCRIBER& sub_,
        ActionMethod ActionMethod_,
        DisconnectMethod DisMethod_= nullptr
    );
    ~CallBack();

    void Notify(const MSG& msg_) override;

private:
    void NotifyDisc() override; 
    SUBSCRIBER& m_sub;
    ActionMethod m_actionMethod;
    DisconnectMethod m_disMethod;

}; // class CallBack

//*******************************************************************
//**************************** Dispatcher ***************************
//*******************************************************************

// public
template <typename MSG>
Dispatcher<MSG>::~Dispatcher()
{
    NotifyDiscAll();
}




template <typename MSG>
void Dispatcher<MSG>::NotifyAll(const MSG& msg_)
{
    std::for_each
    (
        m_subscribers.begin(),
        m_subscribers.end(),
        [&](ICallBack<MSG> *cb_)
        {
            cb_->Notify(msg_);
        }
    );
}



// private
template <typename MSG>
void Dispatcher<MSG>::NotifyDiscAll()
{
    std::for_each
    (
        m_subscribers.begin(),
        m_subscribers.end(),
        [](ICallBack<MSG> *cb_)
        {
            cb_->NotifyDisc();
        }
    );
}



template <typename MSG>
void Dispatcher<MSG>::Subscribe(ICallBack<MSG>* cb_)
{
    try
    {
        m_subscribers.push_back(cb_);
    }
    catch(const std::bad_alloc& ba_)
    {
        std::cerr << ba_.what() << std::endl;
    }
}





template <typename MSG>
void Dispatcher<MSG>::UnSubscribe(ICallBack<MSG>* cb_)
{
    m_subscribers.erase
    (
        std::find
        (
            m_subscribers.begin(),
            m_subscribers.end(),
            cb_
        )
    );
}




template <typename MSG>
Dispatcher<MSG> *ICallBack<MSG>::GetDisp()
{
    return m_dispatcher;
}


//*******************************************************************
//**************************** ICallBack ****************************
//*******************************************************************


// public
template <typename MSG>
ICallBack<MSG>::ICallBack(Dispatcher<MSG>* disp_) : m_dispatcher(disp_)
{
    m_dispatcher->Subscribe(this);
}



template <typename MSG>
ICallBack<MSG>::~ICallBack()
{
    m_dispatcher->UnSubscribe(this);
}



//*******************************************************************
//**************************** Callback *****************************
//*******************************************************************

//public
template <typename MSG, typename SUBSCRIBER>
CallBack<MSG, SUBSCRIBER>::CallBack
(
    Dispatcher<MSG> *disp_,
    SUBSCRIBER& sub_,
    ActionMethod actionMethod_,
    DisconnectMethod disMethod_
) :
    ICallBack<MSG>::ICallBack(disp_),
    m_sub(sub_),
    m_actionMethod(actionMethod_),
    m_disMethod(disMethod_)
{

}




template <typename MSG, typename SUBSCRIBER>
CallBack<MSG, SUBSCRIBER>::~CallBack()
{
    if (nullptr != m_disMethod)
    {
        (m_sub.*m_disMethod)();
    }
}




template <typename MSG, typename SUBSCRIBER>
void CallBack<MSG, SUBSCRIBER>::Notify(const MSG& msg_)
{
    (m_sub.*m_actionMethod)(msg_);
}




// private
template <typename MSG, typename SUBSCRIBER>
void CallBack<MSG, SUBSCRIBER>::NotifyDisc()
{
    (m_sub.*m_disMethod)();
}




} // namespace ilrd

#endif // __ILRD_HRD32_MSG_BROKER_HPP__
