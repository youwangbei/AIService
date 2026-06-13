#pragma once
#include <string>
#include <vector>
#include <utility>
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <chrono>

#include "../../../HttpServer/include/utils/JsonUtil.h"
#include "../../../HttpServer/include/utils/MysqlUtil.h"

#include"AIFactory.h"
#include"AIConfig.h"
#include"AIToolRegistry.h"


// AI助手类，提供AI聊天相关功能
class AIHelper {
public:
	AIHelper();

	// 设置AI策略
	void setStrategy(std::shared_ptr<AIStrategy> strat);

	// 添加消息
	void addMessage(int userId, const std::string& userName, bool is_user, const std::string& userInput, std::string sessionId);
	// 恢复历史消息
	void restoreMessage(const std::string& userInput, long long ms);

	// 聊天方法（默认模型）
	std::string chat(int userId, std::string userName, std::string sessionId, std::string userQuestion, bool useRag = false);
	// 聊天方法（指定模型）
	std::string chat(int userId, std::string userName, std::string sessionId, std::string userQuestion, const std::string& model, bool useRag = false);

	// 发送请求
	json request(const json& payload);

	// 获取消息列表
	std::vector<std::pair<std::string, long long>> GetMessages();

private:
	// 查询RAG数据库
	std::string queryRAGDatabase(const std::string& query);
	// 转义字符串
	std::string escapeString(const std::string& input);
	// 将消息推送到MySQL
	void pushMessageToMysql(int userId, const std::string& userName, bool is_user, const std::string& userInput, long long ms,std::string sessionId);

	// 执行curl请求
	json executeCurl(const json& payload);
	// curl写回调函数
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
	
	// 处理工具调用
	std::string processToolCall(const std::string& response);
	// 解析工具调用
	bool parseToolCall(const std::string& response, std::string& toolName, std::string& toolArgs, std::string& toolResult);

	std::shared_ptr<AIStrategy> strategy;  // AI策略
	std::vector<std::pair<std::string, long long>> messages;  // 消息列表
};
