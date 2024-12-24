/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       03/10/23.
*       Title:      Ram Storage.
*       Desc:       class to define the RAM storage device
*       Group:      HRD30.
***************************************************************************************/


#ifndef __RAM_STORAGE__
#define __RAM_STORAGE__


#include "i_storage.hpp"

namespace ilrd
{

class RamStorage : public IStorage
{
public:

    explicit RamStorage(size_t size_);
    RamStorage(const RamStorage& other_)  = delete;
    ~RamStorage();

    void Read(std::shared_ptr<DriverData> data_) const override;
    void Write(std::shared_ptr<DriverData> data_) override;

private:

    size_t m_size;
    char *m_pool;

};  // RamStorage

class RamStorageException : public IStorageException
{
public:
    RamStorageException(const std::string& error_) : IStorageException(error_){}
};

};  // namespace ilrd

#endif  // __RAM_STORAGE__
