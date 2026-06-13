#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include <regex>
#include <vector>

#include "RouterHandler.h"
#include "../http/HttpRequest.h"
#include "../http/HttpResponse.h"

namespace http
{
namespace router
{

// 选择注册对象式的路由处理器还是注册回调函数式的处理器取决于处理器执行的复杂程度
// 如果是简单的处理可以注册回调函数，否则注册对象式路由处理器(对象中可封装多个相关函数)
// 二者注册其一即可
class Router
{
public:
    using HandlerPtr = std::shared_ptr<RouterHandler>;                    // 处理器指针类型
    using HandlerCallback = std::function<void(const HttpRequest &, HttpResponse *)>; // 处理器回调类型

    // 路由键（请求方法 + URI）
    struct RouteKey
    {
        HttpRequest::Method method;  // HTTP请求方法
        std::string path;            // 路径

        bool operator==(const RouteKey &other) const
        {
            return method == other.method && path == other.path;
        }
    };

    // 为RouteKey定义哈希函数
    struct RouteKeyHash
    {
        size_t operator()(const RouteKey &key) const
        {
            size_t methodHash = std::hash<int>{}(static_cast<int>(key.method)); // 方法哈希
            size_t pathHash = std::hash<std::string>{}(key.path);               // 路径哈希
            return methodHash * 31 + pathHash; // 组合哈希
        }
    };

    // 注册路由处理器
    void registerHandler(HttpRequest::Method method, const std::string &path, HandlerPtr handler);

    // 注册回调函数形式的处理器
    void registerCallback(HttpRequest::Method method, const std::string &path, const HandlerCallback &callback);

    // 注册动态路由处理器（支持正则）
    void addRegexHandler(HttpRequest::Method method, const std::string &path, HandlerPtr handler)
    {
        std::regex pathRegex = convertToRegex(path);
        regexHandlers_.emplace_back(method, pathRegex, handler);
    }

    // 注册动态路由处理函数（支持正则）
    void addRegexCallback(HttpRequest::Method method, const std::string &path, const HandlerCallback &callback)
    {
        std::regex pathRegex = convertToRegex(path);
        regexCallbacks_.emplace_back(method, pathRegex, callback);
    }

    // 处理请求，路由到对应的处理器
    bool route(const HttpRequest &req, HttpResponse *resp);

private:
    // 将路径模式转换为正则表达式，支持匹配任意路径参数
    std::regex convertToRegex(const std::string &pathPattern)
    { 
        std::string regexPattern = "^" + std::regex_replace(pathPattern, std::regex(R"(/:([^/]+))"), R"(/([^/]+))") + "$";
        return std::regex(regexPattern);
    }

    // 提取路径参数
    void extractPathParameters(const std::smatch &match, HttpRequest &request)
    {
        for (size_t i = 1; i < match.size(); ++i)
        {
            request.setPathParameters("param" + std::to_string(i), match[i].str());
        }
    }

private:
    // 正则路由回调对象
    struct RouteCallbackObj
    {
        HttpRequest::Method method_;      // 请求方法
        std::regex pathRegex_;            // 路径正则
        HandlerCallback callback_;        // 回调函数
        RouteCallbackObj(HttpRequest::Method method, std::regex pathRegex, const HandlerCallback &callback)
            : method_(method), pathRegex_(pathRegex), callback_(callback) {}
    };

    // 正则路由处理器对象
    struct RouteHandlerObj
    {
        HttpRequest::Method method_;      // 请求方法
        std::regex pathRegex_;            // 路径正则
        HandlerPtr handler_;              // 处理器对象
        RouteHandlerObj(HttpRequest::Method method, std::regex pathRegex, HandlerPtr handler)
            : method_(method), pathRegex_(pathRegex), handler_(handler) {}
    };

    std::unordered_map<RouteKey, HandlerPtr, RouteKeyHash>      handlers_;       // 精准匹配处理器映射
    std::unordered_map<RouteKey, HandlerCallback, RouteKeyHash> callbacks_;     // 精准匹配回调映射
    std::vector<RouteHandlerObj>                                regexHandlers_; // 正则匹配处理器列表
    std::vector<RouteCallbackObj>                               regexCallbacks_; // 正则匹配回调列表
};


} // namespace router
} // namespace http