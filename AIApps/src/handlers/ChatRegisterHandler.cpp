#include "../include/handlers/ChatRegisterHandler.h"


// 处理用户注册请求
void ChatRegisterHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    
    json parsed = json::parse(req.getBody());
    std::string username = parsed["username"];
    std::string password = parsed["password"];


    int userId = insertUser(username, password);
    if (userId != -1)
    {

        json successResp;
        successResp["status"] = "success";
        successResp["message"] = "Register successful";
        successResp["userId"] = userId;
        std::string successBody = successResp.dump(4);

        resp->setStatusLine(req.getVersion(), http::HttpResponse::Ok200, "OK");
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(successBody.size());
        resp->setBody(successBody);
    }
    else
    {

        json failureResp;
        failureResp["status"] = "error";
        failureResp["message"] = "username already exists";
        std::string failureBody = failureResp.dump(4);

        resp->setStatusLine(req.getVersion(), http::HttpResponse::Conflict409, "Conflict");
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(failureBody.size());
        resp->setBody(failureBody);
    }
}

// 插入新用户到数据库
int ChatRegisterHandler::insertUser(const std::string& username, const std::string& password)
{

    if (!isUserExist(username))
    {

        std::string sql = "INSERT INTO users (username, password) VALUES ('" + username + "', '" + password + "')";
        mysqlUtil_.executeUpdate(sql);
        std::string sql2 = "SELECT id FROM users WHERE username = '" + username + "'";
        auto res = mysqlUtil_.executeQuery(sql2);
        if (res->next())
        {
            return res->getInt("id");
        }
    }
    return -1;
}

// 检查用户是否已存在
bool ChatRegisterHandler::isUserExist(const std::string& username)
{
    std::string sql = "SELECT id FROM users WHERE username = '" + username + "'";
    auto res = mysqlUtil_.executeQuery(sql);
    if (res->next())
    {
        return true;
    }
    return false;
}