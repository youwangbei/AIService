#include "../../include/http/HttpResponse.h"

namespace http
{

// 将HTTP响应追加到缓冲区
void HttpResponse::appendToBuffer(muduo::net::Buffer* outputBuf) const
{
    char buf[32]; 
    snprintf(buf, sizeof buf, "%s %d ", httpVersion_.c_str(), statusCode_);
    
    outputBuf->append(buf);
    outputBuf->append(statusMessage_);
    outputBuf->append("\r\n");

    if (closeConnection_)
    {
        outputBuf->append("Connection: close\r\n");
    }
    else
    {
        outputBuf->append("Connection: Keep-Alive\r\n");
    }

    for (const auto& header : headers_)
    {
        outputBuf->append(header.first);
        outputBuf->append(": "); 
        outputBuf->append(header.second);
        outputBuf->append("\r\n");
    }
    outputBuf->append("\r\n");
    
    outputBuf->append(body_);
}

// 设置HTTP响应的状态行
void HttpResponse::setStatusLine(const std::string& version,
                                 HttpStatusCode statusCode,
                                 const std::string& statusMessage)
{
    httpVersion_ = version;
    statusCode_ = statusCode;
    statusMessage_ = statusMessage;
}

} // namespace http