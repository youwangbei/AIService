#include "../../../include/utils/db/DbConnection.h"
#include "../../../include/utils/db/DbException.h"
#include <muduo/base/Logging.h>

namespace http 
{
namespace db 
{

// 构造数据库连接对象，建立到MySQL服务器的连接
DbConnection::DbConnection(const std::string& host,
                         const std::string& user,
                         const std::string& password,
                         const std::string& database)
    : host_(host)
    , user_(user)
    , password_(password)
    , database_(database)
{
    try 
    {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        
        std::string connectHost = host_;
        size_t tcpPrefix = connectHost.find("tcp://");
        if (tcpPrefix == 0) {
            connectHost = connectHost.substr(6);
        }
        
        conn_.reset(driver->connect(connectHost, user_, password_));
        if (conn_) 
        {
            conn_->setSchema(database_);
            
            conn_->setClientOption("OPT_RECONNECT", "true");
            conn_->setClientOption("OPT_CONNECT_TIMEOUT", "10");
            conn_->setClientOption("multi_statements", "false");
            
            std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
            stmt->execute("SET NAMES utf8mb4");
            
            LOG_INFO << "Database connection established";
        }
    } 
    catch (const sql::SQLException& e) 
    {
        LOG_ERROR << "Failed to create database connection: " << e.what();
        throw DbException(e.what());
    }
}

// 析构数据库连接对象，关闭连接
DbConnection::~DbConnection() 
{
    try 
    {
        cleanup();
    } 
    catch (...) 
    {
    }
    LOG_INFO << "Database connection closed";
}

// 测试数据库连接是否可用
bool DbConnection::ping() 
{
    std::lock_guard<std::mutex> lock(mutex_);
    try 
    {
        if (!conn_) return false;
        std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery("SELECT 1"));
        if (rs && rs->next()) {
            return true;
        }
        return false;
    } 
    catch (const sql::SQLException& e) 
    {
        LOG_ERROR << "Ping failed: " << e.what();
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR << "Ping exception: " << e.what();
        return false;
    }
}

// 检查数据库连接是否有效
bool DbConnection::isValid() 
{
    try 
    {
        if (!conn_) return false;
        std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
        stmt->execute("SELECT 1");
        return true;
    } 
    catch (const sql::SQLException&) 
    {
        return false;
    }
}

// 重新连接到数据库
void DbConnection::reconnect() 
{
    try 
    {
        if (conn_) 
        {
            conn_->reconnect();
        } 
        else 
        {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            conn_.reset(driver->connect(host_, user_, password_));
            conn_->setSchema(database_);
        }
    } 
    catch (const sql::SQLException& e) 
    {
        LOG_ERROR << "Reconnect failed: " << e.what();
        throw DbException(e.what());
    }
}

// 清理数据库连接资源
void DbConnection::cleanup() 
{
    std::lock_guard<std::mutex> lock(mutex_);
    try 
    {
        if (conn_) 
        {
            if (!conn_->getAutoCommit()) 
            {
                conn_->rollback();
                conn_->setAutoCommit(true);
            }
            
            std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
            while (stmt->getMoreResults()) 
            {
                auto result = stmt->getResultSet();
                while (result && result->next()) 
                {
                }
            }
        }
    } 
    catch (const std::exception& e) 
    {
        LOG_WARN << "Error cleaning up connection: " << e.what();
        try 
        {
            reconnect();
        } 
        catch (...) 
        {
        }
    }
}

} // namespace db
} // namespace http
