#pragma once
#include <memory>
#include "MQManager.h"
#include "AIHelper.h"

// 消息队列消费者服务类
class MQConsumerService {
public:
    // 构造函数
    MQConsumerService();
    // 析构函数
    ~MQConsumerService();
    
    // 启动服务
    void start();
    // 停止服务
    void shutdown();
    
private:
    // 处理消息
    void handleMessage(const std::string& message);
    
    std::unique_ptr<RabbitMQThreadPool> consumer_;  // RabbitMQ消费者
    AIHelper aiHelper_;                        // AI助手
};
