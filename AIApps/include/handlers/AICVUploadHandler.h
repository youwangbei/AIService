#pragma once
#include "../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

// AI CV上传处理器类
class AICVUploadHandler : public http::router::RouterHandler
{
public:
    // 构造函数
    explicit AICVUploadHandler(ChatServer* server) : server_(server) {}

    // 处理请求
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    ChatServer* server_;  // 聊天服务器指针
};
