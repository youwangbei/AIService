#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include "DbConnection.h"

namespace http 
{
namespace db 
{

// 数据库连接池类，单例模式
class DbConnectionPool 
{
public:
    // 获取单例实例
    static DbConnectionPool& getInstance() 
    {
        static DbConnectionPool instance;
        return instance;
    }

    // 初始化连接池
    void init(const std::string& host,
             const std::string& user,
             const std::string& password,
             const std::string& database,
             size_t poolSize = 10);

    // 获取一个数据库连接
    std::shared_ptr<DbConnection> getConnection();

private:
    // 私有构造函数
    DbConnectionPool();
    // 私有析构函数
    ~DbConnectionPool();

    // 禁止拷贝
    DbConnectionPool(const DbConnectionPool&) = delete;
    // 禁止赋值
    DbConnectionPool& operator=(const DbConnectionPool&) = delete;

    // 创建新的数据库连接
    std::shared_ptr<DbConnection> createConnection();

    // 检查连接有效性
    void checkConnections();

private:
    std::string                               host_;         // 主机地址
    std::string                               user_;         // 用户名
    std::string                               password_;     // 密码
    std::string                               database_;     // 数据库名
    std::queue<std::shared_ptr<DbConnection>> connections_;  // 连接队列
    std::mutex                                mutex_;        // 互斥锁
    std::condition_variable                   cv_;           // 条件变量
    bool                                      initialized_ = false;  // 初始化标志
    std::thread                               checkThread_;  // 检查线程
};

} // namespace db
} // namespace http
