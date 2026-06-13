#pragma once
#include "db/DbConnectionPool.h"

#include <string>

namespace http
{

// MySQL工具类，提供便捷的数据库访问接口
class MysqlUtil
{
public:
    // 初始化数据库连接池
    static void init(const std::string& host, const std::string& user,
                    const std::string& password, const std::string& database,
                    size_t poolSize = 10)
    {
        http::db::DbConnectionPool::getInstance().init(
            host, user, password, database, poolSize);
    }

    // 执行查询SQL语句，返回结果集
    template<typename... Args>
    sql::ResultSet* executeQuery(const std::string& sql, Args&&... args)
    {
        auto conn = http::db::DbConnectionPool::getInstance().getConnection();
        return conn->executeQuery(sql, std::forward<Args>(args)...);
    }

    // 执行更新SQL语句，返回受影响的行数
    template<typename... Args>
    int executeUpdate(const std::string& sql, Args&&... args)
    {
        auto conn = http::db::DbConnectionPool::getInstance().getConnection();
        return conn->executeUpdate(sql, std::forward<Args>(args)...);
    }
};

} // namespace http
