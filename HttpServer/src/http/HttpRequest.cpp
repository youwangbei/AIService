#include "../../include/http/HttpRequest.h"

namespace http
{

// 设置请求接收时间
void HttpRequest::setReceiveTime(muduo::Timestamp t)
{
    receiveTime_ = t;
}

// 设置HTTP请求方法
bool HttpRequest::setMethod(const char *start, const char *end)
{
    assert(method_ == Invalid);
    std::string m(start, end);
    if (m == "GET")
    {
        method_ = Get;
    }
    else if (m == "POST")
    {
        method_ = Post;
    }
    else if (m == "PUT")
    {
        method_ = Put;
    }
    else if (m == "DELETE")
    {
        method_ = Delete;
    }
    else if (m == "OPTIONS")
    {
        method_ = Options;
    }
    else
    {
        method_ = Invalid;
    }

    return method_ != Invalid;
}

// 设置请求路径
void HttpRequest::setPath(const char *start, const char *end)
{
    path_.assign(start, end);
}

// 设置路径参数
void HttpRequest::setPathParameters(const std::string &key, const std::string &value)
{
    pathParameters_[key] = value;
}

// 获取路径参数
std::string HttpRequest::getPathParameters(const std::string &key) const
{
    auto it = pathParameters_.find(key);
    if (it != pathParameters_.end())
    {
        return it->second;
    }
    return "";
}

// 获取查询参数
std::string HttpRequest::getQueryParameters(const std::string &key) const
{
    auto it = queryParameters_.find(key);
    if (it != queryParameters_.end())
    {
        return it->second;
    }
    return "";
}

// 解析并设置查询参数
void HttpRequest::setQueryParameters(const char *start, const char *end)
{
    std::string argumentStr(start, end);
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;

    while ((pos = argumentStr.find('&', prev)) != std::string::npos)
    {
        std::string pair = argumentStr.substr(prev, pos - prev);
        std::string::size_type equalPos = pair.find('=');

        if (equalPos != std::string::npos)
        {
            std::string key = pair.substr(0, equalPos);
            std::string value = pair.substr(equalPos + 1);
            queryParameters_[key] = value;
        }

        prev = pos + 1;
    }

    std::string lastPair = argumentStr.substr(prev);
    std::string::size_type equalPos = lastPair.find('=');
    if (equalPos != std::string::npos)
    {
        std::string key = lastPair.substr(0, equalPos);
        std::string value = lastPair.substr(equalPos + 1);
        queryParameters_[key] = value;
    }
}

// 添加HTTP请求头
void HttpRequest::addHeader(const char *start, const char *colon, const char *end)
{
    std::string key(start, colon);
    ++colon;
    while (colon < end && isspace(*colon))
    {
        ++colon;
    }
    std::string value(colon, end);
    while (!value.empty() && isspace(value[value.size() - 1]))
    {
        value.resize(value.size() - 1);
    }
    headers_[key] = value;
}

// 获取HTTP请求头
std::string HttpRequest::getHeader(const std::string &field) const
{
    std::string result;
    auto it = headers_.find(field);
    if (it != headers_.end())
    {
        result = it->second;
    }
    return result;
}

// 交换两个HttpRequest对象的内容
void HttpRequest::swap(HttpRequest &that)
{
    std::swap(method_, that.method_);
    std::swap(path_, that.path_);
    std::swap(pathParameters_, that.pathParameters_);
    std::swap(queryParameters_, that.queryParameters_);
    std::swap(version_, that.version_);
    std::swap(headers_, that.headers_);
    std::swap(receiveTime_, that.receiveTime_);
}

} // namespace http