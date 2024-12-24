/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       03/10/23.
*       Title:      Interface Driver Communication.
*       Desc:       Abstract class for protocol.
*       Group:      HRD30.
***************************************************************************************/


#ifndef __I_DRIVER_COMM__
#define __I_DRIVER_COMM__

#include <stdexcept>            // runtime error
#include <memory>               // shared ptr

#include "driver_data.hpp"

namespace ilrd
{

class IDriverComm
{
public:

    explicit IDriverComm() = default;
    virtual ~IDriverComm() = default;
    IDriverComm(const IDriverComm& other_) = delete;
    IDriverComm& operator=(const IDriverComm& other_) = delete;

    virtual std::shared_ptr<DriverData> RecvRequest() = 0;
    virtual void SendReply(std::shared_ptr<DriverData> data_) = 0;
    virtual int GetFD() const = 0;
    virtual void Disconnect() = 0;

};  // IDriverComm

class IDriverCommException : public std::runtime_error
{
public:
    IDriverCommException(const std::string& error_) : std::runtime_error(error_){}
};

};  // namespace ilrd

#endif  // __I_DRIVER_COMM__
