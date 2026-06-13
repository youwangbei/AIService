#pragma once

#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>
#include <functional>

// 消息队列管理器类，单例模式，用于管理RabbitMQ连接
class MQManager {
public:
    // 获取单例实例
    static MQManager& instance() {
        static MQManager mgr;
        return mgr;
    }

    // 发布消息
    void publish(const std::string& queue, const std::string& msg);
    // 检查连接状态
    bool isConnected() const { return isConnected_; }

private:
    // 消息队列连接结构
    struct MQConn {
        AmqpClient::Channel::ptr_t channel;  // AMQP通道
        std::mutex mtx;                      // 互斥锁
    };

    // 私有构造函数
    MQManager(size_t poolSize = 5);

    // 禁止复制
    MQManager(const MQManager&) = delete;
    MQManager& operator=(const MQManager&) = delete;

    std::vector<std::shared_ptr<MQConn>> pool_;  // 连接池
    size_t poolSize_;                           // 连接池大小
    std::atomic<size_t> counter_;               // 轮询计数器
    bool isConnected_;                          // 连接状态
};

// RabbitMQ线程池消费者类
class RabbitMQThreadPool {
public:
    // 消息处理函数类型定义
    using HandlerFunc = std::function<void(const std::string&)>;

    // 构造函数
    RabbitMQThreadPool(const std::string& host,
        const std::string& queue,
        int thread_num,
        HandlerFunc handler)
        : stop_(false),
        rabbitmq_host_(host),
        queue_name_(queue),
        thread_num_(thread_num),
        handler_(handler) {}

    // 启动消费者
    void start();
    // 停止消费者
    void shutdown();

    // 析构函数
    ~RabbitMQThreadPool() {
        shutdown();
    }

private:
    // 工作线程函数
    void worker(int id);

private:
    std::vector<std::thread> workers_;   // 工作线程池
    std::atomic<bool> stop_;             // 停止标志
    std::string queue_name_;             // 队列名称
    int thread_num_;                     // 线程数量
    std::string rabbitmq_host_;          // RabbitMQ主机地址
    HandlerFunc handler_;                // 消息处理函数
};
