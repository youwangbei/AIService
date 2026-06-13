# 本地语音合成服务 (TTS)

## 概述


## 快速开始

### 1. 安装依赖

```bash
# 安装 Python 依赖
pip install -r tts_requirements.txt

# 推荐安装 edge-tts (微软TTS, 免费, 效果好)
pip install edge-tts flask
```

### 2. 启动 TTS 服务

```bash
# 使用 Edge TTS (推荐, 效果最好)
python3 tts_server.py --engine edge

# 使用 Google TTS (需要网络)
python3 tts_server.py --engine gtts

# 使用 pyttsx3 (完全离线)
python3 tts_server.py --engine pyttsx3

# 使用 Coqui TTS (深度学习模型)
python3 tts_server.py --engine coqui
```

### 3. 启动 HTTP 服务器

```bash
cd build
./http_server
```

### 4. 测试语音合成

```bash
# 登录
curl -X POST http://localhost:8080/login \
  -H "Content-Type: application/json" \
  -d '{"username":"testuser","password":"123456"}' \
  -c cookies.txt

# 测试语音合成 (使用 Edge TTS)
curl -X POST http://localhost:8080/chat/tts \
  -H "Content-Type: application/json" \
  -d '{"text":"你好，这是测试语音","engine":"edge"}' \
  -b cookies.txt
```

## TTS 引擎对比

| 引擎 | 优点 | 缺点 | 推荐场景 |
|------|------|------|---------|
| **edge** | 效果好, 免费, 支持多声音 | 需要网络 | ✅ **首选** |
| gtts | 使用谷歌TTS | 需要网络, 限速 | 备选 |
| pyttsx3 | 完全离线 | 效果一般 | 无网络环境 |
| coqui | 开源深度学习 | 需下载模型 | 追求开源 |

## 使用不同的声音

Edge TTS 支持多种中文声音：

```bash
# 查看可用声音
curl http://127.0.0.1:5000/voices

# 在请求中指定声音
curl -X POST http://localhost:8080/chat/tts \
  -H "Content-Type: application/json" \
  -d '{"text":"你好","engine":"edge","voice":"zh-CN-YunxiNeural"}' \
  -b cookies.txt
```

常用声音：
- `zh-CN-XiaoxiaoNeural` - 女声, 晓晓
- `zh-CN-YunxiNeural` - 男声, 云希
- `zh-CN-YunyangNeural` - 男声, 云扬

## API 端点

### TTS 服务 API

```
POST /synthesize
Content-Type: application/json

{
  "text": "要转换的文本",
  "engine": "edge|gtts|pyttsx3|coqui",
  "lang": "zh-CN",
  "voice": "声音名称 (仅 edge)"
}
```

响应：音频文件 (MP3/WAV)

### HTTP 服务器 API

```
POST /chat/tts
Content-Type: application/json

{
  "text": "要转换的文本",
  "engine": "edge|gtts|pyttsx3|coqui"
}
```

响应：
```json
{
  "success": true,
  "url": "data:audio/mp3;base64,..."
}
```

## 故障排除

### 问题：TTS 服务启动失败

**解决方案：**
```bash
# 检查 Python 版本
python3 --version  # 需要 >= 3.6

# 安装依赖
pip install flask edge-tts gtts

# 测试安装
python3 -c "import edge_tts; print('OK')"
```

### 问题：语音效果不好

**解决方案：**
- 优先使用 `edge` 引擎
- 尝试不同的声音
- 调整语速

### 问题：服务响应慢

**解决方案：**
- 使用 `pyttsx3` 完全离线（但效果较差）
- 减少文本长度
- 使用缓存

## 技术架构

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│   前端      │────▶│ HTTP服务器   │────▶│ TTS服务     │
│   浏览器    │◀────│ C++ Server  │◀────│ Python      │
└─────────────┘     └─────────────┘     └─────────────┘
                                              │
                                              ▼
                                        ┌─────────────┐
                                        │ 音频引擎    │
                                        │ Edge/GTTS  │
                                        └─────────────┘
```

## 许可证

本项目采用 GPLv3 许可证。

TTS 服务使用的引擎：
- Edge TTS: 微软免费服务
- gTTS: 谷歌免费服务
- pyttsx3: MIT 许可证
- Coqui TTS: Mozilla 公共许可证
