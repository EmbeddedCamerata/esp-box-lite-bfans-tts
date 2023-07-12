#include "esp_err.h"
#include "esp_log.h"
#include "cJSON.h"

static const char *TAG = "CJson parser";

esp_err_t json_parser(char *json_buf, char* parsed)
{
    cJSON *cjson_root = NULL;
    cjson_root = cJSON_Parse(json_buf);

    if (cjson_root == NULL)
    {
        ESP_LOGE(TAG, "Parsed json failed.\n");
        return ESP_FAIL;
    }

    /*
        {
            "code":0,
            "message":"0",
            "ttl":1,
            "data":{
                "mid":42602419,
                "following":246,
                "whisper":0,
                "black":0,
                "follower":69
            }
        }
    */
    cJSON *cjson_data = cJSON_GetObjectItem(cjson_root, "data");
    cJSON *cjson_follower = cJSON_GetObjectItem(cjson_data, "follower");

    parsed = cJSON_Print(cjson_follower);
    
    ESP_LOGI(TAG, "follower: %s\n", parsed);

    return ESP_OK;
}

// TODO Write parser task with message queue.