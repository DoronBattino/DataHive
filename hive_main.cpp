#include <signal.h>             // sigaction
#include <string>               // string
#include <sys/epoll.h>          // epoll
#include <memory>               // shared ptr
#include <iostream>             // cout
#include <cstring>              // stoi


#include "i_driver_comm.hpp"
#include "i_storage.hpp"
#include "nbd_driver_comm.hpp"
#include "ram_storage.hpp"
#include "driver_data.hpp"

/************************************************************************************/

enum exitStatus
{
    SUCCESS = 0,
    FAILURE
};

static int disconnect = 0;

/************************************************************************************/

// signal handler
static void DisconnectNBD(int);

// helper functions
static void VerifyTypeOfRequest(ilrd::IDriverComm *, ilrd::IStorage *,
                                            std::shared_ptr<ilrd::DriverData>);

static exitStatus SetSigMask();

/************************************************************************************/

int main(int argc_, char **argv_)
{
    // handle signals SIGINT and SIGTERM
    if (SUCCESS != SetSigMask())
    {
        return FAILURE;
    }

    // get cmd params
    const std::string path(argv_[1]);
    size_t size = std::stoi(argv_[2]);

    // create driver_comm so can do epoll on appFd (also q from keyboard)
    ilrd::IStorage *is = new ilrd::RamStorage(size);
    ilrd::IDriverComm *idc = new ilrd::NBDDriverComm(path, size);

    // setup epoll
    const int NUM_EVENTS = 1024;

    int efd = epoll_create1(0);
    if (0 > efd)
    {
        std::cout << "epoll create error" << std::endl;
        disconnect = 1;
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = STDIN_FILENO;
    if (0 > epoll_ctl(efd, EPOLL_CTL_ADD, STDIN_FILENO, &event))
    {
        std::cout << "epoll ctl" << std::endl;
        disconnect = 1;
    }

    int appFd = idc->GetFD();
    event.data.fd = appFd;
    if (0 > epoll_ctl(efd, EPOLL_CTL_ADD, appFd, &event))
    {
        std::cout << "epoll ctl" << std::endl;
        disconnect = 1;
    }

    struct epoll_event events[NUM_EVENTS];

     try
     {
        // Event loop
        while (!disconnect)
        {
            int num_events = epoll_wait(efd, events, NUM_EVENTS, -1);
            if (-1 == num_events)
            {
                std::cout << "epoll wait error" << std::endl;
                disconnect = 1;
                break;    
            }    

            for (int i = 0; i < num_events; ++i)
            {
                //event on app fd
                if (appFd == events[i].data.fd)
                {
                    std::shared_ptr<ilrd::DriverData> requestData = idc->RecvRequest();
                    VerifyTypeOfRequest(idc, is, requestData);
                }

                //event on stdin
                else if (STDIN_FILENO == events[i].data.fd)
                {
                    char inputBuff[2];
                    if (0 > read(STDIN_FILENO, inputBuff, sizeof(inputBuff)))
                    {
                        std::cerr << "fail to read input from keyboard" << std::endl;
                    }
                    if ('q' == inputBuff[0])
                    {
                        disconnect = 1;
                    }
                    break;
                }
            }
        }
    }
    
    catch(const ilrd::IStorageException& ise_)
    {
        std::cerr << ise_.what() << std::endl;
        return FAILURE;
    }

    catch(const ilrd::IDriverCommException& idce_)
    {
        std::cerr << idce_.what() << std::endl;
        delete is;
        return FAILURE;
    }

    // clean up
    delete idc;
    delete is;

    return 0;
}




// signal handler
static void DisconnectNBD(int sig_)
{
    (void)sig_;
    disconnect = 1;
}



// helper functions
static void VerifyTypeOfRequest(ilrd::IDriverComm *idc_, ilrd::IStorage *is_,
                                                std::shared_ptr<ilrd::DriverData> sp_)
{
    try
    {
        switch (sp_->m_type)
        {
        case ilrd::ActionType::READ :

            std::cout << "read request" << std::endl;
            is_->Read(sp_);
            idc_->SendReply(sp_);
            break;
        
        case ilrd::ActionType::WRITE :
        
            std::cout << "write request" << std::endl;
            is_->Write(sp_);
            idc_->SendReply(sp_);
            break;

        case ilrd::ActionType::DISCONNECT :
        
            std::cout << "disconnect request" << std::endl;
            disconnect = 1;
            break;

        case ilrd::ActionType::FLUSH :

            std::cout << "flush request recieved" << std::endl;

        case ilrd::ActionType::TRIM :

            std::cout << "trim request recieved" << std::endl;

        default:
            break;
        }
    }

    catch(const ilrd::RamStorageException& rse_)
    {
        throw ilrd::IDriverCommException("storage error");
    }
    
}





static exitStatus SetSigMask()
{
    struct sigaction newSa, oldSa;

    newSa.sa_handler = DisconnectNBD;
    newSa.sa_flags = SA_RESTART;
    if
    (
        0 != sigemptyset(&newSa.sa_mask) ||
        0 != sigaddset(&newSa.sa_mask, SIGINT) ||
        0 != sigaddset(&newSa.sa_mask, SIGTERM)
    )
    {
        return FAILURE;
    } 

    if
    (
        0 != sigaction(SIGINT, &newSa, &oldSa) ||
        0 != sigaction(SIGTERM, &newSa, &oldSa)
    )
    {
        return FAILURE;
    }

    return SUCCESS;
}






// static void SetEpoll(struct epoll_event *ee_, int *fd_, int *ep_)
// {
//     *ep_ = epoll_create1(0);
//     if (0 > *ep_)
//     {
//         std::cout << "epoll create error" << std::endl;
//     }

//     ee_->events = EPOLLIN;
//     ee_->data.fd = STDIN_FILENO;
//     if (0 > epoll_ctl(*fd_, EPOLL_CTL_ADD, STDIN_FILENO, ee_))
//     {
//         std::cout << "epoll ctl add error" << std::endl;
//     }

//     ee_->data.fd = *fd_;
//     if (0 > epoll_ctl(*fd_, EPOLL_CTL_ADD, *fd_, ee_))
//     {
//         std::cout << "epoll ctl add error" << std::endl;
//     }
// }