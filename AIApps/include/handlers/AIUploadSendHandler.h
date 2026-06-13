#pragma once
#include "../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

// AI上传发送处理器类
class AIUploadSendHandler : public http::router::RouterHandler
{
public:
    // 构造函数
    explicit AIUploadSendHandler(ChatServer* server) : server_(server) {}

    // 处理请求
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    ChatServer* server_;  // 聊天服务器指针
};
