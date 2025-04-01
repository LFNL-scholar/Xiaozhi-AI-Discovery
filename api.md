# 小智ESP32服务器API文档

## 概述
本文档描述了小智ESP32设备通信系统的服务器端API实现。API支持MQTT和WebSocket协议进行设备通信，并使用UDP进行音频流传输。

## 传输协议

### MQTT配置
- **端口**: 8883 (TLS)
- **保活时间**: 90秒
- **主题**:
  - 订阅: `voice/+/up` (+ 为设备ID)
  - 发布: `voice/{device_id}/down`

### WebSocket配置
- **端口**: 8443 (WSS)
- **路径**: `/ws/{device_id}`

### UDP配置（音频流）
- **端口**: 动态分配
- **加密**: AES-CTR
- **帧格式**: `[16字节随机数][加密音频数据]`

## API端点

### 1. 会话管理

#### 1.1 客户端问候
- **方向**: 设备 → 服务器
- **类型**: `hello`
```json
{
    "type": "hello",
    "device_id": "string",
    "transport": "mqtt|websocket",
    "audio_params": {
        "format": "opus",
        "sample_rate": 16000,
        "channels": 1,
        "frame_duration": 30
    }
}
```

#### 1.2 服务器问候
- **方向**: 服务器 → 设备
- **类型**: `hello`
```json
{
    "type": "hello",
    "transport": "mqtt|websocket",
    "session_id": "string",
    "audio_params": {
        "sample_rate": 16000
    },
    "udp": {
        "server": "string",
        "port": 12345,
        "key": "base64_string",
        "nonce": "base64_string"
    }
}
```

### 2. 语音交互

#### 2.1 开始监听
- **方向**: 服务器 → 设备
- **类型**: `listen`
```json
{
    "type": "listen",
    "mode": "auto|manual|realtime"
}
```

#### 2.2 停止监听
- **方向**: 服务器 → 设备
- **类型**: `stop`
```json
{
    "type": "stop"
}
```

#### 2.3 唤醒词检测
- **方向**: 设备 → 服务器
- **类型**: `wake`
```json
{
    "type": "wake",
    "wake_word": "string"
}
```

#### 2.4 语音转文字结果
- **方向**: 服务器 → 设备
- **类型**: `stt`
```json
{
    "type": "stt",
    "text": "string",
    "is_final": true
}
```

#### 2.5 文字转语音控制
- **方向**: 服务器 → 设备
- **类型**: `tts`
```json
{
    "type": "tts",
    "state": "start|sentence_start|stop",
    "text": "string"  // 仅在 sentence_start 时使用
}
```

### 3. 显示控制

#### 3.1 表情显示
- **方向**: 服务器 → 设备
- **类型**: `llm`
```json
{
    "type": "llm",
    "emotion": "neutral|happy|sad|thinking"
}
```

#### 3.2 聊天消息
- **方向**: 服务器 → 设备
- **类型**: `chat`
```json
{
    "type": "chat",
    "role": "user|assistant",
    "content": "string"
}
```

#### 3.3 通知
- **方向**: 服务器 → 设备
- **类型**: `notification`
```json
{
    "type": "notification",
    "text": "string",
    "duration": 3000  // 可选，单位：毫秒
}
```

### 4. IoT控制

#### 4.1 设备描述
- **方向**: 设备 → 服务器
- **类型**: `iot_descriptors`
```json
{
    "type": "iot_descriptors",
    "devices": {
        "device_id": {
            "type": "string",
            "name": "string",
            "capabilities": ["string"]
        }
    }
}
```

#### 4.2 设备状态
- **方向**: 设备 → 服务器
- **类型**: `iot_states`
```json
{
    "type": "iot_states",
    "states": {
        "device_id": {
            "power": "on|off",
            "brightness": 50
        }
    }
}
```

#### 4.3 设备命令
- **方向**: 服务器 → 设备
- **类型**: `iot_command`
```json
{
    "type": "iot_command",
    "commands": [
        {
            "device": "device_id",
            "action": "string",
            "params": {}
        }
    ]
}
```

### 5. 系统管理

#### 5.1 版本检查
- **方向**: 设备 → 服务器
- **类型**: `version`
```json
{
    "type": "version",
    "current_version": "string"
}
```

#### 5.2 更新信息
- **方向**: 服务器 → 设备
- **类型**: `update`
```json
{
    "type": "update",
    "version": "string",
    "url": "string",
    "checksum": "string"
}
```

## 错误处理

### 错误响应
- **方向**: 双向
- **类型**: `error`
```json
{
    "type": "error",
    "code": "string",
    "message": "string"
}
```

常见错误代码：
- `invalid_message`: 消息格式或内容无效
- `session_expired`: 会话已过期
- `auth_failed`: 认证失败
- `server_error`: 服务器内部错误
- `device_error`: 设备端错误

## 安全性

### TLS配置
- 最低TLS版本: 1.2
- 必需的密码套件:
  - TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
  - TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256

### 音频加密
- 算法: AES-CTR
- 密钥大小: 256位
- 随机数大小: 16字节
- 密钥交换: 在会话建立期间

## 速率限制

- 每个设备最大连接数: 1
- 最大消息速率: 100条/秒
- 最大音频包速率: 50包/秒

## 实现注意事项

1. 所有时间戳应使用ISO 8601格式，采用UTC时区
2. 所有文本应使用UTF-8编码
3. 二进制数据应使用base64编码
4. 音频数据应使用Opus编码
5. 会话超时时间: 30分钟无活动

## 示例流程

### 1. 正常交互流程
1. 设备连接并发送问候
2. 服务器响应问候并发送会话信息
3. 设备发送唤醒词检测
4. 服务器发送开始监听
5. 设备传输音频流
6. 服务器发送语音转文字结果
7. 服务器发送文字转语音响应
8. 服务器发送停止监听

### 2. IoT控制流程
1. 设备发送设备描述
2. 设备发送当前状态
3. 服务器发送控制命令
4. 设备执行命令
5. 设备发送更新后的状态

### 3. 更新流程
1. 设备发送版本检查
2. 服务器发送更新信息
3. 设备下载并验证更新
4. 设备应用更新并重启 