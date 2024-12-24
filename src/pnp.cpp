#include <unistd.h>                 // read
#include <dlfcn.h>                  // dlopen, dlclose, dlsym

#include "pnp.hpp"
#include "logger.hpp"
#include "singleton.hpp"

using namespace ilrd;


//******************************************************************
//************************** PlugNPlay ******************************
//******************************************************************

PlugNPlay::PlugNPlay(const std::string &path_) :
    m_dispatcher(),
    m_monitor(std::unique_ptr<DirMonitor>(new DirMonitor(&m_dispatcher, path_))),
    m_loader(std::unique_ptr<SOLoader> (new SOLoader(&m_dispatcher)))
{}



//******************************************************************
//************************** SOLoader ******************************
//******************************************************************

// public:
PlugNPlay::SOLoader::SOLoader(Dispatcher<std::string> *dispatcher_) :
        m_callBack(dispatcher_, *this, &PlugNPlay::SOLoader::Load, nullptr)
{}




PlugNPlay::SOLoader::~SOLoader()
{
    std::for_each
    (
        m_dlopenHandles.begin(),
        m_dlopenHandles.end(),
        [this](void *handler)
        {
            dlclose(handler);
        }
    );
}



// private
void PlugNPlay::SOLoader::Load(const std::string &file_name_)
{
    Logger *logger = Singleton<Logger>::GetInstance();
    void *SOLoad;
    m_dlopenHandles.push_back(SOLoad);
    m_dlopenHandles[m_dlopenHandles.size()-1] = dlopen(file_name_.c_str() , RTLD_LAZY);
    if (!m_dlopenHandles[m_dlopenHandles.size()-1]) 
    {
        std::cout << dlerror() << std::endl;
        logger->Log
        (
            Logger::Level::ERROR,
            "dlopen failed",
            __FILE__,
            __LINE__
        );
        return;
    }

    logger->Log
    (
        Logger::Level::INFO,
        file_name_ + " loaded.",
        __FILE__,
        __LINE__
    );
}



//******************************************************************
//************************** DirMonitor ****************************
//******************************************************************

// public:
PlugNPlay::DirMonitor::DirMonitor(Dispatcher<std::string> *dispatcher_,
                    const std::string &path_) :
                        m_dispatcher(dispatcher_),
                        m_dirPath(path_),
                        m_monitorFd(0),
                        m_stopMonitor(false)
{
    m_logger = Singleton<Logger>::GetInstance();
    m_eventListener = std::thread(&DirMonitor::MonitorLoop, this);
}




PlugNPlay::DirMonitor::~DirMonitor()
{
    m_stopMonitor = true;
    m_eventListener.join();
    close(m_monitorFd);
}




// private:
void PlugNPlay::DirMonitor::MonitorLoop()
{
    m_monitorFd = inotify_init();
    while (!m_stopMonitor)
    {
        if (0 > inotify_add_watch(m_monitorFd, m_dirPath.c_str(), IN_CLOSE_WRITE | IN_MOVED_TO))
        {
            m_logger->Log
            (
                Logger::Level::ERROR,
                "inotify failed",
                __FILE__,
                __LINE__
            );
            return;
        }

        const size_t BUFF_SIZE = 256;
        char buff[BUFF_SIZE];
        if (0 > read(m_monitorFd, buff, BUFF_SIZE))
        {
            m_logger->Log
            (
                Logger::Level::ERROR,
                "read failed",
                __FILE__,
                __LINE__
            );
            return;
        }

        struct inotify_event *ie = reinterpret_cast<struct inotify_event *>(buff);
        const std::string name(ie->name);
        const std::string suffix(".so"); // need to check if it is a shared object
        if (0 == name.substr(name.length() - suffix.length()).compare(suffix))
        {
            m_dispatcher->NotifyAll(name);
        }
    }
    
}