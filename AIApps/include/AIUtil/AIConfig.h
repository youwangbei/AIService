#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../../../HttpServer/include/utils/JsonUtil.h"  


// AI工具结构体，定义一个可用的AI工具
struct AITool {
    std::string name;                                // 工具名称
    std::unordered_map<std::string, std::string> params;  // 工具参数
    std::string desc;                                // 工具描述
};


// AI工具调用结构体，用于解析AI返回的工具调用
struct AIToolCall {
    std::string toolName;                            // 调用的工具名称
    json args;                                       // 工具参数
    bool isToolCall = false;                         // 是否为工具调用
};


// AI配置类，单例模式，用于管理AI服务的配置信息
class AIConfig {
public:
    // 获取单例实例
    static AIConfig& instance() {
        static AIConfig instance;
        return instance;
    }
    
    // 从文件加载配置
    bool loadFromFile(const std::string& path);
    // 构建提示词
    std::string buildPrompt(const std::string& userInput) const;
    // 解析AI响应
    AIToolCall parseAIResponse(const std::string& response) const;
    // 构建工具结果提示词
    std::string buildToolResultPrompt(const std::string& userInput,const std::string& toolName,const json& toolArgs,const json& toolResult) const;

    // 获取API密钥
    std::string getApiKey() const { return apiKey_; }
    // 获取API地址
    std::string getApiUrl() const { return apiUrl_; }
    // 获取模型名称
    std::string getModel() const { return model_; }
    // 获取提示词模板
    std::string getPromptTemplate() const { return promptTemplate_; }
    // 获取百度客户端ID
    std::string getBaiduClientId() const { return baiduClientId_; }
    // 获取百度客户端密钥
    std::string getBaiduClientSecret() const { return baiduClientSecret_; }
    // 构建工具列表字符串
    std::string buildToolList() const;

private:
    // 私有构造函数，禁止外部实例化
    AIConfig() = default;
    AIConfig(const AIConfig&) = delete;
    AIConfig& operator=(const AIConfig&) = delete;

    std::string promptTemplate_;                    // 提示词模板
    std::vector<AITool> tools_;                     // AI工具列表
    std::string apiKey_;                            // API密钥
    std::string apiUrl_;                            // API地址
    std::string model_;                             // 模型名称
    std::string baiduClientId_;                     // 百度客户端ID
    std::string baiduClientSecret_;                 // 百度客户端密钥
};
