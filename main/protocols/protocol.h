#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cJSON.h>
#include <string>
#include <functional>

struct BinaryProtocol3 {
    uint8_t type;
    uint8_t reserved;
    uint16_t payload_size;
    uint8_t payload[];
} __attribute__((packed));

enum AbortReason {
    kAbortReasonNone,
    kAbortReasonWakeWordDetected
};

enum ListeningMode {
    kListeningModeAutoStop, // 自动停止模式。在这种模式下，设备会在检测到语音活动结束后自动停止监听。
    kListeningModeManualStop, // 手动停止模式。在这种模式下，设备会一直监听，直到收到停止命令。
    kListeningModeAlwaysOn // 实时模式。在这种模式下，设备始终保持监听状态，适用于需要实时响应的场景。
                            // 这种模式通常需要设备支持AEC（回声消除）功能，以避免音频反馈。
};

class Protocol {
public:
    virtual ~Protocol() = default;

    inline int server_sample_rate() const {
        return server_sample_rate_;
    }

    void OnIncomingAudio(std::function<void(std::vector<uint8_t>&& data)> callback);
    void OnIncomingJson(std::function<void(const cJSON* root)> callback);
    void OnAudioChannelOpened(std::function<void()> callback);
    void OnAudioChannelClosed(std::function<void()> callback);
    void OnNetworkError(std::function<void(const std::string& message)> callback);

    virtual bool OpenAudioChannel() = 0;
    virtual void CloseAudioChannel() = 0;
    virtual bool IsAudioChannelOpened() const = 0;
    virtual void SendAudio(const std::vector<uint8_t>& data) = 0;
    virtual void SendWakeWordDetected(const std::string& wake_word);
    virtual void SendStartListening(ListeningMode mode);
    virtual void SendStopListening();
    virtual void SendAbortSpeaking(AbortReason reason);
    virtual void SendIotDescriptors(const std::string& descriptors);
    virtual void SendIotStates(const std::string& states);

protected:
    std::function<void(const cJSON* root)> on_incoming_json_;
    std::function<void(std::vector<uint8_t>&& data)> on_incoming_audio_;
    std::function<void()> on_audio_channel_opened_;
    std::function<void()> on_audio_channel_closed_;
    std::function<void(const std::string& message)> on_network_error_;

    int server_sample_rate_ = 16000;
    std::string session_id_;

    virtual void SendText(const std::string& text) = 0;
};

#endif // PROTOCOL_H

