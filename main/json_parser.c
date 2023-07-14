#include "esp_err.h"
#include "esp_log.h"
#include "cJSON.h"

static const char *TAG = "CJson parser";

/**
 * @brief Parse json string, and get the fans string.
 *
 * @param   json_buf    Json string.
 * @param   parsed      Pointer of parsed fans string.
 * @return
 *  - ESP_OK on success.
 *  - other on failure.
 */
esp_err_t json_parse_fans(const char *json_buf, char **parsed)
{
    esp_err_t err = ESP_OK;
    cJSON *cjson_root = NULL;
    
    cjson_root = cJSON_Parse(json_buf);
    if (cjson_root == NULL)
    {
        ESP_LOGE(TAG, "Parsed json failed.\n");
        err = ESP_FAIL;
        goto exit;
    }

    cJSON *cjson_data = cJSON_GetObjectItem(cjson_root, "data");
    if (cjson_data == NULL)
    {
        ESP_LOGE(TAG, "Parsed \"data\" failed.\n");
        err = ESP_FAIL;
        goto exit;
    }

    cJSON *cjson_follower = cJSON_GetObjectItem(cjson_data, "follower");
    if (cjson_follower == NULL)
    {
        ESP_LOGE(TAG, "Parsed \"follower\" failed.\n");
        err = ESP_FAIL;
        goto exit;
    }

    *parsed = (char *)malloc(64 * sizeof(char));
    *parsed = cJSON_Print(cjson_follower);

    ESP_LOGD(TAG, "follower: %s\n", parsed);

exit:
    cJSON_Delete(cjson_root);
    return err;
}