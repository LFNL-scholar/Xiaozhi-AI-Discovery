#include "websocket_protocol.h"
#include "board.h"
#include "system_info.h"
#include "application.h"

#include <cstring>
#include <cJSON.h>
#include <esp_log.h>
#include <arpa/inet.h>

#define TAG "WS"

#ifdef CONFIG_CONNECTION_TYPE_WEBSOCKET

// 构造函数：创建事件组用于同步操作
WebsocketProtocol::WebsocketProtocol() {
    event_group_handle_ = xEventGroupCreate();
}

// 析构函数：清理websocket连接和事件组
WebsocketProtocol::~WebsocketProtocol() {
    if (websocket_ != nullptr) {
        delete websocket_;
    }
    vEventGroupDelete(event_group_handle_);
}

// 发送音频数据
// @param data: 要发送的音频数据向量
void WebsocketProtocol::SendAudio(const std::vector<uint8_t>& data) {
    if (websocket_ == nullptr) {
        return;
    }
    // 以二进制格式发送数据(binary=true)
    websocket_->Send(data.data(), data.size(), true);
}

// 发送文本消息
// @param text: 要发送的文本内容
void WebsocketProtocol::SendText(const std::string& text) {
    if (websocket_ == nullptr) {
        return;
    }
    websocket_->Send(text);
}

// 检查音频通道是否已打开
bool WebsocketProtocol::IsAudioChannelOpened() const {
    return websocket_ != nullptr;
}

// 关闭音频通道
void WebsocketProtocol::CloseAudioChannel() {
    if (websocket_ != nullptr) {
        delete websocket_;
        websocket_ = nullptr;
    }
}

// 打开音频通道
bool WebsocketProtocol::OpenAudioChannel() {
    if (websocket_ != nullptr) {
        delete websocket_;
    }

    // 配置websocket连接参数
    std::string url = CONFIG_WEBSOCKET_URL;
    std::string token = "Bearer " + std::string(CONFIG_WEBSOCKET_ACCESS_TOKEN);
    websocket_ = Board::GetInstance().CreateWebSocket();
    
    // 设置请求头
    websocket_->SetHeader("Authorization", token.c_str());
    websocket_->SetHeader("Protocol-Version", "1");
    websocket_->SetHeader("Device-Id", SystemInfo::GetMacAddress().c_str());

    // 设置数据接收回调
    websocket_->OnData([this](const char* data, size_t len, bool binary) {
        if (binary) {
            // 处理二进制数据(音频)
            if (on_incoming_audio_ != nullptr) {
                on_incoming_audio_(std::vector<uint8_t>((uint8_t*)data, (uint8_t*)data + len));
            }
        } else {
            // 处理JSON数据
            auto root = cJSON_Parse(data);
            auto type = cJSON_GetObjectItem(root, "type");
            if (type != NULL) {
                if (strcmp(type->valuestring, "hello") == 0) {
                    // 处理服务器的hello消息
                    ParseServerHello(root);
                } else {
                    // 处理其他JSON消息
                    if (on_incoming_json_ != nullptr) {
                        on_incoming_json_(root);
                    }
                }
            } else {
                ESP_LOGE(TAG, "Missing message type, data: %s", data);
            }
            cJSON_Delete(root);
        }
    });

    // 设置断开连接回调
    websocket_->OnDisconnected([this]() {
        ESP_LOGI(TAG, "Websocket disconnected");
        if (on_audio_channel_closed_ != nullptr) {
            on_audio_channel_closed_();
        }
    });

    // 建立websocket连接
    if (!websocket_->Connect(url.c_str())) {
        ESP_LOGE(TAG, "Failed to connect to websocket server");
        if (on_network_error_ != nullptr) {
            on_network_error_("无法连接服务");
        }
        return false;
    }

    // 构造并发送客户端hello消息
    std::string message = "{";
    message += "\"type\":\"hello\",";
    message += "\"version\": 1,";
    message += "\"transport\":\"websocket\",";
    message += "\"audio_params\":{";
    message += "\"format\":\"opus\", \"sample_rate\":16000, \"channels\":1, \"frame_duration\":" + std::to_string(OPUS_FRAME_DURATION_MS);
    message += "}}";
    websocket_->Send(message);

    // 等待服务器hello响应，超时时间10秒
    EventBits_t bits = xEventGroupWaitBits(event_group_handle_, 
                                          WEBSOCKET_PROTOCOL_SERVER_HELLO_EVENT, 
                                          pdTRUE,  // 清除事件位
                                          pdFALSE, // 任一事件满足即可
                                          pdMS_TO_TICKS(10000));
                                          
    if (!(bits & WEBSOCKET_PROTOCOL_SERVER_HELLO_EVENT)) {
        ESP_LOGE(TAG, "Failed to receive server hello");
        if (on_network_error_ != nullptr) {
            on_network_error_("等待响应超时");
        }
        return false;
    }

    // 通知音频通道已打开
    if (on_audio_channel_opened_ != nullptr) {
        on_audio_channel_opened_();
    }

    return true;
}

// 解析服务器发来的hello消息
// @param root: 解析后的JSON对象
void WebsocketProtocol::ParseServerHello(const cJSON* root) {
    // 检查传输方式
    auto transport = cJSON_GetObjectItem(root, "transport");
    if (transport == nullptr || strcmp(transport->valuestring, "websocket") != 0) {
        ESP_LOGE(TAG, "Unsupported transport: %s", transport->valuestring);
        return;
    }

    // 获取音频参数
    auto audio_params = cJSON_GetObjectItem(root, "audio_params");
    if (audio_params != NULL) {
        auto sample_rate = cJSON_GetObjectItem(audio_params, "sample_rate");
        if (sample_rate != NULL) {
            server_sample_rate_ = sample_rate->valueint;
        }
    }

    // 设置收到服务器hello的事件标志
    xEventGroupSetBits(event_group_handle_, WEBSOCKET_PROTOCOL_SERVER_HELLO_EVENT);
}

#endif
