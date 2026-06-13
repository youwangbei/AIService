#pragma once
#include "../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

// AI CV上传发送处理器类
class AICVUploadSendHandler : public http::router::RouterHandler
{
public:
    // 构造函数
    explicit AICVUploadSendHandler(ChatServer* server) : server_(server) {}

    // 处理请求
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    ChatServer* server_;  // 聊天服务器指针
};
