#pragma once
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <string>


#include "AIStrategy.h"

// 策略工厂类，单例模式，用于创建不同的AI策略
class StrategyFactory {

public:
    // 创建函数类型定义
    using Creator = std::function<std::shared_ptr<AIStrategy>()>;

    // 获取单例实例
    static StrategyFactory& instance();

    // 注册策略
    void registerStrategy(const std::string& name, Creator creator);

    // 创建策略
    std::shared_ptr<AIStrategy> create(const std::string& name);

private:
    // 私有构造函数
    StrategyFactory() = default;
    std::unordered_map<std::string, Creator> creators;  // 策略创建者映射
};



// 策略注册辅助模板类
template<typename T>
struct StrategyRegister {
    // 构造函数，自动注册策略
    StrategyRegister(const std::string& name) {
        StrategyFactory::instance().registerStrategy(name, [] {
            std::shared_ptr<AIStrategy> instance = std::make_shared<T>();
            return instance;
            });
    }
};
