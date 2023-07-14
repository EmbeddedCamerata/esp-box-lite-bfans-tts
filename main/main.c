#include "nvs_flash.h"
#include "esp_wifi.h"
#include "freertos/portmacro.h"

/* BSP lib */
// #include "bsp_lcd.h"
#include "bsp_board.h"
#include "bsp_btn.h"

// #include "lv_demo.h"
// #include "lv_port.h"
// #include "lvgl.h"

/* User lib */
#include "wifi_connect.h"
#include "https_req.h"
#include "tts_report.h"

extern char https_req_buf[MAX_REQUEST_BUF_LEN];
static void tts_welcome_task(void *pvParams);

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

    ESP_ERROR_CHECK(tts_init());

    // Connect to WIFI
    wifi_init_sta();

    // LVGL init
    // ESP_ERROR_CHECK(lv_port_init());
    // bsp_lcd_set_backlight(true);

    // TODO LVGL显示任务
    // 消息队列传递从HTTPS得到并cJSON解析后的数据至显示任务

    xTaskCreate(&tts_welcome_task, "tts_welcome_task", 4 * 1024, NULL, 5, NULL);
    xTaskCreate(&https_request_task, "https_request_task", 8 * 1024, NULL, 5, NULL);

    /* Register buttons callback functions */
    bsp_btn_register_callback(BOARD_BTN_ID_PREV, BUTTON_PRESS_DOWN, tts_report_cb, https_req_buf);
    bsp_btn_register_callback(BOARD_BTN_ID_ENTER, BUTTON_PRESS_DOWN, tts_welcome_report_cb, NULL);
}

static void tts_welcome_task(void *pvParams)
{
    tts_welcome_report();
    vTaskDelete(NULL);
}