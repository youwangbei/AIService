#pragma once
#include "../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"


// 聊天历史处理器类
class ChatHistoryHandler : public http::router::RouterHandler
{
public:
    // 构造函数
    explicit ChatHistoryHandler(ChatServer* server) : server_(server) {}

    // 处理请求
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:

private:
    ChatServer* server_;  // 聊天服务器指针
};
