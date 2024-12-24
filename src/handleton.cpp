#include <iostream>
#include <dlfcn.h>

#include "singleton.hpp"
using namespace ilrd;

// class Test
// {
// public:
//     void DoH(){std::cout << "this function is from header file" << std::endl;}
// };

typedef void (*PrintFunc)();

/*******************************************************************************/


int main()
{
    Test *t = ilrd::Singleton<Test>::GetInstance();
    std::cout << "singleton address is: " << t << std::endl;

    void *libFunc = dlopen("./handletonlib.so", RTLD_LAZY);

    if (!libFunc)
    {
        std::cerr << "fail to load shared object" << dlerror() << std::endl;
        return 1;
    }

    PrintFunc pf = reinterpret_cast<PrintFunc>(dlsym(libFunc, "DoLib"));
    if (!pf)
    {
        std::cerr << "fail to get print function" << dlerror() << std::endl;
        dlclose(libFunc);
        return 1;
    }

    pf();
    dlclose(libFunc);

    return 0;
}