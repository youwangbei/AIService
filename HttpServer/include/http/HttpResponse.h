#pragma once

#include <muduo/net/TcpServer.h>

namespace http
{

// HTTP响应类
class HttpResponse 
{
public:
    // HTTP状态码枚举
    enum HttpStatusCode
    {
        Unknown,
        Ok200 = 200,
        Accepted202 = 202,
        NoContent204 = 204,
        MovedPermanently301 = 301,
        BadRequest400 = 400,
        Unauthorized401 = 401,
        Forbidden403 = 403,
        NotFound404 = 404,
        Conflict409 = 409,
        InternalServerError500 = 500,
    };

    // 构造函数
    // close: 是否关闭连接
    HttpResponse(bool close = true)
        : statusCode_(Unknown)
        , closeConnection_(close)
    {}

    // 设置HTTP版本
    void setVersion(std::string version)
    { httpVersion_ = version; }
    // 设置状态码
    void setStatusCode(HttpStatusCode code)
    { statusCode_ = code; }

    // 获取状态码
    HttpStatusCode getStatusCode() const
    { return statusCode_; }

    // 设置状态消息
    void setStatusMessage(const std::string message)
    { statusMessage_ = message; }

    // 设置是否关闭连接
    void setCloseConnection(bool on)
    { closeConnection_ = on; }

    // 获取是否关闭连接
    bool closeConnection() const
    { return closeConnection_; }
    
    // 设置Content-Type响应头
    void setContentType(const std::string& contentType)
    { addHeader("Content-Type", contentType); }

    // 设置Content-Length响应头
    void setContentLength(uint64_t length)
    { addHeader("Content-Length", std::to_string(length)); }

    // 添加响应头
    void addHeader(const std::string& key, const std::string& value)
    { headers_[key] = value; }
    
    // 设置响应体
    void setBody(const std::string& body)
    { 
        body_ = body;
    }

    // 设置状态行
    void setStatusLine(const std::string& version,
                         HttpStatusCode statusCode,
                         const std::string& statusMessage);

    // 设置错误头（空实现）
    void setErrorHeader(){}

    // 将响应追加到缓冲区
    void appendToBuffer(muduo::net::Buffer* outputBuf) const;
private:
    std::string                        httpVersion_;    // HTTP版本
    HttpStatusCode                     statusCode_;     // 状态码
    std::string                        statusMessage_;  // 状态消息
    bool                               closeConnection_; // 是否关闭连接
    std::map<std::string, std::string> headers_;        // 响应头
    std::string                        body_;           // 响应体
    bool                               isFile_;         // 是否是文件
};

} // namespace http