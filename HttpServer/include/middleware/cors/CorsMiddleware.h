#pragma once

#include "../Middleware.h"
#include "../../http/HttpRequest.h"
#include "../../http/HttpResponse.h"
#include "CorsConfig.h"

namespace http 
{
namespace middleware 
{

// CORS中间件类，处理跨域资源共享
class CorsMiddleware : public Middleware 
{
public:
    // 构造函数，接受CORS配置
    explicit CorsMiddleware(const CorsConfig& config = CorsConfig::defaultConfig());
    
    // 请求前处理
    void before(HttpRequest& request) override;
    // 响应后处理
    void after(HttpResponse& response) override;

    // 工具函数：用分隔符连接字符串列表
    std::string join(const std::vector<std::string>& strings, const std::string& delimiter);

private:
    // 检查来源是否被允许
    bool isOriginAllowed(const std::string& origin) const;
    // 处理预检请求
    void handlePreflightRequest(const HttpRequest& request, HttpResponse& response);
    // 添加CORS响应头
    void addCorsHeaders(HttpResponse& response, const std::string& origin);

private:
    CorsConfig config_;  // CORS配置
};

} // namespace middleware
} // namespace http
