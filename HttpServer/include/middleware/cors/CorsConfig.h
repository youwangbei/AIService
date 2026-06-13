#pragma once

#include <string>
#include <vector>

namespace http 
{
namespace middleware 
{

// CORS配置结构体
struct CorsConfig 
{
    std::vector<std::string> allowedOrigins;  // 允许的来源
    std::vector<std::string> allowedMethods;  // 允许的HTTP方法
    std::vector<std::string> allowedHeaders;  // 允许的请求头
    bool allowCredentials = false;             // 是否允许凭证
    int maxAge = 3600;                         // 预检请求的最大缓存时间（秒）
    
    // 获取默认配置
    static CorsConfig defaultConfig() 
    {
        CorsConfig config;
        config.allowedOrigins = {"*"};
        config.allowedMethods = {"GET", "POST", "PUT", "DELETE", "OPTIONS"};
        config.allowedHeaders = {"Content-Type", "Authorization"};
        return config;
    }
};

} // namespace middleware
} // namespace http
