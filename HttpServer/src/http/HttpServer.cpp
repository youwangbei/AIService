#include "../../include/http/HttpServer.h"

#include <any>
#include <functional>
#include <memory>

namespace http
{

// 默认HTTP回调函数，返回404
void defaultHttpCallback(const HttpRequest &, HttpResponse *resp)
{
    resp->setStatusCode(HttpResponse::NotFound404);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
}

// HttpServer构造函数
// port: 监听端口
// name: 服务器名称
// useSSL: 是否使用SSL
// option: TCP服务器选项
HttpServer::HttpServer(int port,
                       const std::string &name,
                       bool useSSL,
                       muduo::net::TcpServer::Option option)
    : listenAddr_(port)
    , server_(&mainLoop_, listenAddr_, name, option)
    , useSSL_(useSSL)
    , httpCallback_(std::bind(&HttpServer::handleRequest, this, std::placeholders::_1, std::placeholders::_2))
{
    initialize();
}

// 启动HTTP服务器
void HttpServer::start()
{
    LOG_WARN << "HttpServer[" << server_.name() << "] starts listening on" << server_.ipPort();
    server_.start();
    mainLoop_.loop();
}

// 初始化服务器，设置连接和消息回调
void HttpServer::initialize()
{
    server_.setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(
        std::bind(&HttpServer::onMessage, this,
                  std::placeholders::_1,
                  std::placeholders::_2,
                  std::placeholders::_3));
}

// 设置SSL配置
void HttpServer::setSslConfig(const ssl::SslConfig& config)
{
    if (useSSL_)
    {
        sslCtx_ = std::make_unique<ssl::SslContext>(config);
        if (!sslCtx_->initialize())
        {
            LOG_ERROR << "Failed to initialize SSL context";
            abort();
        }
    }
}

// 连接建立或关闭时的回调函数
void HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
        if (useSSL_)
        {
            auto sslConn = std::make_unique<ssl::SslConnection>(conn, sslCtx_.get());
            sslConn->setMessageCallback(
                std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            sslConns_[conn] = std::move(sslConn);
            sslConns_[conn]->startHandshake();
        }
        conn->setContext(HttpContext());
    }
    else 
    {
        if (useSSL_)
        {
            sslConns_.erase(conn);
        }
    }
}

// 消息到达时的回调函数
void HttpServer::onMessage(const muduo::net::TcpConnectionPtr &conn,
                           muduo::net::Buffer *buf,
                           muduo::Timestamp receiveTime)
{
    try
    {
        if (useSSL_)
        {
            LOG_INFO << "onMessage useSSL_ is true";
            auto it = sslConns_.find(conn);
            if (it != sslConns_.end())
            {
                LOG_INFO << "onMessage sslConns_ is not empty";
                it->second->onRead(conn, buf, receiveTime);

                if (!it->second->isHandshakeCompleted())
                {
                    LOG_INFO << "onMessage sslConns_ is not empty";
                    return;
                }

                muduo::net::Buffer* decryptedBuf = it->second->getDecryptedBuffer();
                if (decryptedBuf->readableBytes() == 0)
                    return;

                buf = decryptedBuf;
                LOG_INFO << "onMessage decryptedBuf is not empty";
            }
        }
        HttpContext *context = boost::any_cast<HttpContext>(conn->getMutableContext());
        if (!context->parseRequest(buf, receiveTime))
        {
            conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->shutdown();
        }
        if (context->gotAll())
        {
            onRequest(conn, context->request());
            context->reset();
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Exception in onMessage: " << e.what();
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
}

// 处理HTTP请求
void HttpServer::onRequest(const muduo::net::TcpConnectionPtr &conn, const HttpRequest &req)
{
    const std::string &connection = req.getHeader("Connection");
    bool close = ((connection == "close") ||
                  (req.getVersion() == "HTTP/1.0" && connection != "Keep-Alive"));
    HttpResponse response(close);

    httpCallback_(req, &response);

    muduo::net::Buffer buf;
    response.appendToBuffer(&buf);
    LOG_INFO << "Sending response:\n" << buf.toStringPiece().as_string();

    conn->send(&buf);
    if (response.closeConnection())
    {
        conn->shutdown();
    }
}

// 处理HTTP请求的具体逻辑
void HttpServer::handleRequest(const HttpRequest &req, HttpResponse *resp)
{
    try
    {
        HttpRequest mutableReq = req;
        middlewareChain_.processBefore(mutableReq);

        if (!router_.route(mutableReq, resp))
        {
            LOG_INFO << "请求的啥，url：" << req.method() << " " << req.path();
            LOG_INFO << "未找到路由，返回404";
            resp->setStatusCode(HttpResponse::NotFound404);
            resp->setStatusMessage("Not Found");
            resp->setCloseConnection(true);
        }

        middlewareChain_.processAfter(*resp);
    }
    catch (const HttpResponse& res) 
    {
        *resp = res;
    }
    catch (const std::exception& e) 
    {
        resp->setStatusCode(HttpResponse::InternalServerError500);
        resp->setBody(e.what());
    }
}

} // namespace http