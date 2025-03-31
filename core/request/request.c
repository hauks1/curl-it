#include <cjson/cJSON.h>
#include "request.h"
#include "../crypto/love/love.h"
#include "../message/message.h"

int prepare_request_server(cJSON *json_obj, message_t *message, unsigned char *master_decoded_sig_buf[], dig_t data_points[], size_t num_data_points, char *pk_b64, int sig_len, uint64_t scale, char *func)
{
    // JSON setup and sending
    cJSON *signatures = cJSON_CreateArray();
    cJSON *messages = cJSON_CreateArray();
    cJSON *tags = cJSON_CreateArray();
    cJSON *data_set_id = cJSON_CreateString(TEST_DATABASE);
    cJSON *public_key = cJSON_CreateString(pk_b64);
    cJSON *scale_json = cJSON_CreateNumber(scale);
    cJSON *func_json = cJSON_CreateString(func);

    for (size_t i = 0; i < num_data_points; i++)
    {
        cJSON *sig = cJSON_CreateString(master_decoded_sig_buf[i]);
        cJSON_AddItemToArray(signatures, sig);
        cJSON *msg = cJSON_CreateNumber(data_points[i]);
        cJSON_AddItemToArray(messages, msg);
        cJSON *tag = cJSON_CreateString(message->tags[i]);
        cJSON_AddItemToArray(tags, tag);
    }
    cJSON *id = cJSON_CreateString(message->ids[0]);
    cJSON_AddItemToObject(json_obj, "id", id);
    cJSON_AddItemToObject(json_obj, "datapoints", messages);
    if (messages == NULL)
    {
        fprintf(stderr, "Failed to create JSON array for messages\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddItemToObject(json_obj, "signatures", signatures);
    if (signatures == NULL)
    {
        fprintf(stderr, "Failed to create JSON array for signatures\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddNumberToObject(json_obj, "signature_length", sig_len);
    if (id == NULL)
    {
        fprintf(stderr, "Failed to create JSON array for ids\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddItemToObject(json_obj, "tags", tags);
    if (tags == NULL)
    {
        fprintf(stderr, "Failed to create JSON array for tags\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddItemToObject(json_obj, "data_set_id", data_set_id);
    if (data_set_id == NULL)
    {
        fprintf(stderr, "Failed to create JSON string for data_set_id\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddItemToObject(json_obj, "public_key", public_key);
    if (public_key == NULL)
    {
        fprintf(stderr, "Failed to create JSON string for public_key\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddItemToObject(json_obj, "scale", scale_json);
    if (scale_json == NULL)
    {
        fprintf(stderr, "Failed to create JSON number for scale\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddItemToObject(json_obj, "function", func_json);
    if (func_json == NULL)
    {
        fprintf(stderr, "Failed to create JSON string for function\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    return 0;
}

int add_love_data_json(cJSON *json, love_data_t *data)
{
    if (json == NULL || data == NULL)
    {
        fprintf(stderr, "JSON or LOVE data pointer is NULL\n");
        return -1;
    }

    // Add the encoded LOVE parameters to the JSON object with error handling
    if (!cJSON_AddStringToObject(json, "love_r", data->r_encoded))
    {
        fprintf(stderr, "Failed to add love_r to JSON\n");
        return -1;
    }
    if (!cJSON_AddStringToObject(json, "love_u1", data->u1_encoded))
    {
        fprintf(stderr, "Failed to add love_u1 to JSON\n");
        return -1;
    }
    if (!cJSON_AddStringToObject(json, "love_u2", data->u2_encoded))
    {
        fprintf(stderr, "Failed to add love_u2 to JSON\n");
        return -1;
    }
    if (!cJSON_AddStringToObject(json, "love_v2", data->v2_encoded))
    {
        fprintf(stderr, "Failed to add love_v2 to JSON\n");
        return -1;
    }
    if (!cJSON_AddStringToObject(json, "love_e", data->e_encoded))
    {
        fprintf(stderr, "Failed to add love_e to JSON\n");
        return -1;
    }

    return 0;
}
