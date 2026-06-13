#pragma once
#include "SslContext.h"
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/noncopyable.h>
#include <openssl/ssl.h>
#include <memory>

namespace ssl 
{

// 消息回调类型定义
using MessageCallback = std::function<void(const std::shared_ptr<muduo::net::TcpConnection>&,
                                         muduo::net::Buffer*,
                                         muduo::Timestamp)>;

// SSL连接类，封装SSL加密的TCP连接
class SslConnection : muduo::noncopyable 
{
public:
    using TcpConnectionPtr = std::shared_ptr<muduo::net::TcpConnection>;
    using BufferPtr = muduo::net::Buffer*;
    
    // 构造函数，接受TCP连接和SSL上下文
    SslConnection(const TcpConnectionPtr& conn, SslContext* ctx);
    // 析构函数
    ~SslConnection();

    // 开始SSL握手
    void startHandshake();
    // 发送加密数据
    void send(const void* data, size_t len);
    // 处理读事件回调
    void onRead(const TcpConnectionPtr& conn, BufferPtr buf, muduo::Timestamp time);
    // 检查握手是否完成
    bool isHandshakeCompleted() const { return state_ == SSLState::ESTABLISHED; }
    // 获取解密后的缓冲区
    muduo::net::Buffer* getDecryptedBuffer() { return &decryptedBuffer_; }
    // BIO写回调
    static int bioWrite(BIO* bio, const char* data, int len);
    // BIO读回调
    static int bioRead(BIO* bio, char* data, int len);
    // BIO控制回调
    static long bioCtrl(BIO* bio, int cmd, long num, void* ptr);
    // 设置消息回调
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
private:
    // 处理SSL握手
    void handleHandshake();
    // 处理加密数据
    void onEncrypted(const char* data, size_t len);
    // 处理解密数据
    void onDecrypted(const char* data, size_t len);
    // 获取最后的错误
    SSLError getLastError(int ret);
    // 处理错误
    void handleError(SSLError error);

private:
    SSL*                ssl_;            // OpenSSL的SSL指针
    SslContext*         ctx_;            // SSL上下文
    TcpConnectionPtr    conn_;           // TCP连接
    SSLState            state_;            // SSL连接状态
    BIO*                readBio_;         // 读BIO
    BIO*                writeBio_;        // 写BIO
    muduo::net::Buffer  readBuffer_;     // 读缓冲区
    muduo::net::Buffer  writeBuffer_;    // 写缓冲区
    muduo::net::Buffer  decryptedBuffer_; // 解密后数据缓冲区
    MessageCallback     messageCallback_; // 消息回调
};

} // namespace ssl
