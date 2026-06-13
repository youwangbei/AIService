#pragma once
#include <memory>
#include <string>
#include <mutex>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <mysql_driver.h>
#include <mysql/mysql.h>
#include <muduo/base/Logging.h>
#include "DbException.h"

namespace http 
{
namespace db 
{

// 数据库连接类，封装单个数据库连接
class DbConnection 
{
public:
    // 构造函数，创建数据库连接
    DbConnection(const std::string& host, 
                const std::string& user,
                const std::string& password,
                const std::string& database);
    // 析构函数
    ~DbConnection();

    // 禁止拷贝构造
    DbConnection(const DbConnection&) = delete;
    // 禁止赋值操作
    DbConnection& operator=(const DbConnection&) = delete;

    // 检查连接是否有效
    bool isValid();
    // 重新连接数据库
    void reconnect();
    // 清理资源
    void cleanup();

    // 执行查询SQL语句，支持可变参数
    template<typename... Args>
    sql::ResultSet* executeQuery(const std::string& sql, Args&&... args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        try 
        {
            std::unique_ptr<sql::PreparedStatement> stmt(
                conn_->prepareStatement(sql)
            );
            bindParams(stmt.get(), 1, std::forward<Args>(args)...);
            return stmt->executeQuery();
        } 
        catch (const sql::SQLException& e) 
        {
            LOG_ERROR << "Query failed: " << e.what() << ", SQL: " << sql;
            throw DbException(e.what());
        }
    }
    
    // 执行更新SQL语句，支持可变参数
    template<typename... Args>
    int executeUpdate(const std::string& sql, Args&&... args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        try 
        {
            std::unique_ptr<sql::PreparedStatement> stmt(
                conn_->prepareStatement(sql)
            );
            bindParams(stmt.get(), 1, std::forward<Args>(args)...);
            return stmt->executeUpdate();
        } 
        catch (const sql::SQLException& e) 
        {
            LOG_ERROR << "Update failed: " << e.what() << ", SQL: " << sql;
            throw DbException(e.what());
        }
    }

    // 测试连接是否有效
    bool ping();
private:
    // 绑定参数的基础版本（无参数）
    void bindParams(sql::PreparedStatement*, int) {}
    
    // 绑定参数的通用模板版本
    template<typename T, typename... Args>
    void bindParams(sql::PreparedStatement* stmt, int index, 
                   T&& value, Args&&... args) 
    {
        stmt->setString(index, std::to_string(std::forward<T>(value)));
        bindParams(stmt, index + 1, std::forward<Args>(args)...);
    }
    
    // 绑定参数的字符串特化版本
    template<typename... Args>
    void bindParams(sql::PreparedStatement* stmt, int index, 
                   const std::string& value, Args&&... args) 
    {
        stmt->setString(index, value);
        bindParams(stmt, index + 1, std::forward<Args>(args)...);
    }

private:
    std::shared_ptr<sql::Connection> conn_;       // 数据库连接指针
    std::string                      host_;       // 主机地址
    std::string                      user_;       // 用户名
    std::string                      password_;   // 密码
    std::string                      database_;   // 数据库名
    std::mutex                       mutex_;      // 互斥锁，保证线程安全
};

} // namespace db
} // namespace http
