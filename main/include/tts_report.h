#ifndef _TTS_REPORT_H_
#define _TTS_REPORT_H_

#include "esp_err.h"

esp_err_t tts_init(void);
esp_err_t tts_report(char *prompt);

#endif /* _TTS_REPORT_H_ */