#include "../include/AIUtil/AsyncTaskManager.h"
#include <chrono>
#include <sstream>
#include <iomanip>

// 获取单例实例
AsyncTaskManager& AsyncTaskManager::instance() {
    static AsyncTaskManager instance;
    return instance;
}

// 生成唯一任务 ID
std::string AsyncTaskManager::generateTaskId() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    std::stringstream ss;
    ss << std::hex << ms.count();
    return "task_" + ss.str();
}

// 创建新任务
std::string AsyncTaskManager::createTask(int userId, const std::string& username, 
                                         const std::string& sessionId, const std::string& question,
                                         const std::string& model, bool useRag) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string taskId = generateTaskId();
    tasks_[taskId] = TaskResult();
    return taskId;
}

// 获取任务结果
TaskResult AsyncTaskManager::getTaskResult(const std::string& taskId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        return it->second;
    }
    TaskResult empty;
    empty.error = "Task not found";
    return empty;
}

// 设置任务结果
void AsyncTaskManager::setTaskResult(const std::string& taskId, const std::string& result, 
                                     bool success, const std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        it->second.completed = true;
        if (success) {
            it->second.result = result;
        } else {
            it->second.error = error;
        }
    }
}
