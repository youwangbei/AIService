#include "../include/AIUtil/AIConfig.h"

// 从配置文件加载配置
bool AIConfig::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[AIConfig] 无法打开配置文件: " << path << std::endl;
        return false;
    }

    json j;
    file >> j;

    // Ollama 本地大模型服务配置
    apiKey_ = j.value("ai_api_key", "ollama");
    apiUrl_ = j.value("ai_api_url", "http://localhost:11434/v1/chat/completions");
    model_ = j.value("ai_model", "qwen3-vl:2b");
    
    // 百度 AI API 配置（语音合成等）
    baiduClientId_ = j.value("baidu_client_id", "");
    baiduClientSecret_ = j.value("baidu_client_secret", "");
    
    // 环境变量优先（用于 Docker 部署）
    const char* envKey = std::getenv("AI_API_KEY");
    if (envKey) apiKey_ = envKey;
    const char* envUrl = std::getenv("AI_API_URL");
    if (envUrl) apiUrl_ = envUrl;
    const char* envModel = std::getenv("AI_MODEL");
    if (envModel) model_ = envModel;

    if (!j.contains("prompt_template") || !j["prompt_template"].is_string()) {
        std::cerr << "[AIConfig] 缺少 prompt_template 配置" << std::endl;
        return false;
    }
    promptTemplate_ = j["prompt_template"].get<std::string>();

    if (j.contains("tools") && j["tools"].is_array()) {
        for (auto& tool : j["tools"]) {
            AITool t;
            t.name = tool.value("name", "");
            t.desc = tool.value("desc", "");
            if (tool.contains("params") && tool["params"].is_object()) {
                for (auto& [key, val] : tool["params"].items()) {
                    t.params[key] = val.get<std::string>();
                }
            }
            tools_.push_back(std::move(t));
        }
    }
    return true;
}

// 构建工具列表字符串
std::string AIConfig::buildToolList() const {
    std::ostringstream oss;
    for (const auto& t : tools_) {
        oss << t.name << "(";
        bool first = true;
        for (const auto& [key, val] : t.params) {
            if (!first) oss << ", ";
            oss << key;
            first = false;
        }
        oss << ") - " << t.desc << "\n";
    }
    return oss.str();
}

// 根据用户输入构建完整提示词
std::string AIConfig::buildPrompt(const std::string& userInput) const {
    std::string result = promptTemplate_;
    
    size_t pos = result.find("{user_input}");
    if (pos != std::string::npos) {
        result.replace(pos, 12, userInput);
    }
    
    std::string toolList = buildToolList();
    pos = result.find("{tool_list}");
    if (pos != std::string::npos) {
        result.replace(pos, 11, toolList);
    }
    
    return result;
}

// 解析 AI 响应，判断是否为工具调用
AIToolCall AIConfig::parseAIResponse(const std::string& response) const {
    AIToolCall result;
    try {
        json j = json::parse(response);

        if (j.contains("tool") && j["tool"].is_string()) {
            result.toolName = j["tool"].get<std::string>();
            if (j.contains("args") && j["args"].is_object()) {
                result.args = j["args"];
            }
            result.isToolCall = true;
        }
    }
    catch (...) {
        result.isToolCall = false;
    }
    return result;
}

// 构建包含工具执行结果的提示词
std::string AIConfig::buildToolResultPrompt(
    const std::string& userInput,
    const std::string& toolName,
    const json& toolArgs,
    const json& toolResult) const {
    std::ostringstream oss;
    oss << "下面是用户说的话：" << userInput << "\n"
        << "我刚才调用了工具 [" << toolName << "] ，参数为："
        << toolArgs.dump() << "\n"
        << "工具返回的结果如下：\n" << toolResult.dump(4) << "\n"
        << "请根据以上信息，用自然语言回答用户。";
    return oss.str();
}
