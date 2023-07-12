#include "nvs_flash.h"
#include "bsp_board.h"
#include "esp_wifi.h"
#include "esp_event.h"
// #include "bsp_lcd.h"
// #include "lv_demo.h"
// #include "lv_port.h"
// #include "lvgl.h"

#include "cJSON.h"

/* User lib */
#include "wifi_connect.h"
#include "https_req.h"
#include "tts_report.h"

extern char *buf;
// void json_parse(char *json)
// {

// }

void app_main(void)
{
    ESP_ERROR_CHECK(bsp_board_init());
    ESP_ERROR_CHECK(bsp_board_power_ctrl(POWER_MODULE_AUDIO, true));

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // LVGL init
    // ESP_ERROR_CHECK(lv_port_init());
    // bsp_lcd_set_backlight(true);
    
    // Connect to WIFI
    wifi_init_sta();

    xTaskCreate(&https_request_task, "https_request_task", 4 * 1024, NULL, 5, NULL);

    /*
     * LVGL显示任务
     * 消息队列传递从HTTPS得到并cJSON解析后的数据至显示任务
     */

    /*
     * TTS
     */
    tts_report();
}