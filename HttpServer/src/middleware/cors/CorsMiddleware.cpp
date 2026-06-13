#include "../../../include/middleware/cors/CorsMiddleware.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <muduo/base/Logging.h>

namespace http 
{
namespace middleware 
{

// 构造CORS中间件，初始化配置
CorsMiddleware::CorsMiddleware(const CorsConfig& config) : config_(config) {}

// 在请求处理前执行，处理CORS预检请求
void CorsMiddleware::before(HttpRequest& request) 
{
    LOG_DEBUG << "CorsMiddleware::before - Processing request";
    
    if (request.method() == HttpRequest::Method::Options) 
    {
        LOG_INFO << "Processing CORS preflight request";
        HttpResponse response;
        handlePreflightRequest(request, response);
        throw response;
    }
}

// 在响应处理后执行，添加CORS响应头
void CorsMiddleware::after(HttpResponse& response) 
{
    LOG_DEBUG << "CorsMiddleware::after - Processing response";
    
    if (!config_.allowedOrigins.empty()) 
    {
        if (std::find(config_.allowedOrigins.begin(), config_.allowedOrigins.end(), "*") 
            != config_.allowedOrigins.end()) 
        {
            addCorsHeaders(response, "*");
        } 
        else 
        {
            addCorsHeaders(response, config_.allowedOrigins[0]);
        }
    }
}

// 检查指定的Origin是否被允许
bool CorsMiddleware::isOriginAllowed(const std::string& origin) const 
{
    return config_.allowedOrigins.empty() || 
           std::find(config_.allowedOrigins.begin(), 
                    config_.allowedOrigins.end(), "*") != config_.allowedOrigins.end() ||
           std::find(config_.allowedOrigins.begin(), 
                    config_.allowedOrigins.end(), origin) != config_.allowedOrigins.end();
}

// 处理CORS预检请求
void CorsMiddleware::handlePreflightRequest(const HttpRequest& request, 
                                          HttpResponse& response) 
{
    const std::string& origin = request.getHeader("Origin");
    
    if (!isOriginAllowed(origin)) 
    {
        LOG_WARN << "Origin not allowed: " << origin;
        response.setStatusCode(HttpResponse::Forbidden403);
        return;
    }

    addCorsHeaders(response, origin);
    response.setStatusCode(HttpResponse::NoContent204);
    LOG_INFO << "Preflight request processed successfully";
}

// 为响应添加CORS相关的HTTP头
void CorsMiddleware::addCorsHeaders(HttpResponse& response, 
                                  const std::string& origin) 
{
    try 
    {
        response.addHeader("Access-Control-Allow-Origin", origin);
        
        if (config_.allowCredentials) 
        {
            response.addHeader("Access-Control-Allow-Credentials", "true");
        }
        
        if (!config_.allowedMethods.empty()) 
        {
            response.addHeader("Access-Control-Allow-Methods", 
                             join(config_.allowedMethods, ", "));
        }
        
        if (!config_.allowedHeaders.empty()) 
        {
            response.addHeader("Access-Control-Allow-Headers", 
                             join(config_.allowedHeaders, ", "));
        }
        
        response.addHeader("Access-Control-Max-Age", 
                          std::to_string(config_.maxAge));
        
        LOG_DEBUG << "CORS headers added successfully";
    } 
    catch (const std::exception& e) 
    {
        LOG_ERROR << "Error adding CORS headers: " << e.what();
    }
}

// 将字符串列表用指定分隔符连接成一个字符串
std::string CorsMiddleware::join(const std::vector<std::string>& strings, const std::string& delimiter) 
{
    std::ostringstream result;
    for (size_t i = 0; i < strings.size(); ++i) 
    {
        if (i > 0) result << delimiter;
        result << strings[i];
    }
    return result.str();
}

} // namespace middleware
} // namespace http
