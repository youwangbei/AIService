#!/usr/bin/env python3
from flask import Flask, request, jsonify, send_file
import tempfile
import os
import sys
import argparse

app = Flask(__name__)

TTS_ENGINE = 'gtts'
TEMP_DIR = tempfile.gettempdir()

def synthesize_gtts(text, lang='zh-CN', output_file=None):
    try:
        from gtts import gTTS
        if output_file is None:
            output_file = os.path.join(TEMP_DIR, 'tts_output.mp3')

        tts = gTTS(text=text, lang=lang, slow=False)
        tts.save(output_file)
        return output_file
    except Exception as e:
        raise Exception(f"gTTS 合成失败: {str(e)}")

def synthesize_pyttsx3(text, output_file=None):
    try:
        import subprocess
        import time
        if output_file is None:
            output_file = os.path.join(TEMP_DIR, 'tts_output.wav')

        temp_wav = output_file
        
        script = f'''
import pyttsx3
import sys

text = {repr(text)}
output_file = {repr(temp_wav)}

engine = pyttsx3.init()
engine.setProperty('rate', 150)
engine.setProperty('volume', 0.9)

voices = engine.getProperty('voices')
chinese_voice = None
for voice in voices:
    if "chinese" in voice.name.lower() or "zh" in voice.name.lower():
        chinese_voice = voice
        break

if chinese_voice is None:
    for voice in voices:
        if "en" in voice.name.lower():
            chinese_voice = voice
            break

if chinese_voice:
    engine.setProperty("voice", chinese_voice.id)

engine.save_to_file(text, output_file)
engine.runAndWait()
engine.stop()
del engine
'''
        result = subprocess.run([sys.executable, '-c', script], 
                               capture_output=True, text=True)
        
        if result.returncode != 0:
            raise Exception(f"pyttsx3 子进程执行失败: {result.stderr}")
        
        time.sleep(0.3)
        
        if not os.path.exists(temp_wav):
            raise Exception("音频文件未生成")
        
        file_size = os.path.getsize(temp_wav)
        if file_size < 100:
            raise Exception(f"音频文件太小: {file_size} bytes")
        
        print(f"Audio saved to {temp_wav} (size: {file_size} bytes)")
        return temp_wav
    except Exception as e:
        raise Exception(f"pyttsx3 合成失败: {str(e)}")

def synthesize_edge_tts(text, output_file=None, voice='zh-CN-XiaoxiaoNeural'):
    try:
        import asyncio
        from edge_tts import Communicator

        if output_file is None:
            output_file = os.path.join(TEMP_DIR, 'tts_output.mp3')

        async def generate():
            communicator = Communicator(text, voice)
            await communicator.save(output_file)

        asyncio.get_event_loop().run_until_complete(generate())
        return output_file
    except ImportError:
        raise Exception("请安装 edge-tts: pip install edge-tts")
    except Exception as e:
        raise Exception(f"Edge TTS 合成失败: {str(e)}")

def synthesize_coqui(text, output_file=None):
    try:
        from TTS.api import TTS

        if output_file is None:
            output_file = os.path.join(TEMP_DIR, 'tts_output.wav')

        tts = TTS(model_name="tts_models/zh-CN/baker/tacotron2-DDCG", progress_bar=False, gpu=False)
        tts.tts_to_file(text=text, file_path=output_file)
        return output_file
    except ImportError:
        raise Exception("请安装 Coqui TTS: pip install TTS")
    except Exception as e:
        raise Exception(f"Coqui TTS 合成失败: {str(e)}")

@app.route('/synthesize', methods=['POST'])
def synthesize():
    try:
        data = request.get_json()
        if not data or 'text' not in data:
            return jsonify({'error': '缺少 text 参数'}), 400

        text = data['text']
        if not text or len(text.strip()) == 0:
            return jsonify({'error': 'text 不能为空'}), 400

        engine = data.get('engine', TTS_ENGINE).lower()
        lang = data.get('lang', 'zh-CN')
        voice = data.get('voice', 'zh-CN-XiaoxiaoNeural')

        output_file = os.path.join(TEMP_DIR, f'tts_{os.getpid()}.mp3')

        if engine == 'gtts':
            result_file = synthesize_gtts(text, lang, output_file)
        elif engine == 'pyttsx3':
            result_file = synthesize_pyttsx3(text, output_file.replace('.mp3', '.wav'))
        elif engine == 'edge':
            result_file = synthesize_edge_tts(text, output_file, voice)
        elif engine == 'coqui':
            result_file = synthesize_coqui(text, output_file.replace('.mp3', '.wav'))
        else:
            return jsonify({'error': f'不支持的引擎: {engine}'}), 400

        return send_file(result_file, mimetype='audio/mpeg' if result_file.endswith('.mp3') else 'audio/wav')

    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/health', methods=['GET'])
def health():
    return jsonify({'status': 'ok', 'engine': TTS_ENGINE})

@app.route('/voices', methods=['GET'])
def voices():
    return jsonify({
        'engines': {
            'gtts': {'name': 'Google TTS', 'offline': False, 'languages': ['zh-CN', 'zh-TW', 'en']},
            'edge': {'name': 'Microsoft Edge TTS', 'offline': False, 'voices': ['zh-CN-XiaoxiaoNeural', 'zh-CN-YunxiNeural', 'zh-CN-YunyangNeural']},
            'pyttsx3': {'name': 'pyttsx3', 'offline': True, 'languages': ['system']},
            'coqui': {'name': 'Coqui TTS', 'offline': True, 'languages': ['zh-CN']}
        }
    })

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='本地 TTS 服务')
    parser.add_argument('--engine', '-e', default='edge', choices=['gtts', 'edge', 'pyttsx3', 'coqui'],
                       help='TTS 引擎 (默认: edge)')
    parser.add_argument('--port', '-p', type=int, default=5000, help='服务端口 (默认: 5000)')
    parser.add_argument('--host', default='127.0.0.1', help='服务地址 (默认: 127.0.0.1)')

    args = parser.parse_args()
    TTS_ENGINE = args.engine

    print(f"启动 TTS 服务，使用引擎: {args.engine}")
    print(f"服务地址: http://{args.host}:{args.port}")

    app.run(host=args.host, port=args.port, debug=False)
