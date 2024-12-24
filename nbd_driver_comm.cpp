#include <iostream>             // std::cerr
#include <memory>               // std::shared_ptr
#include <exception>            // std::exception
#include <string>               // std::string
#include <cassert>              // assert
#include <signal.h>             // sigfillset
#include <sys/socket.h>         // socketpair
#include <sys/ioctl.h>          // ioctl
#include <linux/nbd.h>          // nbd
#include <fcntl.h>              // open
#include <unistd.h>             // read, write
#include <arpa/inet.h>          // htonl
#include <thread>               // std::thread


#include "nbd_driver_comm.hpp"
#include "driver_data.hpp"

using namespace ilrd;

#ifdef WORDS_BIGENDIAN
u_int64_t ntohll(u_int64_t a) {
  return a;
}
#else
u_int64_t ntohll(u_int64_t a) {
  u_int32_t lo = a & 0xffffffff;
  u_int32_t hi = a >> 32U;
  lo = ntohl(lo);
  hi = ntohl(hi);
  return ((u_int64_t) lo) << 32U | hi;
}
#endif
#define htonll ntohll

/************************** Declarations *************************/

static void WriteAll(int fd_, char *to_write_, size_t size_);
static void ReadAll(int fd_, char *to_write_, size_t size_);
static ActionType NBDToActionType(int nbdType_);

/*****************************************************************/




NBDDriverComm::NBDDriverComm(const std::string& pathDevFile_, size_t size_)
try : m_devFd(NBDDriverComm::SetDevice(pathDevFile_, size_))
{
    // fds
    SetSockets(size_);

    // create thrd for communication
    m_listener = std::thread(&NBDDriverComm::Communication, this);
}
catch(const NBDDriverCommException& nbdEx_)
{
    throw NBDDriverCommException(nbdEx_.what());
}







NBDDriverComm::NBDDriverComm(const std::string& pathDevFile_, size_t sizeOfBlk_, size_t numOfBlk_)
try : m_devFd(NBDDriverComm::SetDevice(pathDevFile_, sizeOfBlk_*numOfBlk_))
{
    // fds
    SetSockets(sizeOfBlk_ * numOfBlk_);

    // create communication thrd
    m_listener = std::thread(&NBDDriverComm::Communication, this);
}

catch(const NBDDriverCommException& nbdEx_)
{
    throw NBDDriverCommException(nbdEx_.what());
}




NBDDriverComm::~NBDDriverComm() noexcept
{
    try
    {
        Disconnect();
    }

    catch(const NBDDriverCommException& nbdEx_)
    {
        std::cerr << nbdEx_.what() << std::endl;
    }
}





// May throw
std::shared_ptr<ilrd::DriverData> NBDDriverComm::RecvRequest()
{
    struct nbd_request request;
    // get request
    ssize_t bytesRead = read(m_appFd, &request, sizeof(struct nbd_request));

    if (bytesRead != sizeof(request))
    {
        throw NBDDriverCommException("read request failed");
    }

    try
    {
        // create DriverData as shared ptr
        std::shared_ptr<ilrd::DriverData> sp = std::make_shared<ilrd::DriverData>
        (
            NBDToActionType(ntohl(request.type)),
            request.handle,
            static_cast<size_t>(ntohll(request.from)),
            static_cast<size_t>(ntohl(request.len))
        );

        // if request to write then the content on the way
        if (ActionType::WRITE == sp->m_type)
        {
            char *buffer = new char[sp->m_len];
            std::cout << "write requested\n";
            // read the content
            ReadAll(m_appFd, buffer, sp->m_len);

            // put it in driver data
            std::copy
            (
                buffer,
                buffer + sp->m_len,
                sp->m_buff
            );
            delete[] buffer;
        }
        // return shared ptr
        return sp;
    }

    catch(...)
    {
        throw NBDDriverCommException("request failed");
    }
}




// SendReplay may throw read exception.
void NBDDriverComm::SendReply(std::shared_ptr<ilrd::DriverData> data_)
{
    struct nbd_reply reply;
    reply.magic = htonl(NBD_REPLY_MAGIC);
    reply.error = htonl(data_->m_status);

    try
    {
        std::copy
        (
            data_->m_handle,
            data_->m_handle + sizeof(data_->m_handle),
            reply.handle
        );

        WriteAll(m_appFd, reinterpret_cast<char *>(&reply), sizeof(struct nbd_reply));

        if (ActionType::READ == data_->m_type)
        {
            WriteAll(m_appFd, data_->m_buff, data_->m_len);
        }
    }

    catch(...)
    {
        throw NBDDriverCommException("bad_alloc");
    }
}





