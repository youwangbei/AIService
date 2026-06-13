#pragma once
#include <stdexcept>
#include <string>

namespace http {
namespace db {

// 数据库异常类，继承自std::runtime_error
class DbException : public std::runtime_error 
{
public:
    // 构造函数，接受字符串消息
    explicit DbException(const std::string& message) 
        : std::runtime_error(message) {}
    
    // 构造函数，接受C风格字符串消息
    explicit DbException(const char* message) 
        : std::runtime_error(message) {}
};

} // namespace db
} // namespace http
