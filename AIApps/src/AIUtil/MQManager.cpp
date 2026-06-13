#include "../include/AIUtil/MQManager.h"

// 构造函数：初始化 RabbitMQ 连接池
MQManager::MQManager(size_t poolSize) 
    : poolSize_(poolSize), counter_(0), isConnected_(false) {
    try {
        for (size_t i = 0; i < poolSize_; ++i) {
            auto conn = std::make_shared<MQConn>();
            try {
                conn->channel = AmqpClient::Channel::Create("localhost");
                isConnected_ = true;
            } catch (...) {
                conn->channel = nullptr;
            }
            pool_.push_back(conn);
        }
    } catch (...) {
        isConnected_ = false;
    }
}

// 发布消息到指定队列
void MQManager::publish(const std::string& queue, const std::string& msg) {
    if (!isConnected_) {
        return;
    }
    
    size_t idx = counter_++ % poolSize_;
    auto& conn = pool_[idx];
    
    std::lock_guard<std::mutex> lock(conn->mtx);
    if (!conn->channel) {
        try {
            conn->channel = AmqpClient::Channel::Create("localhost");
        } catch (...) {
            return;
        }
    }
    
    try {
        conn->channel->DeclareQueue(queue, false, true, false, false);
        conn->channel->BasicPublish("", queue, AmqpClient::BasicMessage::Create(msg));
    } catch (...) {
        conn->channel = nullptr;
    }
}

// 启动线程池
void RabbitMQThreadPool::start() {
    for (int i = 0; i < thread_num_; ++i) {
        workers_.emplace_back(&RabbitMQThreadPool::worker, this, i);
    }
}

// 关闭线程池
void RabbitMQThreadPool::shutdown() {
    stop_ = true;
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

// 工作线程函数：持续从队列消费消息
void RabbitMQThreadPool::worker(int id) {
    (void)id;
    while (!stop_) {
        try {
            auto channel = AmqpClient::Channel::Create(rabbitmq_host_);
            channel->DeclareQueue(queue_name_, false, true, false, false);
            auto consumer_tag = channel->BasicConsume(queue_name_, "", true, false, false);
            
            while (!stop_) {
                AmqpClient::Envelope::ptr_t envelope;
                if (channel->BasicConsumeMessage(consumer_tag, envelope, 1000)) {
                    try {
                        handler_(envelope->Message()->Body());
                    } catch (...) {
                    }
                }
            }
        } catch (...) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}
