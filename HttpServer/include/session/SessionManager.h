#pragma once

#include "SessionStorage.h"
#include "../http/HttpRequest.h"
#include "../http/HttpResponse.h"
#include <memory>
#include <random>

namespace http
{
namespace session
{

// 会话管理器类
class SessionManager
{
public:
    // 构造函数，传入会话存储对象
    explicit SessionManager(std::unique_ptr<SessionStorage> storage);

    // 从请求中获取或创建会话
    std::shared_ptr<Session> getSession(const HttpRequest& req, HttpResponse* resp);
    
    // 销毁会话
    void destroySession(const std::string& sessionId);

    // 清理过期会话
    void cleanExpiredSessions();

    // 更新会话
    void updateSession(std::shared_ptr<Session> session)
    {
        storage_->save(session);
    }
private:
    std::string generateSessionId();                        // 生成随机会话ID
    std::string getSessionIdFromCookie(const HttpRequest& req); // 从Cookie中获取会话ID
    void setSessionCookie(const std::string& sessionId, HttpResponse* resp); // 设置会话Cookie

private:
    std::unique_ptr<SessionStorage> storage_; // 会话存储对象
    std::mt19937 rng_;                       // 随机数生成器，用于生成随机会话ID
};

} // namespace session
} // namespace http