#ifndef _TTS_REPORT_H_
#define _TTS_REPORT_H_

#include "esp_err.h"

esp_err_t tts_init();
esp_err_t tts_report(char *prompt, unsigned int speed);
void tts_report_cb(void *arg);
void tts_welcome_report_cb(void *arg);
void tts_welcome_task(void *pvParams);
void lvgl_show_followers();

inline int get_digit_count(int num)
{
    int digitCount = 0;

    while (num != 0)
    {
        num /= 10;
        digitCount++;
    }

    return digitCount;
}

#endif /* _TTS_REPORT_H_ */