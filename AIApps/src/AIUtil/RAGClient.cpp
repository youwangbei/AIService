#include "../include/AIUtil/RAGClient.h"
#include "rag.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>

class RAGClient::Impl {
public:
    // 构造函数：创建 gRPC 连接
    Impl(const std::string& address) 
        : stub_(rag::RAGService::NewStub(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()))) {}
    
    // 查询知识库
    json Query(const std::string& query, int top_k) {
        grpc::ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(10));
        
        rag::QueryRequest request;
        request.set_query(query);
        request.set_top_k(top_k);
        
        rag::QueryResponse response;
        grpc::Status status = stub_->Query(&context, request, &response);
        
        if (!status.ok()) {
            return json{{"status", "error"}, {"message", status.error_message()}};
        }
        
        json result;
        result["status"] = response.status();
        
        std::vector<std::string> documents;
        for (const auto& doc : response.documents()) {
            documents.push_back(doc);
        }
        result["documents"] = documents;
        
        std::vector<std::string> sources;
        for (const auto& src : response.sources()) {
            sources.push_back(src);
        }
        result["sources"] = sources;
        
        std::vector<double> scores;
        for (const auto& score : response.scores()) {
            scores.push_back(score);
        }
        result["scores"] = scores;
        
        std::string msg = response.message();
        if (!msg.empty()) {
            result["message"] = msg;
        }
        
        return result;
    }
    
    // 添加文档到知识库
    json AddDocument(const std::string& content, const std::string& doc_id) {
        grpc::ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(10));
        
        rag::AddDocumentRequest request;
        request.set_content(content);
        request.set_doc_id(doc_id);
        
        rag::CommonResponse response;
        grpc::Status status = stub_->AddDocument(&context, request, &response);
        
        if (!status.ok()) {
            return json{{"status", "error"}, {"message", status.error_message()}};
        }
        
        return json{
            {"status", response.status()},
            {"message", response.message()}
        };
    }
    
    // 获取知识库中文档数量
    json Count() {
        grpc::ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(10));
        
        rag::CountRequest request;
        rag::CountResponse response;
        grpc::Status status = stub_->Count(&context, request, &response);
        
        if (!status.ok()) {
            return json{{"status", "error"}, {"message", status.error_message()}};
        }
        
        return json{
            {"status", response.status()},
            {"count", response.count()},
            {"message", response.message()}
        };
    }
    
    // 清空知识库
    json Clear() {
        grpc::ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(10));
        
        rag::ClearRequest request;
        rag::CommonResponse response;
        grpc::Status status = stub_->Clear(&context, request, &response);
        
        if (!status.ok()) {
            return json{{"status", "error"}, {"message", status.error_message()}};
        }
        
        return json{
            {"status", response.status()},
            {"message", response.message()}
        };
    }
    
private:
    std::unique_ptr<rag::RAGService::Stub> stub_;
};

// 构造函数
RAGClient::RAGClient(const std::string& address) : impl_(std::make_unique<Impl>(address)) {}
// 析构函数
RAGClient::~RAGClient() = default;

// 查询知识库
json RAGClient::Query(const std::string& query, int top_k) {
    return impl_->Query(query, top_k);
}

// 添加文档到知识库
json RAGClient::AddDocument(const std::string& content, const std::string& doc_id) {
    return impl_->AddDocument(content, doc_id);
}

// 获取知识库中文档数量
json RAGClient::Count() {
    return impl_->Count();
}

// 清空知识库
json RAGClient::Clear() {
    return impl_->Clear();
}