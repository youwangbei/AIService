#pragma once
#include "../../../HttpServer/include/router/RouterHandler.h"
#include "../../../HttpServer/include/utils/MysqlUtil.h"
#include "../ChatServer.h"


// 聊天注册处理器类
class ChatRegisterHandler : public http::router::RouterHandler
{
public:
    // 构造函数
    explicit ChatRegisterHandler(ChatServer* server) : server_(server) {}

    // 处理请求
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    // 插入用户
    int insertUser(const std::string& username, const std::string& password);
    // 检查用户是否存在
    bool isUserExist(const std::string& username);
private:
    ChatServer* server_;            // 聊天服务器指针
    http::MysqlUtil mysqlUtil_;     // MySQL工具
};
