/*******************************************************************************
* Author:           Doron
* Date:             3/4/24
* Version:          0
* Description:      Installs plugins
*******************************************************************************/

#ifndef __ILRD_HRD32_PNP_HPP__
#define __ILRD_HRD32_PNP_HPP__

#include <string>
#include <thread>
#include <sys/inotify.h>
#include <vector>
#include <memory>

#include "logger.hpp"
#include "msg_broker.hpp"

namespace ilrd
{

    class PlugNPlay
    {
    public:
        explicit PlugNPlay(const std::string &path_ = "./pnp");
        ~PlugNPlay() = default;
        PlugNPlay(const PlugNPlay &other_) = delete;
        PlugNPlay &operator=(const PlugNPlay &other_) = delete;

    private:
        class DirMonitor;
        class SOLoader;

        Dispatcher<std::string> m_dispatcher;
        std::unique_ptr<DirMonitor> m_monitor;
        std::unique_ptr<SOLoader> m_loader;

    }; // class PlugNPlay





    class PlugNPlay::DirMonitor
    {
    public:
        explicit DirMonitor(Dispatcher<std::string> *dispatcher_,
                            const std::string &path_ = "./pnp");
        ~DirMonitor();
        DirMonitor(const DirMonitor &other_) = delete;
        DirMonitor &operator=(const DirMonitor &other_) = delete;

    private:
        Dispatcher<std::string> *m_dispatcher;
        std::thread m_eventListener;
        const std::string m_dirPath;
        int m_monitorFd;
        bool m_stopMonitor;
        Logger *m_logger;

        void MonitorLoop();

    }; // class DirMonitor





    class PlugNPlay::SOLoader
    {
    public:
        explicit SOLoader(Dispatcher<std::string> *dispatcher_);
        ~SOLoader();
        SOLoader(const SOLoader &other_) = delete;
        SOLoader &operator=(const SOLoader &other_) = delete;

    private:
        CallBack<std::string, SOLoader> m_callBack;
        std::vector<void *> m_dlopenHandles;

        void Load(const std::string &file_name_);

    }; // class SOLoader

} // namespace ilrd

#endif // __ILRD_HRD32_PNP_HPP__