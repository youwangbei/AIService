#pragma once
#include "SslTypes.h"
#include <string>
#include <vector>

namespace ssl 
{

// SSL配置类
class SslConfig 
{
public:
    // 构造函数
    SslConfig();
    // 析构函数
    ~SslConfig() = default;

    // 设置证书文件路径
    void setCertificateFile(const std::string& certFile) { certFile_ = certFile; }
    // 设置私钥文件路径
    void setPrivateKeyFile(const std::string& keyFile) { keyFile_ = keyFile; }
    // 设置证书链文件路径
    void setCertificateChainFile(const std::string& chainFile) { chainFile_ = chainFile; }
    
    // 设置协议版本
    void setProtocolVersion(SSLVersion version) { version_ = version; }
    // 设置加密套件列表
    void setCipherList(const std::string& cipherList) { cipherList_ = cipherList; }
    
    // 设置是否验证客户端
    void setVerifyClient(bool verify) { verifyClient_ = verify; }
    // 设置验证深度
    void setVerifyDepth(int depth) { verifyDepth_ = depth; }
    
    // 设置会话超时时间（秒）
    void setSessionTimeout(int seconds) { sessionTimeout_ = seconds; }
    // 设置会话缓存大小
    void setSessionCacheSize(long size) { sessionCacheSize_ = size; }

    // 获取证书文件路径
    const std::string& getCertificateFile() const { return certFile_; }
    // 获取私钥文件路径
    const std::string& getPrivateKeyFile() const { return keyFile_; }
    // 获取证书链文件路径
    const std::string& getCertificateChainFile() const { return chainFile_; }
    // 获取协议版本
    SSLVersion getProtocolVersion() const { return version_; }
    // 获取加密套件列表
    const std::string& getCipherList() const { return cipherList_; }
    // 获取是否验证客户端
    bool getVerifyClient() const { return verifyClient_; }
    // 获取验证深度
    int getVerifyDepth() const { return verifyDepth_; }
    // 获取会话超时时间
    int getSessionTimeout() const { return sessionTimeout_; }
    // 获取会话缓存大小
    long getSessionCacheSize() const { return sessionCacheSize_; }

private:
    std::string certFile_;         // 证书文件路径
    std::string keyFile_;          // 私钥文件路径
    std::string chainFile_;        // 证书链文件路径
    SSLVersion  version_;          // SSL协议版本
    std::string cipherList_;       // 加密套件列表
    bool        verifyClient_;     // 是否验证客户端
    int         verifyDepth_;     // 验证深度
    int         sessionTimeout_;   // 会话超时时间
    long        sessionCacheSize_; // 会话缓存大小
};

} // namespace ssl
