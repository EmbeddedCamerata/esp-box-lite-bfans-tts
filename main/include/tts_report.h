#ifndef _TTS_REPORT_H_
#define _TTS_REPORT_H_

#include "esp_err.h"

esp_err_t tts_init(void);

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