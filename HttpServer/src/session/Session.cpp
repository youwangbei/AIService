#include "../include/session/Session.h"

#include "../include/session/SessionManager.h"

namespace http
{
namespace session
{

// 构造会话对象，初始化会话ID、最大过期时间和会话管理器
Session::Session(const std::string& sessionId, SessionManager* sessionManager, int maxAge)
    : sessionId_(sessionId)
    , maxAge_(maxAge)
    , sessionManager_(sessionManager)
{
    refresh();
}

// 检查会话是否已过期
bool Session::isExpired() const
{
    return std::chrono::system_clock::now() > expiryTime_;
}

// 刷新会话过期时间
void Session::refresh()
{
    expiryTime_ = std::chrono::system_clock::now() + std::chrono::seconds(maxAge_);
}

// 设置会话键值对，并通知会话管理器更新
void Session::setValue(const std::string& key, const std::string& value)
{
    data_[key] = value;
    if (sessionManager_)
    {
        sessionManager_->updateSession(shared_from_this());
    }
}

// 获取会话中指定键的值
std::string Session::getValue(const std::string& key) const
{
    auto it = data_.find(key);
    return it != data_.end() ? it->second : std::string();
}

// 移除会话中指定键的值
void Session::remove(const std::string& key)
{
    data_.erase(key);
}

// 清空会话中所有数据
void Session::clear()
{
    data_.clear();
}

} // namespace session
} // namespace http
