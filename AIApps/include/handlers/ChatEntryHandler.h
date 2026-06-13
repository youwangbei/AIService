#pragma once
#include "../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

// 聊天入口处理器类
class ChatEntryHandler : public http::router::RouterHandler
{
public:
    // 构造函数
    explicit ChatEntryHandler(ChatServer* server) : server_(server) {}

    // 处理请求
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    ChatServer* server_;  // 聊天服务器指针
    /*
    http::MysqlUtil mysqlUtil_;
    bool init=false;
    */
};
