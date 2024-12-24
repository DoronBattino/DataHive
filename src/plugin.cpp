#include <iostream>

#include "singleton.hpp"
#include "factory.hpp"
#include "i_task.hpp"


//**********************************************************************
//************************** Definitions *******************************
//**********************************************************************

using namespace ilrd;

enum Args{ONE, TWO};
enum Keys{READ, WRITE, PLUGIN};



class PluginTask : public ITask
{
public:
    PluginTask(Args arg){std::cout << "Plugin task created" << std::endl;}

    void Execute() override
    {
        std::cout << "plugin loaded successfully" << std::endl;
    }

    static std::shared_ptr<ITask> CreateTask(Args arg)
    {
        std::cout << "plugin created successfully" << std::endl;
        return std::make_shared<PluginTask>(arg);
    }
};

void LoadTask() __attribute__((constructor));





//**********************************************************************
//************************** Implementation ****************************
//**********************************************************************

void LoadTask()
{
    std::cout << "start loading" << std::endl;
    Factory<ITask, Keys, Args> *factory = Singleton<Factory<ITask, Keys, Args>>::GetInstance();
    factory->Register(PLUGIN, PluginTask::CreateTask);
    std::cout << "end loading" << std::endl;
}