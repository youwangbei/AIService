#include "../include/AIUtil/MQConsumerService.h"
#include "../include/AIUtil/AsyncTaskManager.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 构造函数
MQConsumerService::MQConsumerService() {}

// 析构函数：关闭消息消费服务
MQConsumerService::~MQConsumerService() {
    shutdown();
}

// 启动消息消费服务
void MQConsumerService::start() {
    consumer_ = std::make_unique<RabbitMQThreadPool>(
        "localhost", "ai_chat_queue", 3,
        std::bind(&MQConsumerService::handleMessage, this, std::placeholders::_1)
    );
    consumer_->start();
}

// 关闭消息消费服务
void MQConsumerService::shutdown() {
    if (consumer_) {
        consumer_->shutdown();
    }
}

// 处理接收到的消息：调用 AI 助手并更新任务结果
void MQConsumerService::handleMessage(const std::string& message) {
    try {
        auto j = json::parse(message);
        std::string taskId = j["taskId"];
        int userId = j["userId"];
        std::string username = j["username"];
        std::string sessionId = j["sessionId"];
        std::string question = j["question"];
        std::string model = j.contains("model") ? j["model"].get<std::string>() : "";
        bool useRag = j.contains("useRag") ? j["useRag"].get<bool>() : false;

        std::string result;
        if (model.empty()) {
            result = aiHelper_.chat(userId, username, sessionId, question, useRag);
        } else {
            result = aiHelper_.chat(userId, username, sessionId, question, model, useRag);
        }

        AsyncTaskManager::instance().setTaskResult(taskId, result, true);
        
    } catch (const std::exception& e) {
        try {
            auto j = json::parse(message);
            AsyncTaskManager::instance().setTaskResult(j["taskId"], "", false, e.what());
        } catch (...) {}
    }
}
