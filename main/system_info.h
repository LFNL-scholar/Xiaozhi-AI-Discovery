#ifndef _SYSTEM_INFO_H_
#define _SYSTEM_INFO_H_

#include <string>

#include <esp_err.h>
#include <freertos/FreeRTOS.h>

class SystemInfo {
public:
    static size_t GetFlashSize();
    static size_t GetMinimumFreeHeapSize(); // 堆内存的最小空闲大小
    static size_t GetFreeHeapSize();    // 当前堆内存的空闲大小
    static std::string GetMacAddress();
    static std::string GetChipModelName();
    static esp_err_t PrintRealTimeStats(TickType_t xTicksToWait);
};

#endif // _SYSTEM_INFO_H_
