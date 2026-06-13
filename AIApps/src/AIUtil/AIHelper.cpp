#include "../include/AIUtil/AIHelper.h"

// AIHelper构造函数
AIHelper::AIHelper() {
	strategy = StrategyFactory::instance().create("1");
}

// 设置AI策略
void AIHelper::setStrategy(std::shared_ptr<AIStrategy> strat) {
	strategy = strat;
}

// 添加消息并保存到MySQL
void AIHelper::addMessage(int userId, const std::string& userName, bool is_user, const std::string& userInput, std::string sessionId) {
	long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
	messages.emplace_back(userInput, ms);
	pushMessageToMysql(userId, userName, is_user, userInput, ms, sessionId);
}

// 恢复历史消息
void AIHelper::restoreMessage(const std::string& userInput, long long ms) {
	messages.emplace_back(userInput, ms);
}

// 查询RAG数据库
std::string AIHelper::queryRAGDatabase(const std::string& query) {
	AIToolRegistry registry;
	json args;
	args["query"] = query;
	
	try {
		json result = registry.invoke("query_rag", args);
		if (result.contains("status") && result["status"].get<std::string>() == "success") {
			if (result.contains("documents") && result["documents"].is_array()) {
				std::string context;
				for (const auto& doc : result["documents"]) {
					if (doc.is_string()) {
						context += doc.get<std::string>() + "\n\n";
					}
				}
				return context;
			}
		}
	} catch (const std::exception& e) {
		std::cerr << "[AIHelper] RAG query failed: " << e.what() << std::endl;
	}
	return "";
}

// 聊天方法（默认模型）
std::string AIHelper::chat(int userId, std::string userName, std::string sessionId, std::string userQuestion, bool useRag) {
	addMessage(userId, userName, true, userQuestion, sessionId);
	
	AIConfig& config = AIConfig::instance();
	
	std::string ragContext;
	if (useRag) {
		ragContext = queryRAGDatabase(userQuestion);
	}
	
	std::string systemPrompt;
	if (!ragContext.empty()) {
		systemPrompt = "参考资料：\n" + ragContext + "\n\n";
	}
	
	systemPrompt += "你是一个智能助手，可以调用工具来帮助回答问题。\n\n"
		"可用工具列表：\n" + config.buildToolList() + "\n\n"
		"工具调用规则：\n"
		"1. 当用户的问题需要获取实时信息、查询数据库或执行特定操作时，应该调用相应的工具\n"
		"2. 如果问题可以直接回答（如常识性问题），则直接回答，不需要调用工具\n"
		"3. 如果没有合适的工具可用，也直接回答\n\n"
		"工具调用格式：\n"
		"请使用JSON格式输出工具调用，例如：\n"
		"{\"tool\":\"工具名称\",\"args\":{\"参数名\":\"参数值\"}}\n\n"
		"如果不需要调用工具，请直接用自然语言回答用户的问题，不需要输出JSON格式。";
	
	json payload;
	payload["model"] = strategy->getModel();
	
	if (!systemPrompt.empty()) {
		json systemMsg;
		systemMsg["role"] = "system";
		systemMsg["content"] = systemPrompt;
		payload["messages"].push_back(systemMsg);
	}
	
	json userMsg;
	userMsg["role"] = "user";
	userMsg["content"] = userQuestion;
	payload["messages"].push_back(userMsg);
	
	json response = request(payload);
	
	std::string aiResponse = strategy->parseResponse(response);
	
	std::string toolName, toolArgs, toolResult;
	bool hasToolCall = parseToolCall(aiResponse, toolName, toolArgs, toolResult);
	
	if (hasToolCall && !toolResult.empty()) {
		std::string summaryPrompt = config.buildToolResultPrompt(userQuestion, toolName, json::parse(toolArgs), json::parse(toolResult));
		
		json summaryPayload;
		summaryPayload["model"] = strategy->getModel();
		
		json summarySystemMsg;
		summarySystemMsg["role"] = "system";
		summarySystemMsg["content"] = "请根据以下信息，用自然、简洁的语言总结回答用户的问题。";
		summaryPayload["messages"].push_back(summarySystemMsg);
		
		json summaryUserMsg;
		summaryUserMsg["role"] = "user";
		summaryUserMsg["content"] = summaryPrompt;
		summaryPayload["messages"].push_back(summaryUserMsg);
		
		json summaryResponse = request(summaryPayload);
		aiResponse = strategy->parseResponse(summaryResponse);
	}
	
	if (!aiResponse.empty()) {
		addMessage(userId, userName, false, aiResponse, sessionId);
	}
	
	return aiResponse;
}

