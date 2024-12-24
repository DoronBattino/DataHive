/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       03/10/23.
*       Title:      Interface for tasks.
*       Desc:       
*       Group:      HRD30.
***************************************************************************************/


#ifndef __ILRD_I_TASK_HPP__
#define __ILRD_I_TASK_HPP__


namespace ilrd
{

enum class Priority
{
    LOW = 0,
    MED,
    HIGH,
    // Note: admin needs to stay the highest priority
    ADMIN
};


class ITask
{
public:
    
    explicit ITask(Priority p_ = Priority::LOW);
    ITask(const ITask& other_) = default;
    ITask(ITask&& other_) = default;
    ITask& operator=(const ITask& other_) = delete;
    ITask& operator=(ITask&& other_) = delete;
    virtual ~ITask() = default;

    bool operator<(const ITask& other_) const;
    virtual void Execute() = 0;

private:

    Priority m_priority;

}; // class ITask

} // namespace ilrd

#endif //__ILRD_I_TASK_HPP__
