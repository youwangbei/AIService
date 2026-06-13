#pragma once
#include <string>
#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <chrono>
#include <thread>
#include "base64.h"

// 本地TTS处理器类，调用本地TTS服务进行语音合成
class LocalTTSProcessor {
public:
    // 构造函数
    LocalTTSProcessor(const std::string& serverUrl = "http://127.0.0.1:5000",
                     int timeoutSeconds = 10)
        : serverUrl_(serverUrl), timeoutSeconds_(timeoutSeconds) {}

    // 语音合成
    std::string synthesize(const std::string& text,
                          const std::string& engine = "edge",
                          const std::string& lang = "zh-CN") {
        try {
            std::string audioFile = downloadAudio(text, engine, lang);

            if (audioFile.empty()) return "";

            std::string base64Audio = readAndEncodeFile(audioFile);
            remove(audioFile.c_str());

            return base64Audio;
        } catch (const std::exception& e) {
            std::cerr << "TTS错误: " << e.what() << std::endl;
            return "";
        }
    }

private:
    // CURL写回调函数
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        std::string* str = static_cast<std::string*>(userp);
        str->append(static_cast<char*>(contents), size * nmemb);
        return size * nmemb;
    }

    // 下载音频
    std::string downloadAudio(const std::string& text,
                              const std::string& engine,
                              const std::string& lang) {
        CURL* curl = curl_easy_init();
        if (!curl) return "";

        std::string responseData;

        std::string url = serverUrl_ + "/synthesize";

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        std::string jsonData = "{\"text\":\"" + escapeJson(text) + "\",\"engine\":\"" + engine + "\",\"lang\":\"" + lang + "\"}";

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutSeconds_);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);

        CURLcode res = curl_easy_perform(curl);

        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

        curl_easy_cleanup(curl);
        if (headers) curl_slist_free_all(headers);

        if (res != CURLE_OK || httpCode != 200) {
            std::cerr << "TTS下载失败: " << curl_easy_strerror(res) << ", HTTP: " << httpCode << std::endl;
            return "";
        }

        if (responseData.empty() || responseData.find("error") != std::string::npos) {
            std::cerr << "TTS服务错误: " << responseData << std::endl;
            return "";
        }

        std::string tempFile = "/tmp/tts_" + std::to_string(getpid()) + "_" +
                               std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".wav";
        std::ofstream ofs(tempFile, std::ios::binary);
        if (!ofs) return "";

        ofs.write(responseData.data(), responseData.size());
        ofs.close();

        return tempFile;
    }

    // 读取文件并Base64编码
    std::string readAndEncodeFile(const std::string& filepath) {
        std::ifstream ifs(filepath, std::ios::binary);
        if (!ifs) return "";

        std::ostringstream oss;
        oss << ifs.rdbuf();
        std::string content = oss.str();

        return base64_encode(content);
    }

    // JSON特殊字符转义
    std::string escapeJson(const std::string& input) {
        std::string result;
        for (char c : input) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }

    std::string serverUrl_;        // 服务器地址
    int timeoutSeconds_;           // 超时时间（秒）
};
