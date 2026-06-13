#include "../include/AIUtil/AIToolRegistry.h"
#include "../include/AIUtil/RAGClient.h"
#include <sstream>
#include <memory>


// 构造函数：注册默认工具
AIToolRegistry::AIToolRegistry() {
    registerTool("get_weather", getWeather);
    registerTool("get_time", getTime);
    registerTool("query_rag", queryRAG);
}

// 注册工具
void AIToolRegistry::registerTool(const std::string& name, ToolFunc func) {
    tools_[name] = func;
}


// 调用指定工具
json AIToolRegistry::invoke(const std::string& name, const json& args) const {
    auto it = tools_.find(name);
    if (it == tools_.end()) {
        throw std::runtime_error("Tool not found: " + name);
    }
    return it->second(args);
}


// 检查是否存在指定工具
bool AIToolRegistry::hasTool(const std::string& name) const {
    return tools_.count(name) > 0;
}


// CURL 回调函数：将响应数据写入字符串
size_t AIToolRegistry::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}


// 天气查询工具：获取指定城市的天气信息
json AIToolRegistry::getWeather(const json& args) {
    if (!args.contains("city")) {
        return json{ {"error", "Missing parameter: city"} };
    }

    std::string city = args["city"].get<std::string>();
    std::string encodedCity;

    
    char* encoded = curl_easy_escape(nullptr, city.c_str(), city.length());
    if (encoded) {
        encodedCity = encoded;
        curl_free(encoded);
    }
    else {
        return json{ {"error", "URL encode failed"} };
    }

    std::string url = "https://wttr.in/" + encodedCity + "?format=3&lang=zh";

    CURL* curl = curl_easy_init();
    std::string response;

    if (!curl) {
        return json{ {"error", "Failed to init CURL"} };
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return json{ {"error", "CURL request failed"} };
    }

    
    return json{ {"city", city}, {"weather", response} };
}


// 时间查询工具：获取当前时间
json AIToolRegistry::getTime(const json& args) {
    (void)args;
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now);
    return json{ {"time", buffer} };
}

// RAG 检索工具：从知识库中检索相关信息
json AIToolRegistry::queryRAG(const json& args) {
    if (!args.contains("query")) {
        return json{ {"error", "Missing parameter: query"} };
    }

    static std::unique_ptr<RAGClient> client = nullptr;
    if (!client) {
        client = std::make_unique<RAGClient>("localhost:50051");
    }

    std::string query = args["query"].get<std::string>();
    int top_k = args.contains("top_k") ? args["top_k"].get<int>() : 3;
    
    return client->Query(query, top_k);
}
