#pragma once

#include <map>
#include <string>
#include <unordered_map>

#include <muduo/base/Timestamp.h>

namespace http
{

// HTTP请求类
class HttpRequest
{
public:
    // HTTP请求方法枚举
    enum Method
    {
        Invalid, Get, Post, Head, Put, Delete, Options
    };
    
    // 构造函数，初始化请求对象
    HttpRequest()
        : method_(Invalid)
        , version_("Unknown")
    {}
    
    // 设置请求接收时间
    void setReceiveTime(muduo::Timestamp t);
    // 获取请求接收时间
    muduo::Timestamp receiveTime() const { return receiveTime_; }
    
    // 设置HTTP请求方法
    bool setMethod(const char* start, const char* end);
    // 获取HTTP请求方法
    Method method() const { return method_; }

    // 设置请求路径
    void setPath(const char* start, const char* end);
    // 获取请求路径
    std::string path() const { return path_; }

    // 设置路径参数
    void setPathParameters(const std::string &key, const std::string &value);
    // 获取路径参数
    std::string getPathParameters(const std::string &key) const;

    // 设置查询参数（URL中的?后面的部分）
    void setQueryParameters(const char* start, const char* end);
    // 获取查询参数
    std::string getQueryParameters(const std::string &key) const;
    
    // 设置HTTP版本
    void setVersion(std::string v)
    {
        version_ = v;
    }

    // 获取HTTP版本
    std::string getVersion() const
    {
        return version_;
    }
    
    // 添加请求头
    void addHeader(const char* start, const char* colon, const char* end);
    // 获取请求头
    std::string getHeader(const std::string& field) const;

    // 获取所有请求头
    const std::map<std::string, std::string>& headers() const
    { return headers_;}

    // 设置请求体
    void setBody(const std::string& body) { content_ = body; }
    // 设置请求体（指针范围版本）
    void setBody(const char* start, const char* end) 
    { 
        if (end >= start) 
        {
            content_.assign(start, end - start); 
        }
    }
    
    // 获取请求体
    std::string getBody() const
    { return content_;}

    // 设置内容长度
    void setContentLength(uint64_t length)
    { contentLength_ = length; }
    
    // 获取内容长度
    uint64_t contentLength() const
    { return contentLength_; }

    // 交换两个HttpRequest对象的内容
    void swap(HttpRequest& that);

private:
    Method                                       method_;              // HTTP请求方法
    std::string                                  version_;             // HTTP版本
    std::string                                  path_;                // 请求路径
    std::unordered_map<std::string, std::string> pathParameters_;      // 路径参数
    std::unordered_map<std::string, std::string> queryParameters_;     // 查询参数
    muduo::Timestamp                             receiveTime_;         // 接收时间
    std::map<std::string, std::string>           headers_;             // 请求头
    std::string                                  content_;             // 请求体
    uint64_t                                     contentLength_ { 0 }; // 内容长度
};  

} // namespace http