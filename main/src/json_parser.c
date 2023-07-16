#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "cJSON.h"

#include "https_req.h"

extern char https_req_buf[MAX_REQUEST_BUF_LEN];
static const char *TAG = "cJSON parser";

/**
 * @brief Parse json string, and get the followers string.
 *
 * @param   json_buf    Json string.
 * @param   parsed      Pointer of parsed followers string.
 * @return
 *  - ESP_OK on success.
 *  - other on failure.
 */
esp_err_t json_parse_followers(char **parsed)
{
    esp_err_t err = ESP_OK;

    char *json_buf;
    cJSON *cjson_root = NULL;

    json_buf = strchr(https_req_buf, '{');
    if (json_buf == NULL)
    {
        ESP_LOGE(TAG, "Couldn't find '{'\n");
        return ESP_FAIL;
    }

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

    *parsed = (char*)malloc(32*sizeof(char));
    *parsed = cJSON_Print(cjson_follower);

exit:
    cJSON_Delete(cjson_root);
    return err;
}