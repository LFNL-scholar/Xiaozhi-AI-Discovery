#include <esp_log.h>          // 用于日志记录
#include <esp_err.h>          // 用于错误代码处理
#include <nvs.h>              // 用于非易失性存储（NVS）操作
#include <nvs_flash.h>        // 用于初始化和擦除 NVS 存储
#include <driver/gpio.h>      // GPIO 驱动库
#include <esp_event.h>        // ESP-IDF 事件循环功能

#include "application.h"      // 项目自定义的 Application 类（实现主要逻辑）
#include "system_info.h"      // 项目自定义的 SystemInfo 类（获取系统信息）


#define TAG "main"

extern "C" void app_main(void)
{
    // Initialize the default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize NVS flash for WiFi configuration
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS flash to fix corruption");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Otherwise, launch the application
    Application::GetInstance().Start();

    // Dump CPU usage every 10 second
    while (true) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        // SystemInfo::PrintRealTimeStats(pdMS_TO_TICKS(1000));
        int free_sram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        int min_free_sram = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL);
        ESP_LOGI(TAG, "Free internal: %u minimal internal: %u", free_sram, min_free_sram);
    }
}
