#include <stdio.h>
#include <stdlib.h>
#include "freertos/portmacro.h"
#include "esp_tts.h"
#include "esp_tts_voice_xiaole.h"
#include "esp_partition.h"
#include "esp_idf_version.h"
#include "esp_spi_flash.h"
#include "driver/i2s.h"
#include "esp_log.h"

/* BSP lib */
#include "bsp_board.h"
#include "bsp_btn.h"

/* User lib */
#include "tts_report.h"
#include "https_req.h"

#define MAX_NUM_LIMIT 9999
#define MIN_NUM_LIMIT 1

static const char *TAG = "TTS report";
static esp_tts_handle_t *tts_handle;

static const char *unit_to_chinese[] = {
    ".", "十", "百", "千", "万"};
static const char *number_to_chinese[] = {
    "零", "一", "二", "三", "四", "五", "六", "七", "八", "九"};

static esp_err_t followers_to_prompt(const char *followers_num_str, char *prompt);

esp_err_t tts_init()
{
    /* 1. create esp tts handle */
    // initial voice set from separate voice data partition
    const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "voice_data");
    if (part == NULL)
    {
        ESP_LOGE(TAG, "Couldn't find voice data partition!\n");
        return ESP_FAIL;
    }
    else
    {
        ESP_LOGI(TAG, "voice_data paration size:%d\n", part->size);
    }
    void *voicedata;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    esp_partition_mmap_handle_t mmap;
    esp_err_t err = esp_partition_mmap(part, 0, part->size, ESP_PARTITION_MMAP_DATA, &voicedata, &mmap);
#else
    spi_flash_mmap_handle_t mmap;
    esp_err_t err = esp_partition_mmap(part, 0, part->size, SPI_FLASH_MMAP_DATA, &voicedata, &mmap);
#endif
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Couldn't map voice data partition!\n");
        return ESP_FAIL;
    }
    esp_tts_voice_t *voice = esp_tts_voice_set_init(&esp_tts_voice_xiaole, (int16_t *)voicedata);

    tts_handle = esp_tts_create(voice);

    if (!tts_handle)
    {
        ESP_LOGE(TAG, "Created tts_handle failed!\n");
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief convert followers number string to chinese prompt.
 *
 * @param   followers_num_str    followers number string.
 * @param   prompt          Converted Chinese prompt.
 * @return
 *  - ESP_OK on success.
 *  - other on failure.
 */
static esp_err_t followers_to_prompt(const char *followers_num_str, char *prompt)
{
    const char *_followers = "粉丝数";
    char number;
    int num = atoi(followers_num_str);
    int digit_cnt = 0;
    int digit, digit_next;
    int i = 0;

    if (MIN_NUM_LIMIT > num || MAX_NUM_LIMIT < num)
    {
        ESP_LOGE(TAG, "Out of range: %d\n", num);
        return ESP_ERR_INVALID_ARG;
    }

    bzero(prompt, sizeof(prompt));
    strcat(prompt, _followers);
    prompt += strlen(prompt);

    digit_cnt = get_digit_count(num);
    for (; i < strlen(followers_num_str); i++)
    {
        number = followers_num_str[i];
        digit = atoi(&number);

        if (digit == 0)
        {
            if (num % 1000 > 0)
            {
                // See the next number whether is zero
                number = followers_num_str[i + 1];
                digit_next = atoi(&number);
                if (digit_next > 0)
                {
                    strcat(prompt, number_to_chinese[digit]);
                }
            }
        }
        else
        {
            strcat(prompt, number_to_chinese[digit]);
            strcat(prompt, unit_to_chinese[digit_cnt - 1]);
        }
        prompt += strlen(prompt);
        digit_cnt--;
    }

    prompt[i] = '\0';

    return ESP_OK;
}

void tts_report_cb(void *arg)
{
    /* 2. play prompt text */
    button_dev_t *btn = (button_dev_t *)arg;
    char *buf = (char *)btn->cb_user_data;

    char *parsed_num;
    char prompt[64];

    esp_err_t err = json_parse_followers(buf, &parsed_num);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "tts report failed!\n");
        return;
    }

    err = followers_to_prompt(parsed_num, prompt);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "converted followers string to Chinese prompt failed!\n");
        return;
    }
    ESP_LOGI(TAG, "prompt = %s\n", prompt);

    err = tts_report(prompt, 1);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "TTS report failed!\n");
        return;
    }
}

void tts_welcome_report_cb(void *arg)
{
    tts_welcome_report();
}

void tts_welcome_report()
{
    char *welcome1 = "原神";
    char *welcome2 = "启动";

    tts_report(welcome1, 1);
    vTaskDelay(1200 / portTICK_RATE_MS);
    tts_report(welcome2, 1);
}

esp_err_t tts_report(char *prompt, unsigned int speed)
{
    if (esp_tts_parse_chinese(tts_handle, prompt))
    {
        int len[1] = {0};
        size_t bytes_write = 0;
        do
        {
            short *pcm_data = esp_tts_stream_play(tts_handle, len, speed);
#ifdef SDCARD_OUTPUT_ENABLE
            wav_encoder_run(wav_encoder, pcm_data, len[0] * 2);
#else
            i2s_write(I2S_NUM_0, pcm_data, len[0] * 2, &bytes_write, portMAX_DELAY);
#endif
        } while (len[0] > 0);
    }
    else
    {
        ESP_LOGE(TAG, "Parse %s failed!\n", prompt);
        return ESP_FAIL;
    }
    esp_tts_stream_reset(tts_handle);

    return ESP_OK;
}