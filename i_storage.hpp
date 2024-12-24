/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       03/10/23.
*       Title:      Interface Storage.
*       Desc:       Abstract class for the storage device.
*       Group:      HRD30.
***************************************************************************************/


#ifndef __I_STORAGE__
#define __I_STORAGE__

#include <stdexcept>            // runtime error
#include <memory>               // shared ptr

#include "driver_data.hpp"

namespace ilrd
{

class IStorage
{
public:

    IStorage() = default;
    virtual ~IStorage() = default;
    IStorage(const IStorage& other_) = delete;
    IStorage& operator=(const IStorage& other_) = delete;

    virtual void Read(std::shared_ptr<DriverData> data_) const = 0;
    virtual void Write(std::shared_ptr<DriverData> data_) = 0;

};  // IStorage

class IStorageException : public std::runtime_error
{
public:
    IStorageException(const std::string& error_) : runtime_error(error_){}
};

};  // namespace ilrd

#endif  // __I_STORAGE__
