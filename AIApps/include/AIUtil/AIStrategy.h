#pragma once
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <memory>

#include "../../../HttpServer/include/utils/JsonUtil.h"



// AI策略基类，定义不同AI服务的接口
class AIStrategy {
public:
    // 虚析构函数
    virtual ~AIStrategy() = default;

    
    // 获取API地址
    virtual std::string getApiUrl() const = 0;

    // 获取API密钥
    virtual std::string getApiKey() const = 0;


    // 获取模型名称
    virtual std::string getModel() const = 0;


    // 构建请求
    virtual json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const = 0;


    // 解析响应
    virtual std::string parseResponse(const json& response) const = 0;

};

// 通用策略类
class GenericStrategy : public AIStrategy {

public:
    // 构造函数，从环境变量获取配置
    GenericStrategy() {
        const char* key = std::getenv("AI_API_KEY");
        apiKey_ = key ? key : "ollama";
        
        const char* url = std::getenv("AI_API_URL");
        apiUrl_ = url ? url : "http://localhost:11434/v1/chat/completions";
        
        const char* model = std::getenv("AI_MODEL");
        model_ = model ? model : "qwen3-vl:2b";
    }

    // 获取API地址
    std::string getApiUrl() const override { return apiUrl_; }
    // 获取API密钥
    std::string getApiKey() const override { return apiKey_; }
    // 获取模型名称
    std::string getModel() const override { return model_; }

    // 构建请求
    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    // 解析响应
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;        // API密钥
    std::string apiUrl_;        // API地址
    std::string model_;         // 模型名称
};

// Qwen2.5策略类
class Qwen25Strategy : public AIStrategy {

public:
    // 构造函数，从环境变量获取配置
    Qwen25Strategy() {
        const char* key = std::getenv("AI_API_KEY");
        apiKey_ = key ? key : "ollama";
        
        const char* url = std::getenv("AI_API_URL");
        apiUrl_ = url ? url : "http://localhost:11434/v1/chat/completions";
        
        const char* model = std::getenv("AI_MODEL_QWEN25");
        model_ = model ? model : "qwen2.5:0.5b";
    }

    // 获取API地址
    std::string getApiUrl() const override { return apiUrl_; }
    // 获取API密钥
    std::string getApiKey() const override { return apiKey_; }
    // 获取模型名称
    std::string getModel() const override { return model_; }

    // 构建请求
    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    // 解析响应
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;        // API密钥
    std::string apiUrl_;        // API地址
    std::string model_;         // 模型名称
};
