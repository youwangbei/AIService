#pragma once
#include <memory>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// RAG客户端类，用于与RAG服务交互
class RAGClient {
public:
    // 构造函数
    RAGClient(const std::string& address = "localhost:50051");
    // 析构函数
    ~RAGClient();
    
    // 查询RAG
    json Query(const std::string& query, int top_k = 3);
    // 添加文档
    json AddDocument(const std::string& content, const std::string& doc_id);
    // 获取文档数量
    json Count();
    // 清空文档
    json Clear();
    
private:
    // 前向声明实现类
    class Impl;
    std::unique_ptr<Impl> impl_;  // 实现类指针
};
