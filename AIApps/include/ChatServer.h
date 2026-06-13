#pragma once

#include <atomic>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>


#include "../../HttpServer/include/http/HttpServer.h"
#include "../../HttpServer/include/utils/MysqlUtil.h"
#include "../../HttpServer/include/utils/FileUtil.h"
#include "../../HttpServer/include/utils/JsonUtil.h"
#include"AIUtil/AISpeechProcessor.h"
#include"AIUtil/AIHelper.h"
#include"AIUtil/ImageRecognizer.h"
#include"AIUtil/base64.h"
#include"AIUtil/MQManager.h"
#include"AIUtil/MQConsumerService.h"


class ChatLoginHandler;
class ChatRegisterHandler;
class ChatLogoutHandler;
class ChatHandler;
class ChatEntryHandler;
class ChatSendHandler;
class ChatHistoryHandler;

class AIMenuHandler;
class AIUploadHandler;
class AIUploadSendHandler;


class ChatCreateAndSendHandler;
class ChatSessionsHandler;
class ChatSpeechHandler;
class AICVUploadHandler;
class AICVUploadSendHandler;
class TaskResultHandler;

// 聊天服务器类，提供AI聊天相关功能
class ChatServer {
public:
	// 构造函数
	// port: 监听端口
	// name: 服务器名称
	// option: TCP服务器选项
	ChatServer(int port,
		const std::string& name,
		muduo::net::TcpServer::Option option = muduo::net::TcpServer::kNoReusePort);

	// 设置工作线程数
	void setThreadNum(int numThreads);
	// 启动服务器
	void start();
	// 从MySQL加载历史记录初始化聊天消息
	void initChatMessage();
private:
	friend class ChatLoginHandler;
	friend class ChatRegisterHandler;
	friend  ChatLogoutHandler;
	friend class ChatHandler;
	friend class ChatEntryHandler;
	friend class ChatSendHandler;
	friend class AIMenuHandler;
	friend class AIUploadHandler;
	friend class AIUploadSendHandler;
	friend class ChatHistoryHandler;

	friend class ChatCreateAndSendHandler;
	friend class ChatSessionsHandler;
	friend class ChatSpeechHandler;
	friend class AICVUploadHandler;
	friend class AICVUploadSendHandler;
	friend class TaskResultHandler;

private:
	// 初始化服务器
	void initialize();
	// 初始化会话管理
	void initializeSession();
	// 初始化路由
	void initializeRouter();
	// 初始化中间件
	void initializeMiddleware();
	

	// 从MySQL读取数据
	void readDataFromMySQL();

	// 包装HTTP响应
	void packageResp(const std::string& version, http::HttpResponse::HttpStatusCode statusCode,
		const std::string& statusMsg, bool close, const std::string& contentType,
		int contentLen, const std::string& body, http::HttpResponse* resp);

	// 设置会话管理器
	void setSessionManager(std::unique_ptr<http::session::SessionManager> manager)
	{
		httpServer_.setSessionManager(std::move(manager));
	}
	// 获取会话管理器
	http::session::SessionManager* getSessionManager() const
	{
		return httpServer_.getSessionManager();
	}

	http::HttpServer	httpServer_;  // HTTP服务器实例

	http::MysqlUtil		mysqlUtil_;  // MySQL工具类

	std::unordered_map<int, bool>	onlineUsers_;  // 在线用户映射
	std::mutex	mutexForOnlineUsers_;  // 在线用户锁

	

	// std::unordered_map<int, std::shared_ptr<AIHelper>> chatInformation;

	std::unordered_map<int, std::unordered_map<std::string,std::shared_ptr<AIHelper>>> chatInformation;  // 用户聊天信息映射
	std::mutex	mutexForChatInformation;  // 聊天信息锁

	std::unordered_map<int, std::shared_ptr<ImageRecognizer>> ImageRecognizerMap;  // 图像识别器映射
	std::mutex	mutexForImageRecognizerMap;  // 图像识别器锁

	std::unordered_map<int,std::vector<std::string>> sessionsIdsMap;  // 用户会话ID映射
	std::mutex mutexForSessionsId;  // 会话ID锁

	std::unique_ptr<MQConsumerService> mqConsumer_;  // 消息队列消费者

};

