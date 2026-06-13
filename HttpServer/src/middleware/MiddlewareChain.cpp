#include "../../include/middleware/MiddlewareChain.h"
#include <muduo/base/Logging.h>

namespace http
{
namespace middleware
{

// 向中间件链添加一个中间件
void MiddlewareChain::addMiddleware(std::shared_ptr<Middleware> middleware)
{
    middlewares_.push_back(middleware);
}

// 按顺序处理HTTP请求前的中间件逻辑
void MiddlewareChain::processBefore(HttpRequest& request)
{
    for (auto& middleware : middlewares_)
    {
        middleware->before(request);
    }
}

// 按逆序处理HTTP响应后的中间件逻辑
void MiddlewareChain::processAfter(HttpResponse& response)
{
    try
    {
        for (auto it = middlewares_.rbegin(); it != middlewares_.rend(); ++it)
        {
            if (*it)
            {
                (*it)->after(response);
            }
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR << "Error in middleware after processing: " << e.what();
    }
}

} // namespace middleware
} // namespace http
