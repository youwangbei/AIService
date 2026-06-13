#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <chrono>

namespace http
{

namespace session
{

class SessionManager;

// 会话类
class Session : public std::enable_shared_from_this<Session>
{
public:
    // 构造函数
    // sessionId: 会话ID
    // sessionManager: 会话管理器指针
    // maxAge: 最大存活时间，默认3600秒（1小时）
    Session(const std::string& sessionId, SessionManager* sessionManager, int maxAge = 3600);
    
    // 获取会话ID
    const std::string& getId() const 
    { return sessionId_; }

    // 检查会话是否过期
    bool isExpired() const;
    // 刷新会话过期时间
    void refresh();

    // 设置会话管理器
    void setManager(SessionManager* sessionManager) 
    { sessionManager_ = sessionManager; }

    // 获取会话管理器
    SessionManager* getManager() const 
    { return sessionManager_; }

    // 数据存取
    void setValue(const std::string&key, const std::string&value); // 设置会话数据
    std::string getValue(const std::string&key) const;             // 获取会话数据
    void remove(const std::string&key);                            // 删除会话数据
    void clear();                                                  // 清空所有会话数据
private:
    std::string                                  sessionId_;         // 会话ID
    std::unordered_map<std::string, std::string> data_;              // 会话数据
    std::chrono::system_clock::time_point        expiryTime_;        // 过期时间
    int                                          maxAge_;            // 最大存活时间（秒）
    SessionManager*                              sessionManager_;    // 会话管理器指针
};

} // namespace session
} // namespace http