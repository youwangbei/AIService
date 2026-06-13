#include "../include/handlers/ChatSendHandler.h"
#include "../include/AIUtil/MQManager.h"
#include "../include/AIUtil/AsyncTaskManager.h"


// 处理发送聊天消息请求
void ChatSendHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
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
        std::string sessionId;
        std::string model;
        bool useRag = false;

        auto body = req.getBody();
        if (!body.empty()) {
            auto j = json::parse(body);
            if (j.contains("question")) userQuestion = j["question"];
            if (j.contains("sessionId")) sessionId = j["sessionId"];
            if (j.contains("model")) model = j["model"];
            if (j.contains("useRag")) useRag = j["useRag"].get<bool>();
            if (j.contains("async") && j["async"].get<bool>()) {
                std::string taskId = AsyncTaskManager::instance().createTask(
                    userId, username, sessionId, userQuestion, model, useRag);
                
                json asyncResp;
                asyncResp["success"] = true;
                asyncResp["taskId"] = taskId;
                asyncResp["message"] = "Task accepted, please poll for result";
                std::string asyncBody = asyncResp.dump(4);
                
                resp->setStatusLine(req.getVersion(), http::HttpResponse::Accepted202, "Accepted");
                resp->setContentType("application/json");
                resp->setContentLength(asyncBody.size());
                resp->setBody(asyncBody);
                
                json mqMessage;
                mqMessage["taskId"] = taskId;
                mqMessage["userId"] = userId;
                mqMessage["username"] = username;
                mqMessage["sessionId"] = sessionId;
                mqMessage["question"] = userQuestion;
                mqMessage["model"] = model;
                mqMessage["useRag"] = useRag;
                
                MQManager::instance().publish("ai_chat_queue", mqMessage.dump());
                return;
            }
        }


        std::shared_ptr<AIHelper> AIHelperPtr;
        {
            std::lock_guard<std::mutex> lock(server_->mutexForChatInformation);

            auto& userSessions = server_->chatInformation[userId];

            if (userSessions.find(sessionId) == userSessions.end()) {

                userSessions.emplace( 
                    sessionId,
                    std::make_shared<AIHelper>()
                );
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
