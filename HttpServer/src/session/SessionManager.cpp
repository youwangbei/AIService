#include"../include/session/SessionManager.h"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace http
{
namespace session
{

// 构造会话管理器，初始化会话存储和随机数生成器
SessionManager::SessionManager(std::unique_ptr<SessionStorage> storage)
    : storage_(std::move(storage)) 
    , rng_(std::random_device{}())
{}

// 获取或创建会话，从请求Cookie中读取会话ID，若不存在或过期则创建新会话
std::shared_ptr<Session> SessionManager::getSession(const HttpRequest& req, HttpResponse* resp)
{   
    std::string sessionId = getSessionIdFromCookie(req);
    
    std::shared_ptr<Session> session;

    if (!sessionId.empty())
    {
        session = storage_->load(sessionId);
    }

    if (!session || session->isExpired())
    {
        sessionId = generateSessionId();
        session = std::make_shared<Session>(sessionId, this);
        setSessionCookie(sessionId, resp);
    }
    else 
    {
        session->setManager(this);
    }

    session->refresh();
    storage_->save(session);
    return session;
}

// 生成随机的会话ID
std::string SessionManager::generateSessionId()
{
    std::stringstream ss;
    std::uniform_int_distribution<> dist(0, 15);

    for (int i = 0; i < 32; ++i)
    {
        ss << std::hex << dist(rng_);
    }
    return ss.str();
}

// 销毁指定的会话
void SessionManager::destroySession(const std::string& sessionId)
{
    storage_->remove(sessionId);
}

// 清理过期的会话（当前为空实现）
void SessionManager::cleanExpiredSessions()
{
}

// 从HTTP请求的Cookie中提取会话ID
std::string SessionManager::getSessionIdFromCookie(const HttpRequest& req)
{
    std::string sessionId;
    std::string cookie = req.getHeader("Cookie");

    if (!cookie.empty())
    {
        size_t pos = cookie.find("sessionId=");
        if (pos != std::string::npos)
        {
            pos += 10;
            size_t end = cookie.find(';', pos);
            if (end != std::string::npos)
            {
                sessionId = cookie.substr(pos, end - pos);
            }
            else
            {
                sessionId = cookie.substr(pos);
            }
        }
    }
    
    return sessionId;
}

// 在HTTP响应中设置会话Cookie
void SessionManager::setSessionCookie(const std::string& sessionId, HttpResponse* resp)
{
    std::string cookie = "sessionId=" + sessionId + "; Path=/; HttpOnly";
    resp->addHeader("Set-Cookie", cookie);
}

} // namespace session
} // namespace http
