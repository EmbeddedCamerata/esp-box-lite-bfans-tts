#ifndef _JSON_PARSER_H_
#define _JSON_PARSER_H_

#include "esp_err.h"

esp_err_t json_parse_fans(const char *json_buf, char **parsed);

#endif /* _JSON_PARSER_H_ */