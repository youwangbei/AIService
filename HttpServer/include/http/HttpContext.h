#pragma once

#include <iostream>

#include <muduo/net/TcpServer.h>

#include "HttpRequest.h"

namespace http
{

// HTTP上下文类，用于解析HTTP请求
class HttpContext 
{
public:
    // HTTP请求解析状态枚举
    enum HttpRequestParseState
    {
        ExpectRequestLine,  // 期望请求行
        ExpectHeaders,      // 期望请求头
        ExpectBody,         // 期望请求体
        GotAll,             // 解析完成
    };
    
    // 构造函数，初始化解析状态
    HttpContext()
    : state_(ExpectRequestLine)
    {}

    // 解析HTTP请求
    // buf: 输入缓冲区
    // receiveTime: 接收时间
    bool parseRequest(muduo::net::Buffer* buf, muduo::Timestamp receiveTime);
    // 检查是否解析完成
    bool gotAll() const 
    { return state_ == GotAll;  }

    // 重置解析状态
    void reset()
    {
        state_ = ExpectRequestLine;
        HttpRequest dummyData;
        request_.swap(dummyData);
    }

    // 获取const的HTTP请求对象
    const HttpRequest& request() const
    { return request_;}

    // 获取可修改的HTTP请求对象
    HttpRequest& request()
    { return request_;}

private:
    // 处理请求行
    bool processRequestLine(const char* begin, const char* end);
private:
    HttpRequestParseState state_;  // 当前解析状态
    HttpRequest           request_; // HTTP请求对象
};

} // namespace http