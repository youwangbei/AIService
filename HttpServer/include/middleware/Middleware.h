#pragma once

#include "../http/HttpRequest.h"
#include "../http/HttpResponse.h"

namespace http 
{
namespace middleware 
{

// 中间件基类，定义中间件接口
class Middleware 
{
public:
    // 虚析构函数
    virtual ~Middleware() = default;
    
    // 请求处理前的拦截
    virtual void before(HttpRequest& request) = 0;
    
    // 响应处理后的拦截
    virtual void after(HttpResponse& response) = 0;
    
    // 设置下一个中间件
    void setNext(std::shared_ptr<Middleware> next) 
    {
        nextMiddleware_ = next;
    }

protected:
    std::shared_ptr<Middleware> nextMiddleware_;  // 下一个中间件
};

} // namespace middleware
} // namespace http
