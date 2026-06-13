#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <ctime>
#include <curl/curl.h>
#include "../../../HttpServer/include/utils/JsonUtil.h"

// AI工具注册表类，用于注册和调用AI工具
class AIToolRegistry {
public:
    // 工具函数类型定义
    using ToolFunc = std::function<json(const json&)>;

    // 构造函数
    AIToolRegistry();

    // 注册工具
    void registerTool(const std::string& name, ToolFunc func);
    // 调用工具
    json invoke(const std::string& name, const json& args) const;
    // 检查工具是否存在
    bool hasTool(const std::string& name) const;

private:
    std::unordered_map<std::string, ToolFunc> tools_;  // 工具映射

    
    // CURL写回调函数
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
    // 获取天气工具
    static json getWeather(const json& args);
    // 获取时间工具
    static json getTime(const json& args);
    // RAG查询工具
    static json queryRAG(const json& args);
};
