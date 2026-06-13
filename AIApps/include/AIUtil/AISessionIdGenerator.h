#pragma once
#include <chrono>
#include <random>
#include <cstdlib>
#include <ctime>
#include <string>


// AI会话ID生成器类，用于生成唯一的会话ID
class AISessionIdGenerator {
public:
    // 构造函数，初始化随机数种子
    AISessionIdGenerator() {
        
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }
    
    // 生成会话ID
    std::string generate();
};
