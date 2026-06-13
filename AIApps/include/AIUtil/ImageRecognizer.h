#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <onnxruntime_cxx_api.h>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>

// 图像识别器类，使用ONNX Runtime进行图像识别
class ImageRecognizer {
public:
    // 构造函数
    explicit ImageRecognizer(const std::string& model_path,
        const std::string& label_path = "../AIApps/resource/model/imagenet_classes.txt");

    // 从文件预测
    std::string PredictFromFile(const std::string& image_path);
    // 从缓冲区预测
    std::string PredictFromBuffer(const std::vector<unsigned char>& image_data);
    // 从OpenCV Mat预测
    std::string PredictFromMat(const cv::Mat& img);

private:
    Ort::Env env;                              // ONNX Runtime环境
    std::unique_ptr<Ort::Session> session;     // ONNX会话
    std::unique_ptr<Ort::AllocatorWithDefaultOptions> allocator;  // 内存分配器

    std::string input_name;                    // 输入节点名称
    std::string output_name;                   // 输出节点名称
    std::vector<int64_t> input_shape;          // 输入形状
    int input_height{}, input_width{};         // 输入高度和宽度

    std::vector<std::string> labels;           // 标签列表

    // 加载标签
    void LoadLabels(const std::string& label_path);
};
