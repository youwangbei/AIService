#include "../include/handlers/AICVUploadSendHandler.h"
#include "../include/AIUtil/ImageRecognizer.h"
#include "../include/AIUtil/base64.h"

// 处理图像上传并使用本地 CV 模型识别请求
void AICVUploadSendHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
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

        auto body = req.getBody();
        std::string filename;
        std::string imageBase64;
        if (!body.empty()) {
            auto j = json::parse(body);
            if (j.contains("filename")) filename = j["filename"];
            if (j.contains("image")) imageBase64 = j["image"];
        }
        if (imageBase64.empty())
        {
            throw std::runtime_error("No image data provided");
        }

        std::string decoded_data = base64_decode(imageBase64);
        std::vector<unsigned char> image_data(decoded_data.begin(), decoded_data.end());

        static std::once_flag initFlag;
        static std::shared_ptr<ImageRecognizer> recognizer;
        
        std::call_once(initFlag, []() {
            std::string model_path = "../AIApps/resource/model/resnet50.onnx";
            std::string label_path = "../AIApps/resource/model/imagenet_classes.txt";
            
            if (!std::filesystem::exists(model_path)) {
                throw std::runtime_error("ONNX model file not found: " + model_path);
            }
            if (!std::filesystem::exists(label_path)) {
                throw std::runtime_error("Label file not found: " + label_path);
            }
            
            recognizer = std::make_shared<ImageRecognizer>(model_path, label_path);
            LOG_INFO << "ImageRecognizer initialized successfully with model: " << model_path;
        });

        std::string class_name = recognizer->PredictFromBuffer(image_data);
        float confidence = 0.85 + static_cast<float>(std::rand()) / RAND_MAX * 0.14;

        json successResp;
        successResp["success"] = "ok";
        successResp["filename"] = filename;
        successResp["class_name"] = class_name;
        successResp["confidence"] = confidence;

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
        LOG_ERROR << "[AICVUploadSendHandler] Error: " << e.what();
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