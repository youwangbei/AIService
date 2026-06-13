#pragma once
#include "../../../HttpServer/include/router/RouterHandler.h"
#include "../AIUtil/AsyncTaskManager.h"

// 任务结果处理器类
class TaskResultHandler : public http::router::RouterHandler {
public:
    // 处理请求
    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
};
