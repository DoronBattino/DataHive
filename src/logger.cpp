#include "logger.hpp"

namespace ilrd
{


// also start thread
Logger::Logger(Level level_, const std::string& filePath_) :
                m_level(level_),
                m_file(filePath_.c_str(), std::ofstream::app),
                m_writeStop(0)
{

}





Logger::~Logger()
{
    while (!m_waitQ.Empty());
    m_writeStop = 1;

    if (m_logWriter.joinable())
    {
        try
        {
            m_logWriter.join();
        }

        catch(const std::system_error& se_)
        {
            std::cerr << se_.what() << std::endl;
        }
        
    }
}





void Logger::LogWrite()
{
    while(!m_writeStop)
    {
        std::string toLog;
        if (true == m_waitQ.Pop(std::chrono::milliseconds(2), toLog))
        {
            m_file << toLog << "\n";
        }
    }
}





const std::string Logger::CreateMsg
(
    const std::string& msg_,
    const std::string& filePath_,
    std::size_t line_
)
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%d-%m-%Y %X");

    return
    (
        ss.str() +
        " line: " + std::to_string(line_) +
        " file: " + filePath_ +
        " message: " + msg_
    );
}





void Logger::Log
(
    Level level_,
    const std::string& msg_,
    const std::string& filePath_,
    std::size_t line_
)
{
    std::unique_lock lock(m_mutex);
    if (level_ < m_level)
    {
        return;
    }
    lock.unlock();

    if (false == m_logWriter.joinable())
    {
        m_logWriter = std::thread(&Logger::LogWrite, this);
    }

    std::string msg(CreateMsg(msg_, filePath_, line_));
    m_waitQ.Push(msg);
}





void Logger::SetLogFile(const std::string& filePath_)
{
    while (!m_waitQ.Empty());

    std::unique_lock lock(m_mutex);
    m_file.close();
    m_file.open(filePath_, std::ofstream::app);
}





void Logger::SetCurrLevel(Level level_)
{
    std::unique_lock lock(m_mutex);
    m_level = level_;
}


} // namespace ilrd



