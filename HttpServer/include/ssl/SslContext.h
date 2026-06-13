#pragma once
#include "SslConfig.h"
#include <openssl/ssl.h>
#include <memory>
#include <muduo/base/noncopyable.h>

namespace ssl 
{

// SSL上下文类，封装OpenSSL的SSL_CTX
class SslContext : muduo::noncopyable 
{
public:
    // 构造函数，接受SSL配置
    explicit SslContext(const SslConfig& config);
    // 析构函数
    ~SslContext();

    // 初始化SSL上下文
    bool initialize();
    // 获取原生OpenSSL句柄
    SSL_CTX* getNativeHandle() { return ctx_; }

private:
    // 加载证书
    bool loadCertificates();
    // 设置协议
    bool setupProtocol();
    // 设置会话缓存
    void setupSessionCache();
    // 处理SSL错误
    static void handleSslError(const char* msg);

private:
    SSL_CTX*  ctx_;    // OpenSSL的SSL_CTX指针
    SslConfig config_;  // SSL配置
};

} // namespace ssl