// 解析工具调用
bool AIHelper::parseToolCall(const std::string& response, std::string& toolName, std::string& toolArgs, std::string& toolResult) {
	std::string trimmed = response;
	size_t start = trimmed.find("```json");
	if (start != std::string::npos) {
		trimmed = trimmed.substr(start + 7);
	} else {
		start = trimmed.find("{\"tool\"");
		if (start == std::string::npos) {
			start = trimmed.find("{\"Tool\"");
		}
		if (start != std::string::npos) {
			trimmed = trimmed.substr(start);
		}
	}
	
	size_t end = trimmed.find("```");
	if (end != std::string::npos) {
		trimmed = trimmed.substr(0, end);
	}
	
	size_t lastBrace = trimmed.rfind("}");
	if (lastBrace != std::string::npos) {
		trimmed = trimmed.substr(0, lastBrace + 1);
	}
	
	try {
		json toolCall = json::parse(trimmed);
		if (toolCall.contains("tool") && toolCall["tool"].is_string()) {
			toolName = toolCall["tool"].get<std::string>();
			toolArgs = toolCall.contains("args") && toolCall["args"].is_object() 
				? toolCall["args"].dump() : "{}";
			
			AIToolRegistry registry;
			if (registry.hasTool(toolName)) {
				json argsJson = toolCall.contains("args") && toolCall["args"].is_object() 
					? toolCall["args"] : json::object();
				json result = registry.invoke(toolName, argsJson);
				toolResult = result.dump();
				return true;
			} else {
				toolResult = "{\"error\": \"未知工具: " + toolName + "\"}";
				return true;
			}
		}
	} catch (const std::exception& e) {
		std::cerr << "[AIHelper] Failed to parse tool call: " << e.what() << std::endl;
	}
	
	return false;
}

// 处理工具调用
std::string AIHelper::processToolCall(const std::string& response) {
	std::string trimmed = response;
	size_t start = trimmed.find("```json");
	if (start != std::string::npos) {
		trimmed = trimmed.substr(start + 7);
	} else {
		start = trimmed.find("{\"tool\"");
		if (start == std::string::npos) {
			start = trimmed.find("{\"Tool\"");
		}
		if (start != std::string::npos) {
			trimmed = trimmed.substr(start);
		}
	}
	
	size_t end = trimmed.find("```");
	if (end != std::string::npos) {
		trimmed = trimmed.substr(0, end);
	}
	
	size_t lastBrace = trimmed.rfind("}");
	if (lastBrace != std::string::npos) {
		trimmed = trimmed.substr(0, lastBrace + 1);
	}
	
	try {
		json toolCall = json::parse(trimmed);
		if (toolCall.contains("tool") && toolCall["tool"].is_string()) {
			std::string toolName = toolCall["tool"].get<std::string>();
			json args = toolCall.contains("args") && toolCall["args"].is_object() 
				? toolCall["args"] : json::object();
			
			AIToolRegistry registry;
			if (registry.hasTool(toolName)) {
				json result = registry.invoke(toolName, args);
				return result.dump();
			} else {
				return "[错误] 未知工具: " + toolName;
			}
		}
	} catch (const std::exception& e) {
		std::cerr << "[AIHelper] Failed to parse tool call: " << e.what() << std::endl;
	}
	
	return "";
}

