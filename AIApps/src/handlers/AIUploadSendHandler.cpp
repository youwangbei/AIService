#include "../include/handlers/AIUploadSendHandler.h"
#include "../include/AIUtil/AIStrategy.h"
#include "../include/AIUtil/AIFactory.h"
#include "../include/AIUtil/base64.h"
#include <curl/curl.h>
#include <sstream>

// CURL 回调函数：将响应数据写入字符串
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// 处理图像上传并使用视觉模型分析请求
void AIUploadSendHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
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

        auto body = req.getBody();
        std::string filename;
        std::string imageBase64;
        std::string userText;
        
        if (!body.empty()) {
            auto j = json::parse(body);
            if (j.contains("filename")) filename = j["filename"];
            if (j.contains("image")) imageBase64 = j["image"];
            if (j.contains("content")) userText = j["content"];
            else if (j.contains("text")) userText = j["text"];
        }

        bool hasImage = !imageBase64.empty();

        if (userText.empty() || !hasImage) {
            json errorResp;
            errorResp["success"] = false;
            errorResp["message"] = "请输入问题并选择图片";
            std::string errorBody = errorResp.dump(4);
            resp->setStatusLine(req.getVersion(), http::HttpResponse::BadRequest400, "Bad Request");
            resp->setCloseConnection(true);
            resp->setContentType("application/json");
            resp->setContentLength(errorBody.size());
            resp->setBody(errorBody);
            return;
        }

        std::string imageDataUri = "data:image/jpeg;base64," + imageBase64;

        json payload;
        payload["model"] = "qwen3-vl:2b";
        payload["stream"] = false;
        
        json imageMsg;
        imageMsg["role"] = "user";
        
        json contentItem1;
        contentItem1["type"] = "text";
        contentItem1["text"] = userText;
        
        json contentItem2;
        contentItem2["type"] = "image_url";
        contentItem2["image_url"]["url"] = imageDataUri;
        
        imageMsg["content"] = json::array({contentItem1, contentItem2});
        payload["messages"] = json::array({imageMsg});

        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to init CURL");
        }

        std::string response;
        std::string url = "http://localhost:11434/v1/chat/completions";
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        
        std::string data = payload.dump();
        LOG_INFO << "[AIUploadSendHandler] Request payload: " << data;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: application/json");
        
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);
        
        CURLcode res = curl_easy_perform(curl);
        
        curl_easy_cleanup(curl);
        if (headers) {
            curl_slist_free_all(headers);
        }
        
        if (res != CURLE_OK) {
            throw std::runtime_error(std::string("CURL request failed: ") + curl_easy_strerror(res));
        }
        
        LOG_INFO << "[AIUploadSendHandler] Response: " << response;
        
        json respJson = json::parse(response);
        std::string description;
        
        if (respJson.contains("choices") && respJson["choices"].size() > 0) {
            if (respJson["choices"][0].contains("message")) {
                description = respJson["choices"][0]["message"].value("content", "");
            }
        }
        
        if (description.empty()) {
            description = "无法识别图片内容";
        }

        json successResp;
        successResp["success"] = "ok";
        successResp["filename"] = filename;
        successResp["class_name"] = description;
        successResp["confidence"] = 0.95;

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
        LOG_ERROR << "[AIUploadSendHandler] Error: " << e.what();
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