static void ReadAll(int fd_, char *to_write_, size_t size_)
{
    int bytesRead = 0;
    
    while (size_ > 0)
    {
        bytesRead = read(fd_, to_write_, size_);
        assert(bytesRead > 0);
        to_write_ += bytesRead;
        size_ -= bytesRead;
    }

    if (0 != size_)
    {
        throw NBDDriverCommException("read failed");
    }

    std::cout << "read done" << std::endl;
}




// May throw write exception
static void WriteAll(int fd_, char *to_write_, size_t size_)
{
    int bytesWritten = 0;
    
    while (0 < size_)
    {
        bytesWritten = write(fd_, to_write_, size_);
        assert(bytesWritten > 0);
        to_write_ += bytesWritten;
        size_ -= bytesWritten;
    }

    if (0 != size_)
    {
        throw NBDDriverCommException("read failed");
    }

    std::cout << "write done" << std::endl;
}



// Disconnect may throw close / ioctl exception.
void NBDDriverComm::Disconnect()
{
    // cleanup socket and device
    if (0 > ioctl(m_devFd, NBD_CLEAR_QUE))
    {
        throw NBDDriverCommException("cleaan up: ioctl: fail to clear que");
    }

    if (0 > ioctl(m_devFd, NBD_CLEAR_SOCK))
    {
        throw NBDDriverCommException("clean up: ioctl: fail to clear socket");
    }

    if (0 > ioctl(m_devFd, NBD_DISCONNECT))
    {
        throw NBDDriverCommException("clean up: ioctl: fail to disconnect socket");
    }    

    // join thrd of communication
    m_listener.join();

    // close fds
    if (0 > close(m_devFd))
    {
        throw NBDDriverCommException("clean up: ioctl: fail to close fd");
    }
    if (0 > close(m_kerFd))
    {
        throw NBDDriverCommException("clean up: ioctl: fail to close fd");
    }
    if (0 > close(m_appFd))
    {
        throw NBDDriverCommException("clean up: ioctl: fail to close fd");
    }
}





int NBDDriverComm::GetFD() const
{
    return m_appFd;
}





int NBDDriverComm::SetDevice(const std::string& dev_path_, size_t size_)
{
    assert(nullptr != dev_path_.c_str());
    std::cout << "open device" << std::endl;
    int retFd = open(dev_path_.c_str(), O_RDWR);

    if (0 > retFd)
    {
        throw NBDDriverCommException("open failed");
    }
    
    if (0 > ioctl(retFd, NBD_SET_SIZE, size_))
    {
        throw NBDDriverCommException("ioctl: fail tp set size");
    }

    if (0 > ioctl(retFd, NBD_CLEAR_SOCK))
    {
        throw NBDDriverCommException("ioctl: fail to clear socket");
    }

    return retFd;
}




// May throw socket exception
void NBDDriverComm::SetSockets(std::size_t size_)
{
    int sp[2] = {0};
    if (0 > socketpair(AF_UNIX, SOCK_STREAM, 0, sp))
    {
        NBDDriverCommException("socketpair failed");
    }
    m_appFd = sp[0];
    m_kerFd = sp[1];
}






// May throw sigfillset / ioctl exceptions
void NBDDriverComm::Communication()
{
    sigset_t sigset;
    if (0 > sigfillset(&sigset) || 0 > pthread_sigmask(SIG_SETMASK, &sigset, NULL))
    {
        throw NBDDriverCommException("communication thread : sigfillset fail");
    }

    if (0 > ioctl(m_devFd, NBD_SET_SOCK, m_kerFd))
    {
        throw NBDDriverCommException("communication thread : ioctl: set socket fail");
    }
    
    if (ioctl(m_devFd, NBD_DO_IT))
    {
        throw NBDDriverCommException("communication thread : ioctl: nbd do it fail");
    }
}





static ilrd::ActionType NBDToActionType(int nbdType_)
{
    switch (nbdType_)
    {
    case NBD_CMD_READ:
        return READ;

    case NBD_CMD_WRITE:
        return WRITE;

    case NBD_CMD_DISC:
        return DISCONNECT;

    case NBD_CMD_FLUSH:
        return FLUSH;

    case NBD_CMD_TRIM:
        return TRIM;
    }

    return DISCONNECT;
}