// 聊天方法（指定模型）
std::string AIHelper::chat(int userId, std::string userName, std::string sessionId, std::string userQuestion, const std::string& model, bool useRag) {
	std::string strategyKey;
	if (model == "qwen3-vl" || model == "qwen3-vl:2b") {
		strategyKey = "1";
	} else if (model == "qwen2.5" || model == "qwen2.5:0.5b") {
		strategyKey = "2";
	} else {
		strategyKey = "1";
	}
	
	auto newStrategy = StrategyFactory::instance().create(strategyKey);
	setStrategy(newStrategy);
	
	return chat(userId, userName, sessionId, userQuestion, useRag);
}

// 发送请求
json AIHelper::request(const json& payload) {
	return executeCurl(payload);
}

// 获取消息列表
std::vector<std::pair<std::string, long long>> AIHelper::GetMessages() {
	return messages;
}

// 转义字符串，用于SQL
std::string AIHelper::escapeString(const std::string& input) {
	std::string result;
	for (char c : input) {
		switch (c) {
			case '\\': result += "\\\\"; break;
			case '\'': result += "\\'"; break;
			case '\"': result += "\\\""; break;
			case '\n': result += "\\n"; break;
			case '\r': result += "\\r"; break;
			case '\t': result += "\\t"; break;
			default: result += c;
		}
	}
	return result;
}

// 将消息推送到MySQL
void AIHelper::pushMessageToMysql(int userId, const std::string& userName, bool is_user, const std::string& userInput, long long ms, std::string sessionId) {
	try {
		std::string escapedContent = escapeString(userInput);
		std::string sql = "INSERT INTO chat_message (user_id, username, session_id, is_user, content, ts) "
			"VALUES (" + std::to_string(userId) + ", '" + userName + "', '" + sessionId + "', " + 
			std::to_string(is_user ? 1 : 0) + ", '" + escapedContent + "', " + std::to_string(ms) + ")";
		http::MysqlUtil util;
		util.executeUpdate(sql);
	} catch (const std::exception& e) {
		std::cerr << "[AIHelper] Failed to push message to MySQL: " << e.what() << std::endl;
	}
}

// 执行curl请求
json AIHelper::executeCurl(const json& payload) {
	CURL* curl = curl_easy_init();
	if (!curl) {
		return json{{"error", "Failed to init CURL"}};
	}

	std::string response;
	std::string url = strategy->getApiUrl();
	std::string apiKey = strategy->getApiKey();
	std::string model = strategy->getModel();

	std::cerr << "[AIHelper] API URL: " << url << std::endl;
	std::cerr << "[AIHelper] Model: " << model << std::endl;
	std::cerr << "[AIHelper] API Key: " << (apiKey.empty() ? "empty" : "set") << std::endl;

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1L);

	std::string data = payload.dump();
	std::cerr << "[AIHelper] Request payload: " << data << std::endl;
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "Accept: application/json");

	if (!apiKey.empty()) {
		std::string authHeader = "Authorization: Bearer " + apiKey;
		headers = curl_slist_append(headers, authHeader.c_str());
	}

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

	CURLcode res = curl_easy_perform(curl);

	curl_easy_cleanup(curl);
	if (headers) {
		curl_slist_free_all(headers);
	}

	if (res != CURLE_OK) {
		std::cerr << "[AIHelper] CURL request failed: " << curl_easy_strerror(res) << std::endl;
		return json{{"error", std::string("CURL request failed: ") + curl_easy_strerror(res)}};
	}

	std::cerr << "[AIHelper] Response: " << response << std::endl;

	try {
		return json::parse(response);
	} catch (const std::exception& e) {
		std::cerr << "[AIHelper] Failed to parse response: " << e.what() << std::endl;
		std::cerr << "[AIHelper] Response: " << response << std::endl;
		return json{{"error", "Failed to parse response"}};
	}
}

// curl写回调函数
size_t AIHelper::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}
