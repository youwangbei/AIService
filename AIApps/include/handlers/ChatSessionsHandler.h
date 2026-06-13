#pragma once
#include "../../../HttpServer/include/router/RouterHandler.h"
#include "../../../HttpServer/include/utils/MysqlUtil.h"
#include "../ChatServer.h"

// 聊天会话处理器类
class ChatSessionsHandler : public http::router::RouterHandler
{
public:
    // 构造函数
    explicit ChatSessionsHandler(ChatServer* server) : server_(server) {}

    // 处理请求
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:

private:
    ChatServer* server_;            // 聊天服务器指针
    http::MysqlUtil mysqlUtil_;     // MySQL工具
};
