#pragma once
#include "Session.h"
#include <memory>

namespace http
{
namespace session
{

// 会话存储基类，定义会话存储的接口
class SessionStorage
{
public:
    // 虚析构函数
    virtual ~SessionStorage() = default;
    // 保存会话
    virtual void save(std::shared_ptr<Session> session) = 0;
    // 加载会话
    virtual std::shared_ptr<Session> load(const std::string& sessionId) = 0;
    // 删除会话
    virtual void remove(const std::string& sessionId) = 0;
};

// 内存会话存储实现
class MemorySessionStorage : public SessionStorage
{
public:
    // 保存会话到内存
    void save(std::shared_ptr<Session> session) override;
    // 从内存加载会话
    std::shared_ptr<Session> load(const std::string& sessionId) override;
    // 从内存删除会话
    void remove(const std::string& sessionId) override;
private:
    std::unordered_map<std::string, std::shared_ptr<Session>> sessions_;  // 内存中的会话映射
};

} // namespace session
} // namespace http
