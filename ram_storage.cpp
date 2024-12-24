#include <iostream>             // cout, endl

#include "driver_data.hpp"
#include "ram_storage.hpp"




ilrd::RamStorage::RamStorage(size_t size_) : m_size(size_)
{

    try
    {
        m_pool = new char[size_];
    }
    catch(const std::bad_alloc& ba_)
    {
        throw RamStorageException("bad_alloc");
    }
    
}




ilrd::RamStorage::~RamStorage()
{
    m_size = 0;
    delete[] m_pool;
    m_pool = nullptr;
}





void ilrd::RamStorage::Read(std::shared_ptr<ilrd::DriverData> data_) const
{
    try
    {
        std::copy
        (
            m_pool + data_->m_offset,
            m_pool + data_->m_offset + data_->m_len,
            data_->m_buff
        );
        std::cout << "read done" << std::endl;
    }
    catch(const std::bad_alloc& ba_)
    {
        throw RamStorageException("failed to read");
    }
}





void ilrd::RamStorage::Write(std::shared_ptr<ilrd::DriverData> data_)
{
    try
    {
        std::copy
        (
            data_->m_buff,
            data_->m_buff + data_->m_len,
            m_pool + data_->m_offset
        );
        std::cout << "write done" << std::endl;
    }

    catch(const std::bad_alloc& ba_)
    {
        throw RamStorageException("failed to write");
    }
    
}