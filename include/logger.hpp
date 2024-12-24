/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       03/10/23.
*       Title:      Logger.
*       Desc:       
*       Group:      HRD30.
***************************************************************************************/


#ifndef __ILRD_LOGGER_HPP__
#define __ILRD_LOGGER_HPP__

#include <string>           // string
#include <mutex>            // mutex
#include <thread>           // std::thread
#include <fstream>          // ofstream
#include <chrono>           // system_time
#include <iomanip>          // put_time
#include <sstream>          // stringstream

#include "singleton.hpp"
#include "waitable_queue.hpp"

namespace ilrd
{
    

class Logger
{
public:

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;
    ~Logger();

    enum class Level
    {
        INFO = 0,
        WARNING,
        ERROR
    };

    void Log(Level level_, const std::string& msg_, const std::string& filePath_, std::size_t line_);
    void SetLogFile(const std::string& filePath_); // needs to support thrd safe
    void SetCurrLevel(Level level_); // needs to support thrd safe

private:

    Logger(Level level_ = Level::ERROR, const std::string& filePath_ = "log_file.txt");

    static const std::string CreateMsg
    (
        const std::string& msg_,
        const std::string& filePath_,
        std::size_t line_
    );


    void LogWrite();

    Level m_level;
    std::ofstream m_file;

    std::size_t m_writeStop;
   
    std::mutex m_mutex;
    std::thread m_logWriter;
    
    WaitableQueue<std::string> m_waitQ;
    
    friend class Singleton<Logger>;

};// class Logger

} // namespace ilrd

#endif // __ILRD_LOGGER_HPP__
