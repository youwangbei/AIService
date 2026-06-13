#include "../include/AIUtil/AIStrategy.h"
#include "../include/AIUtil/AIFactory.h"

// 构建通用策略的请求 JSON
json GenericStrategy::buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const {
    json payload;
    payload["model"] = getModel();
    json msgArray = json::array();

    for (size_t i = 0; i < messages.size(); ++i) {
        json msg;
        msg["role"] = (i % 2 == 0) ? "user" : "assistant";
        msg["content"] = messages[i].first;
        msgArray.push_back(msg);
    }
    payload["messages"] = msgArray;
    return payload;
}

// 解析通用策略的响应 JSON
std::string GenericStrategy::parseResponse(const json& response) const {
    if (!response.contains("choices") || response["choices"].empty()) {
        return "";
    }
    auto& choice = response["choices"][0];
    if (!choice.contains("message") || !choice["message"].contains("content")) {
        return "";
    }
    auto& content = choice["message"]["content"];
    if (content.is_null()) {
        return "";
    }
    return content.get<std::string>();
}

// 构建 Qwen2.5 策略的请求 JSON
json Qwen25Strategy::buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const {
    json payload;
    payload["model"] = getModel();
    json msgArray = json::array();

    for (size_t i = 0; i < messages.size(); ++i) {
        json msg;
        msg["role"] = (i % 2 == 0) ? "user" : "assistant";
        msg["content"] = messages[i].first;
        msgArray.push_back(msg);
    }
    payload["messages"] = msgArray;
    return payload;
}

// 解析 Qwen2.5 策略的响应 JSON
std::string Qwen25Strategy::parseResponse(const json& response) const {
    if (!response.contains("choices") || response["choices"].empty()) {
        return "";
    }
    auto& choice = response["choices"][0];
    if (!choice.contains("message") || !choice["message"].contains("content")) {
        return "";
    }
    auto& content = choice["message"]["content"];
    if (content.is_null()) {
        return "";
    }
    return content.get<std::string>();
}

// 注册通用策略，标识符为 "1"
static StrategyRegister<GenericStrategy> regGeneric("1");
// 注册 Qwen2.5 策略，标识符为 "2"
static StrategyRegister<Qwen25Strategy> regQwen25("2");