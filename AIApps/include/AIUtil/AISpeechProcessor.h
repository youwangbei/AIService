#pragma once
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <memory>
#include <sstream>
#include <thread>
#include <chrono>


#include "../../../HttpServer/include/utils/JsonUtil.h"
#include "base64.h"



// AI语音处理器类，使用百度API进行语音识别和合成
class AISpeechProcessor {
public:
    // 构造函数
    AISpeechProcessor(const std::string& clientId,
                      const std::string& clientSecret,
                      const std::string& cuid = "RZjSQGzNaA8EFWf6rvuHEKDh9i4XJIV9") // 用户唯一标识，需要更改成自身标识
        : client_id_(clientId), client_secret_(clientSecret), cuid_(cuid) 
    {
        token_ = getAccessToken();
    }

    // 语音识别
    std::string recognize(const std::string& speechData,const std::string& format = "pcm",int rate = 16000,int channel = 1);

    // 语音合成
    std::string synthesize(const std::string& text,const std::string& format = "mp3-16k",const std::string& lang = "zh",int speed = 5,int pitch = 5,int volume = 5); 


private:
    std::string client_id_;        // 百度应用ID
    std::string client_secret_;    // 百度应用密钥
    std::string cuid_;             // 用户唯一标识
    std::string token_;            // 访问令牌

    // 获取Access Token
    std::string getAccessToken();

};
