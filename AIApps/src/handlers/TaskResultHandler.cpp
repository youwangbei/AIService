#include "../include/handlers/TaskResultHandler.h"
#include "../../HttpServer/include/http/HttpResponse.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 处理获取异步任务结果请求
void TaskResultHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp) {
    try {
        std::string taskId = req.getQueryParameters("taskId");
        if (taskId.empty()) {
            json errorResp;
            errorResp["success"] = false;
            errorResp["message"] = "taskId is required";
            std::string errorBody = errorResp.dump(4);
            resp->setStatusLine(req.getVersion(), http::HttpResponse::BadRequest400, "Bad Request");
            resp->setContentType("application/json");
            resp->setContentLength(errorBody.size());
            resp->setBody(errorBody);
            return;
        }

        auto taskResult = AsyncTaskManager::instance().getTaskResult(taskId);
        
        json response;
        response["taskId"] = taskId;
        response["completed"] = taskResult.completed;
        
        if (taskResult.completed) {
            if (taskResult.error.empty()) {
                response["success"] = true;
                response["result"] = taskResult.result;
            } else {
                response["success"] = false;
                response["error"] = taskResult.error;
            }
        } else {
            response["success"] = false;
            response["message"] = "Task is still processing";
        }

        std::string responseBody = response.dump(4);
        resp->setStatusLine(req.getVersion(), http::HttpResponse::Ok200, "OK");
        resp->setContentType("application/json");
        resp->setContentLength(responseBody.size());
        resp->setBody(responseBody);
        
    } catch (const std::exception& e) {
        json errorResp;
        errorResp["success"] = false;
        errorResp["message"] = e.what();
        std::string errorBody = errorResp.dump(4);
        resp->setStatusLine(req.getVersion(), http::HttpResponse::InternalServerError500, "Internal Server Error");
        resp->setContentType("application/json");
        resp->setContentLength(errorBody.size());
        resp->setBody(errorBody);
    }
}
