/*
*       Athour:     Doron Battino
*       Reviewer:   
*       Date:       03/10/23.
*       Title:      Driver Data.
*       Desc:       class to keep info and data from / to the protocol.
*       Group:      HRD30.
***************************************************************************************/


#ifndef __DRIVER_DATA__
#define __DRIVER_DATA__

#include <stdexcept>        // runtime error

namespace ilrd
{

enum ActionType
{
    READ = 0,
    WRITE,
    DISCONNECT,
    FLUSH,
    TRIM
};

enum Status
{
    SUCCESS = 0,
    FAILURE
};

class DriverData
{
public:

    explicit DriverData(ActionType type_, char *handle_, std::size_t offset_, std::size_t len_);
    DriverData(const DriverData& other_) = delete;
    DriverData& operator=(const DriverData& other_) = delete;
    ~DriverData();

    ActionType m_type;
    char m_handle[8];
    std::size_t m_offset;
    std::size_t m_len;
    Status m_status;
    char *m_buff;

};  // DriverDate

class DriverDataException : public std::runtime_error
{
public:
    DriverDataException(const std::string& error_) : std::runtime_error(error_){}
};

};  // namespace ilrd

#endif  // __DRIVER_DATA__
