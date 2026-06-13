#pragma once

#include <vector>
#include <memory>
#include "Middleware.h"

namespace http 
{
namespace middleware 
{

// 中间件链类，管理和执行多个中间件
class MiddlewareChain 
{
public:
    // 添加中间件
    void addMiddleware(std::shared_ptr<Middleware> middleware);
    // 执行请求前的中间件链
    void processBefore(HttpRequest& request);
    // 执行响应后的中间件链
    void processAfter(HttpResponse& response);

private:
    std::vector<std::shared_ptr<Middleware>> middlewares_;  // 中间件列表
};

} // namespace middleware
} // namespace http
