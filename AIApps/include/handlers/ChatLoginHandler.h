#pragma once

#include "../../../HttpServer/include/router/RouterHandler.h"
#include "../../../HttpServer/include/utils/MysqlUtil.h"
#include "../ChatServer.h"
#include "../../../HttpServer/include/utils/JsonUtil.h"

// 聊天登录处理器类
class ChatLoginHandler : public http::router::RouterHandler
{
public:
    // 构造函数
    explicit ChatLoginHandler(ChatServer* server) : server_(server) {}

    // 处理请求
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    // 查询用户ID
    int queryUserId(const std::string& username, const std::string& password);

private:
    ChatServer* server_;            // 聊天服务器指针
    http::MysqlUtil mysqlUtil_;     // MySQL工具
};
