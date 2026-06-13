#include "../include/handlers/ChatSpeechHandler.h"
#include "../include/AIUtil/LocalTTSProcessor.h"


// 处理语音合成请求
void ChatSpeechHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
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


        std::string text;
        std::string engine = "pyttsx3";

        auto body = req.getBody();
        if (!body.empty()) {
            auto j = json::parse(body);
            if (j.contains("text")) text = j["text"];
            if (j.contains("engine")) engine = j["engine"];
        }


        std::string speechUrl;

        try {
            LocalTTSProcessor localTTS("http://127.0.0.1:5000", 15);
            std::string audioBase64 = localTTS.synthesize(text, engine, "zh-CN");
            if (!audioBase64.empty()) {
                speechUrl = "data:audio/wav;base64," + audioBase64;
            }
        } catch (const std::exception& e) {
            LOG_WARN << "Local TTS failed: " << e.what();
        }

        if (speechUrl.empty()) {
            std::string clientId = AIConfig::instance().getBaiduClientId();
            std::string clientSecret = AIConfig::instance().getBaiduClientSecret();

            if (!clientSecret.empty() && !clientId.empty()) {
                AISpeechProcessor speechProcessor(clientId, clientSecret);
                speechUrl = speechProcessor.synthesize(text, "mp3-16k", "zh", 5, 5, 5);
            } else {
                throw std::runtime_error("语音合成服务不可用：请启动本地 TTS 服务 (python3 AIApps/resource/tts/tts_server.py) 或配置百度 API 凭证");
            }
        }

        json successResp;
        successResp["success"] = true;
        successResp["url"] = speechUrl;
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









