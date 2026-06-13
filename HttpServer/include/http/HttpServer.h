#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "../router/Router.h"
#include "../session/SessionManager.h"
#include "../middleware/MiddlewareChain.h"
#include "../middleware/cors/CorsMiddleware.h"
#include "../ssl/SslConnection.h"
#include "../ssl/SslContext.h"

class HttpRequest;
class HttpResponse;

namespace http
{

// HTTP服务器类，基于muduo网络库实现
class HttpServer : muduo::noncopyable
{
public:
    // HTTP请求回调函数类型
    using HttpCallback = std::function<void (const http::HttpRequest&, http::HttpResponse*)>;
    
    // 构造函数，初始化HTTP服务器
    // port: 监听端口
    // name: 服务器名称
    // useSSL: 是否启用SSL
    // option: TCP服务器选项
    HttpServer(int port,
               const std::string& name,
               bool useSSL = false,
               muduo::net::TcpServer::Option option = muduo::net::TcpServer::kNoReusePort);
    
    // 设置工作线程数量
    void setThreadNum(int numThreads)
    {
        server_.setThreadNum(numThreads);
    }

    // 启动服务器
    void start();

    // 获取事件循环指针
    muduo::net::EventLoop* getLoop() const 
    { 
        return server_.getLoop(); 
    }

    // 设置HTTP回调函数
    void setHttpCallback(const HttpCallback& cb)
    {
        httpCallback_ = cb;
    }

    // 注册GET请求的回调函数
    void Get(const std::string& path, const HttpCallback& cb)
    {
        router_.registerCallback(HttpRequest::Get, path, cb);
    }
    
    // 注册GET请求的处理器对象
    void Get(const std::string& path, router::Router::HandlerPtr handler)
    {
        router_.registerHandler(HttpRequest::Get, path, handler);
    }

    // 注册POST请求的回调函数
    void Post(const std::string& path, const HttpCallback& cb)
    {
        router_.registerCallback(HttpRequest::Post, path, cb);
    }

    // 注册POST请求的处理器对象
    void Post(const std::string& path, router::Router::HandlerPtr handler)
    {
        router_.registerHandler(HttpRequest::Post, path, handler);
    }

    // 添加带正则表达式的路由处理器
    void addRoute(HttpRequest::Method method, const std::string& path, router::Router::HandlerPtr handler)
    {
        router_.addRegexHandler(method, path, handler);
    }

    // 添加带正则表达式的路由回调
    void addRoute(HttpRequest::Method method, const std::string& path, const router::Router::HandlerCallback& callback)
    {
        router_.addRegexCallback(method, path, callback);
    }

    // 设置会话管理器
    void setSessionManager(std::unique_ptr<session::SessionManager> manager)
    {
        sessionManager_ = std::move(manager);
    }

    // 获取会话管理器指针
    session::SessionManager* getSessionManager() const
    {
        return sessionManager_.get();
    }

    // 添加中间件
    void addMiddleware(std::shared_ptr<middleware::Middleware> middleware) 
    {
        middlewareChain_.addMiddleware(middleware);
    }

    // 启用或禁用SSL
    void enableSSL(bool enable) 
    {
        useSSL_ = enable;
    }

    // 设置SSL配置
    void setSslConfig(const ssl::SslConfig& config);

private:
    // 初始化服务器
    void initialize();

    // 连接建立或关闭时的回调
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    // 消息到达时的回调
    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp receiveTime);
    // 处理HTTP请求
    void onRequest(const muduo::net::TcpConnectionPtr&, const HttpRequest&);

    // 处理请求的具体逻辑
    void handleRequest(const HttpRequest& req, HttpResponse* resp);
    
private:
    muduo::net::InetAddress                      listenAddr_;       // 监听地址
    muduo::net::TcpServer                        server_;           // TCP服务器
    muduo::net::EventLoop                        mainLoop_;         // 主事件循环
    HttpCallback                                 httpCallback_;     // HTTP回调函数
    router::Router                               router_;           // 路由器
    std::unique_ptr<session::SessionManager>     sessionManager_;   // 会话管理器
    middleware::MiddlewareChain                  middlewareChain_;  // 中间件链
    std::unique_ptr<ssl::SslContext>             sslCtx_;           // SSL上下文
    bool                                         useSSL_;           // 是否使用SSL
    std::map<muduo::net::TcpConnectionPtr, std::unique_ptr<ssl::SslConnection>> sslConns_;  // SSL连接映射
};

} // namespace http