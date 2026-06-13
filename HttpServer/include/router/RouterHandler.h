#pragma once
#include <string>
#include <memory>
#include "../http/HttpRequest.h"
#include "../http/HttpResponse.h"

namespace http
{
namespace router
{

// 路由处理器基类
class RouterHandler 
{
public:
    virtual ~RouterHandler() = default;
    // 处理HTTP请求的纯虚函数
    virtual void handle(const HttpRequest& req, HttpResponse* resp) = 0;
};

} // namespace router
} // namespace http