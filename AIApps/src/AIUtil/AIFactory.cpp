#include "../include/AIUtil/AIFactory.h"


// 获取单例工厂实例
StrategyFactory& StrategyFactory::instance() {
    static StrategyFactory factory;
    return factory;
}

// 注册策略创建器
void StrategyFactory::registerStrategy(const std::string& name, Creator creator) {
    creators[name] = std::move(creator);
}

// 根据名称创建策略实例
std::shared_ptr<AIStrategy> StrategyFactory::create(const std::string& name) {
    auto it = creators.find(name);
    if (it == creators.end()) {
        throw std::runtime_error("Unknown strategy: " + name);
    }
    return it->second();
}
