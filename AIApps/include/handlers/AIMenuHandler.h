#pragma once
#include "../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"


// AI菜单处理器类
class AIMenuHandler : public http::router::RouterHandler
{
public:
    // 构造函数
    explicit AIMenuHandler(ChatServer* server) : server_(server) {}

    // 处理请求
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    ChatServer* server_;  // 聊天服务器指针
};
