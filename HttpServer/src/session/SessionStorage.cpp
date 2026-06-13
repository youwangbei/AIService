#include "../include/session/SessionStorage.h"
#include <iostream>

namespace http
{

namespace session
{

// 将会话保存到内存存储中
void MemorySessionStorage::save(std::shared_ptr<Session> session)
{
    sessions_[session->getId()] = session;
}

// 从内存存储中加载会话，若会话过期则移除并返回空
std::shared_ptr<Session> MemorySessionStorage::load(const std::string& sessionId)
{
    auto it = sessions_.find(sessionId);
    if (it != sessions_.end())
    {
        if (!it->second->isExpired())
        {
            return it->second;
        }
        else
        {
            sessions_.erase(it);
        }
    }

    return nullptr;
}

// 从内存存储中移除指定会话
void MemorySessionStorage::remove(const std::string& sessionId)
{
    sessions_.erase(sessionId);
}

} // namespace session
} // namespace http
