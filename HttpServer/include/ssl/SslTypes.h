#pragma once
#include <string>

namespace ssl 
{

// SSL版本枚举
enum class SSLVersion 
{
    TLS_1_0,  // TLS 1.0版本
    TLS_1_1,  // TLS 1.1版本
    TLS_1_2,  // TLS 1.2版本
    TLS_1_3   // TLS 1.3版本
};

// SSL错误类型枚举
enum class SSLError 
{
    NONE,       // 无错误
    WANT_READ,   // 需要读操作
    WANT_WRITE,  // 需要写操作
    SYSCALL,     // 系统调用错误
    SSL,         // SSL协议错误
    UNKNOWN      // 未知错误
};

// SSL连接状态枚举
enum class SSLState 
{
    HANDSHAKE,  // 握手阶段
    ESTABLISHED, // 连接已建立
    SHUTDOWN,   // 关闭阶段
    ERROR         // 错误状态
};

} // namespace ssl
