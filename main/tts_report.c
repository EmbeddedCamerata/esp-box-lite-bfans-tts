#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/portmacro.h"
#include "esp_system.h"
#include "esp_tts.h"
#include "esp_tts_voice_xiaole.h"
#include "esp_partition.h"
#include "esp_idf_version.h"
#include "esp_spi_flash.h"
#include "driver/i2s.h"
#include "esp_log.h"

#include "tts_report.h"

static const char *TAG = "TTS report";

static esp_tts_handle_t *tts_handle;

esp_err_t tts_init()
{
    /*** 1. create esp tts handle ***/
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

esp_err_t tts_report(char *prompt)
{
    /*** 2. play prompt text ***/
    // char *prompt1 = "欢迎使用乐鑫语音合成";
    if (esp_tts_parse_chinese(tts_handle, prompt))
    {
        int len[1] = {0};
        size_t bytes_write = 0;
        do
        {
            short *pcm_data = esp_tts_stream_play(tts_handle, len, 0);
#ifdef SDCARD_OUTPUT_ENABLE
            wav_encoder_run(wav_encoder, pcm_data, len[0] * 2);
#else
            i2s_write(I2S_NUM_0, pcm_data, len[0] * 2, &bytes_write, portMAX_DELAY);
#endif
        } while (len[0] > 0);
    }
    esp_tts_stream_reset(tts_handle);
}