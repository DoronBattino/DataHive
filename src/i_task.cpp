#include "i_task.hpp"

namespace ilrd
{


ITask::ITask(Priority p_) : m_priority(p_) {}




bool ITask::operator<(const ITask& other_) const
{
    return (m_priority < other_.m_priority);
}


} // namespace ilrd
