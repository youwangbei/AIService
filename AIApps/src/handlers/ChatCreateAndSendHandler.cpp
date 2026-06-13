#include "../include/handlers/ChatCreateAndSendHandler.h"


// 处理创建新会话并发送消息请求
void ChatCreateAndSendHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    try
    {

        auto session = server_->getSessionManager()->getSession(req, resp);
        LOG_INFO << "session->getValue(\"isLoggedIn\") = " << session->getValue("isLoggedIn");
        if (session->getValue("isLoggedIn") != "true")
        {

            json errorResp;
            errorResp["status"] = "error";
            errorResp["message"] = "Unauthorized";
            std::string errorBody = errorResp.dump(4);

            server_->packageResp(req.getVersion(), http::HttpResponse::Unauthorized401,
                "Unauthorized", true, "application/json", errorBody.size(),
                errorBody, resp);
            return;
        }


        int userId = std::stoi(session->getValue("userId"));
        std::string username = session->getValue("username");

        std::string userQuestion;
        std::string model;
        bool useRag = false;

        auto body = req.getBody();
        if (!body.empty()) {
            auto j = json::parse(body);
            if (j.contains("content")) userQuestion = j["content"];
            else if (j.contains("question")) userQuestion = j["question"];
            if (j.contains("model")) model = j["model"];
            if (j.contains("useRag")) useRag = j["useRag"].get<bool>();
        }

        AISessionIdGenerator generator;
        std::string sessionId = generator.generate();


        std::shared_ptr<AIHelper> AIHelperPtr;
        {
            std::lock_guard<std::mutex> lock(server_->mutexForChatInformation);

            auto& userSessions = server_->chatInformation[userId];

            if (userSessions.find(sessionId) == userSessions.end()) {

                userSessions.emplace( 
                    sessionId,
                    std::make_shared<AIHelper>()
                );
                server_->sessionsIdsMap[userId].push_back(sessionId);
            }
            AIHelperPtr= userSessions[sessionId];

        }

        std::string aiInformation;
        if (model.empty()) {
            aiInformation = AIHelperPtr->chat(userId, username, sessionId, userQuestion, useRag);
        } else {
            aiInformation = AIHelperPtr->chat(userId, username, sessionId, userQuestion, model, useRag);
        }
        json successResp;
        successResp["success"] = true;
        successResp["Information"] = aiInformation;
        successResp["sessionId"] = sessionId;
        
        std::string successBody = successResp.dump(4);

        resp->setStatusLine(req.getVersion(), http::HttpResponse::Ok200, "OK");
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(successBody.size());
        resp->setBody(successBody);
        return;
    }
    catch (const std::exception& e)
    {

        json failureResp;
        failureResp["status"] = "error";
        failureResp["message"] = e.what();
        std::string failureBody = failureResp.dump(4);
        resp->setStatusLine(req.getVersion(), http::HttpResponse::BadRequest400, "Bad Request");
        resp->setCloseConnection(true);
        resp->setContentType("application/json");
        resp->setContentLength(failureBody.size());
        resp->setBody(failureBody);
    }
}
