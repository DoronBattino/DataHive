/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       03/10/23.
*       Title:      NBD protocol.
*       Desc:       class to define NBD protocol.
*       Group:      HRD30.
***************************************************************************************/


#ifndef __NBD_DRIVER_COMM__
#define __NBD_DRIVER_COMM__

#include <memory>
#include <thread>

#include "i_driver_comm.hpp"

namespace ilrd
{

class NBDDriverComm : public IDriverComm
{
public:

    // Ctors may throw opening socket exception.
    explicit NBDDriverComm(const std::string& pathDevFile_, size_t size_);
    explicit NBDDriverComm(const std::string& pathDevFile_, size_t sizeOfBlk_, size_t numOfBlk_);

    ~NBDDriverComm() noexcept;
    NBDDriverComm(const NBDDriverComm& other_) = delete;
    NBDDriverComm& operator=(const NBDDriverComm& other_) = delete;

    // RecvRequest may throw read exception.
    std::shared_ptr<DriverData> RecvRequest() override;

    // SendReplay may throw read exception.
    void SendReply(std::shared_ptr<DriverData> data_) override;

    // Disconnect may throw close exception.
    void Disconnect() override;
    int GetFD() const override;
    void SetSockets(std::size_t);
    int SetDevice(const std::string& dev_path_, size_t size_);
    void Communication();

private:

    int m_devFd;
    int m_kerFd;
    int m_appFd;

    std::thread m_listener;

};  // NBDDriverComm

class NBDDriverCommException : public IDriverCommException
{
public:
    NBDDriverCommException(const std::string& error_) : IDriverCommException(error_){}
};

};  // namespace ilrd

#endif  // __NBD_DRIVER_COMM__
