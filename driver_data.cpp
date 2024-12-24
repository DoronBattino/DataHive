#include <new>  

#include "driver_data.hpp"


ilrd::DriverData::DriverData(ActionType type_, char *handle_, size_t offset_, size_t len_) :
    m_type(type_),
    m_offset(offset_),
    m_len(len_),
    m_status(SUCCESS)
{
    std::copy
    (
        handle_,
        handle_ + sizeof(m_handle),
        m_handle
    );

    try
    {
        m_buff = new char[len_];
    }
    catch(const std::bad_alloc& ba_)
    {
        DriverDataException dde("bad_alloc");
        throw dde;
    }
}




ilrd::DriverData::~DriverData()
{
    m_type = DISCONNECT;
    m_offset = 0;
    m_len = 0;
    delete[] m_buff;
    m_buff = nullptr;
}