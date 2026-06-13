#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <muduo/base/Logging.h>

// 文件工具类，用于读取和操作文件
class FileUtil
{
public:
    // 构造函数，打开指定路径的文件（二进制模式）
    FileUtil(std::string filePath)
        : filePath_(filePath)
        , file_(filePath, std::ios::binary)
    {}

    // 析构函数，关闭文件
    ~FileUtil()
    {
        file_.close();
    }

    // 检查文件是否有效打开
    bool isValid() const
    { return file_.is_open(); }
    
    // 重置为默认文件（NotFound.html）
    void resetDefaultFile()
    {
        file_.close();
        file_.open("../AIApps/resource/NotFound.html", std::ios::binary);
    }

    // 获取文件大小（字节数）
    uint64_t size()
    {
        file_.seekg(0, std::ios::end);
        uint64_t fileSize = file_.tellg();
        file_.seekg(0, std::ios::beg);
        return fileSize;
    }
    
    // 将文件内容读入缓冲区
    void readFile(std::vector<char>& buffer)
    {
        if (file_.read(buffer.data(), size()))
        {
            LOG_INFO << "File content load into memory (" << size() << " bytes)";
        }    
        else
        {
            LOG_ERROR << "File read failed";
        }
    }

private:
    std::string     filePath_;  // 文件路径
    std::ifstream   file_;      // 文件流
};
