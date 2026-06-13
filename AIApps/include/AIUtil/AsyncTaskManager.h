#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>

// 任务结果结构体
struct TaskResult {
    std::string result;   // 任务结果
    bool completed;       // 是否完成
    std::string error;    // 错误信息
    
    TaskResult() : completed(false) {}
};

// 异步任务管理器类，单例模式，用于管理异步AI任务
class AsyncTaskManager {
public:
    // 获取单例实例
    static AsyncTaskManager& instance();
    
    // 创建任务
    std::string createTask(int userId, const std::string& username, 
                          const std::string& sessionId, const std::string& question,
                          const std::string& model = "", bool useRag = false);
    
    // 获取任务结果
    TaskResult getTaskResult(const std::string& taskId);
    
    // 设置任务结果
    void setTaskResult(const std::string& taskId, const std::string& result, bool success = true, 
                       const std::string& error = "");

private:
    // 私有构造函数
    AsyncTaskManager() = default;
    // 禁止复制
    AsyncTaskManager(const AsyncTaskManager&) = delete;
    AsyncTaskManager& operator=(const AsyncTaskManager&) = delete;
    
    std::unordered_map<std::string, TaskResult> tasks_;  // 任务映射
    std::mutex mutex_;                              // 互斥锁
    std::string generateTaskId();                   // 生成任务ID
};
