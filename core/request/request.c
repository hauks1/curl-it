#include "request.h"

int add_love_data_json_alt(json_t *json, love_data_t *data)
{
    if (json == NULL || data == NULL)
    {
        fprintf(stderr, "JSON or LOVE data pointer is NULL\n");
        return -1;
    }

    // Add the encoded LOVE parameters to the JSON object with error handling
    if (json_add_key(json, "love_r") != 0)
    {
        fprintf(stderr, "Failed to add love_r key to JSON\n");
        return -1;
    }
    if (json_add_string(json, data->r_encoded) != 0)
    {
        fprintf(stderr, "Failed to add love_r value to JSON\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after love_r\n");
        return -1;
    }

    if (json_add_key(json, "love_u1") != 0)
    {
        fprintf(stderr, "Failed to add love_u1 key to JSON\n");
        return -1;
    }
    if (json_add_string(json, data->u1_encoded) != 0)
    {
        fprintf(stderr, "Failed to add love_u1 value to JSON\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after love_u1\n");
        return -1;
    }

    if (json_add_key(json, "love_u2") != 0)
    {
        fprintf(stderr, "Failed to add love_u2 key to JSON\n");
        return -1;
    }
    if (json_add_string(json, data->u2_encoded) != 0)
    {
        fprintf(stderr, "Failed to add love_u2 value to JSON\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after love_u2\n");
        return -1;
    }

    if (json_add_key(json, "love_v2") != 0)
    {
        fprintf(stderr, "Failed to add love_v2 key to JSON\n");
        return -1;
    }
    if (json_add_string(json, data->v2_encoded) != 0)
    {
        fprintf(stderr, "Failed to add love_v2 value to JSON\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after love_v2\n");
        return -1;
    }

    if (json_add_key(json, "love_e") != 0)
    {
        fprintf(stderr, "Failed to add love_e key to JSON\n");
        return -1;
    }
    if (json_add_string(json, data->e_encoded) != 0)
    {
        fprintf(stderr, "Failed to add love_e value to JSON\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after love_e\n");
        return -1;
    }

    return 0;
}

int prepare_req_server(json_t *json, message_t *message, char *master_decoded_sig_buf[],
                       dig_t data_points[], size_t num_data_points,
                       char *pk_b64, int sig_len, uint64_t scale, char *func,
                       love_data_t *love_data)
{
    // Start JSON object
    if (json_start_object(json) != 0)
    {
        fprintf(stderr, "Failed to start JSON object\n");
        return -1;
    }

    // Add ID
    if (json_add_key(json, "id") != 0)
    {
        fprintf(stderr, "Failed to add id key\n");
        return -1;
    }
    if (json_add_string(json, message->ids[0]) != 0)
    {
        fprintf(stderr, "Failed to add id value\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after id\n");
        return -1;
    }

    // Add datapoints array
    if (json_add_key(json, "datapoints") != 0)
    {
        fprintf(stderr, "Failed to add datapoints key\n");
        return -1;
    }
    if (json_start_array(json) != 0)
    {
        fprintf(stderr, "Failed to start datapoints array\n");
        return -1;
    }
    for (size_t i = 0; i < num_data_points; i++)
    {
        if (json_add_number(json, data_points[i]) != 0)
        {
            fprintf(stderr, "Failed to add datapoint %zu\n", i);
            return -1;
        }
        if (i < num_data_points - 1)
        {
            if (json_add_comma(json) != 0)
            {
                fprintf(stderr, "Failed to add comma after datapoint %zu\n", i);
                return -1;
            }
        }
    }
    if (json_end_array(json) != 0)
    {
        fprintf(stderr, "Failed to end datapoints array\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after datapoints array\n");
        return -1;
    }

    // Add signatures array
    if (json_add_key(json, "signatures") != 0)
    {
        fprintf(stderr, "Failed to add signatures key\n");
        return -1;
    }
    if (json_start_array(json) != 0)
    {
        fprintf(stderr, "Failed to start signatures array\n");
        return -1;
    }
    for (size_t i = 0; i < num_data_points; i++)
    {
        if (json_add_string(json, master_decoded_sig_buf[i]) != 0)
        {
            fprintf(stderr, "Failed to add signature %zu\n", i);
            return -1;
        }
        if (i < num_data_points - 1)
        {
            if (json_add_comma(json) != 0)
            {
                fprintf(stderr, "Failed to add comma after signature %zu\n", i);
                return -1;
            }
        }
    }
    if (json_end_array(json) != 0)
    {
        fprintf(stderr, "Failed to end signatures array\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after signatures array\n");
        return -1;
    }

    // Add signature_length
    if (json_add_key(json, "signature_length") != 0)
    {
        fprintf(stderr, "Failed to add signature_length key\n");
        return -1;
    }
    if (json_add_number(json, sig_len) != 0)
    {
        fprintf(stderr, "Failed to add signature_length value\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after signature_length\n");
        return -1;
    }

    // Add tags array
    if (json_add_key(json, "tags") != 0)
    {
        fprintf(stderr, "Failed to add tags key\n");
        return -1;
    }
    if (json_start_array(json) != 0)
    {
        fprintf(stderr, "Failed to start tags array\n");
        return -1;
    }
    for (size_t i = 0; i < num_data_points; i++)
    {
        if (json_add_string(json, message->tags[i]) != 0)
        {
            fprintf(stderr, "Failed to add tag %zu\n", i);
            return -1;
        }
        if (i < num_data_points - 1)
        {
            if (json_add_comma(json) != 0)
            {
                fprintf(stderr, "Failed to add comma after tag %zu\n", i);
                return -1;
            }
        }
    }
    if (json_end_array(json) != 0)
    {
        fprintf(stderr, "Failed to end tags array\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after tags array\n");
        return -1;
    }

    // Add data_set_id
    if (json_add_key(json, "data_set_id") != 0)
    {
        fprintf(stderr, "Failed to add data_set_id key\n");
        return -1;
    }
    if (json_add_string(json, TEST_DATABASE) != 0)
    {
        fprintf(stderr, "Failed to add data_set_id value\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after data_set_id\n");
        return -1;
    }

    // Add public_key
    if (json_add_key(json, "public_key") != 0)
    {
        fprintf(stderr, "Failed to add public_key key\n");
        return -1;
    }
    if (json_add_string(json, pk_b64) != 0)
    {
        fprintf(stderr, "Failed to add public_key value\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after public_key\n");
        return -1;
    }

    // Add scale
    if (json_add_key(json, "scale") != 0)
    {
        fprintf(stderr, "Failed to add scale key\n");
        return -1;
    }
    if (json_add_number(json, scale) != 0)
    {
        fprintf(stderr, "Failed to add scale value\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after scale\n");
        return -1;
    }

    // Add function
    if (json_add_key(json, "function") != 0)
    {
        fprintf(stderr, "Failed to add function key\n");
        return -1;
    }
    if (json_add_string(json, func) != 0)
    {
        fprintf(stderr, "Failed to add function value\n");
        return -1;
    }
    if (json_add_comma(json) != 0)
    {
        fprintf(stderr, "Failed to add comma after function\n");
        return -1;
    }

    // Add LOVE data if provided
    if (love_data != NULL)
    {
        if (add_love_data_json_alt(json, love_data) != 0)
        {
            fprintf(stderr, "Failed to add LOVE data to JSON\n");
            return -1;
        }
    }

    // Close the JSON object
    if (json_end_object(json) != 0)
    {
        fprintf(stderr, "Failed to end JSON object\n");
        return -1;
    }

    return 0;
}
// int prepare_request_server(cJSON *json_obj, message_t *message, unsigned char *master_decoded_sig_buf[], dig_t data_points[], size_t num_data_points, char *pk_b64, int sig_len, uint64_t scale, char *func)
// {
//     // JSON setup and sending
//     cJSON *signatures = cJSON_CreateArray();
//     cJSON *messages = cJSON_CreateArray();
//     cJSON *tags = cJSON_CreateArray();
//     cJSON *data_set_id = cJSON_CreateString(TEST_DATABASE);
//     cJSON *public_key = cJSON_CreateString(pk_b64);
//     cJSON *scale_json = cJSON_CreateNumber(scale);
//     cJSON *func_json = cJSON_CreateString(func);

//     for (size_t i = 0; i < num_data_points; i++)
//     {
//         cJSON *sig = cJSON_CreateString(master_decoded_sig_buf[i]);
//         cJSON_AddItemToArray(signatures, sig);
//         cJSON *msg = cJSON_CreateNumber(data_points[i]);
//         cJSON_AddItemToArray(messages, msg);
//         cJSON *tag = cJSON_CreateString(message->tags[i]);
//         cJSON_AddItemToArray(tags, tag);
//     }
//     cJSON *id = cJSON_CreateString(message->ids[0]);
//     cJSON_AddItemToObject(json_obj, "id", id);
//     cJSON_AddItemToObject(json_obj, "datapoints", messages);
//     if (messages == NULL)
//     {
//         fprintf(stderr, "Failed to create JSON array for messages\n");
//         cJSON_Delete(json_obj);
//         return -1;
//     }
//     cJSON_AddItemToObject(json_obj, "signatures", signatures);
//     if (signatures == NULL)
//     {
//         fprintf(stderr, "Failed to create JSON array for signatures\n");
//         cJSON_Delete(json_obj);
//         return -1;
//     }
//     cJSON_AddNumberToObject(json_obj, "signature_length", sig_len);
//     if (id == NULL)
//     {
//         fprintf(stderr, "Failed to create JSON array for ids\n");
//         cJSON_Delete(json_obj);
//         return -1;
//     }
//     cJSON_AddItemToObject(json_obj, "tags", tags);
//     if (tags == NULL)
//     {
//         fprintf(stderr, "Failed to create JSON array for tags\n");
//         cJSON_Delete(json_obj);
//         return -1;
//     }
//     cJSON_AddItemToObject(json_obj, "data_set_id", data_set_id);
//     if (data_set_id == NULL)
//     {
//         fprintf(stderr, "Failed to create JSON string for data_set_id\n");
//         cJSON_Delete(json_obj);
//         return -1;
//     }
//     cJSON_AddItemToObject(json_obj, "public_key", public_key);
//     if (public_key == NULL)
//     {
//         fprintf(stderr, "Failed to create JSON string for public_key\n");
//         cJSON_Delete(json_obj);
//         return -1;
//     }
//     cJSON_AddItemToObject(json_obj, "scale", scale_json);
//     if (scale_json == NULL)
//     {
//         fprintf(stderr, "Failed to create JSON number for scale\n");
//         cJSON_Delete(json_obj);
//         return -1;
//     }
//     cJSON_AddItemToObject(json_obj, "function", func_json);
//     if (func_json == NULL)
//     {
//         fprintf(stderr, "Failed to create JSON string for function\n");
//         cJSON_Delete(json_obj);
//         return -1;
//     }
//     return 0;
// }

// int add_love_data_json(cJSON *json, love_data_t *data)
// {
//     if (json == NULL || data == NULL)
//     {
//         fprintf(stderr, "JSON or LOVE data pointer is NULL\n");
//         return -1;
//     }

//     // Add the encoded LOVE parameters to the JSON object with error handling
//     if (!cJSON_AddStringToObject(json, "love_r", data->r_encoded))
//     {
//         fprintf(stderr, "Failed to add love_r to JSON\n");
//         return -1;
//     }
//     if (!cJSON_AddStringToObject(json, "love_u1", data->u1_encoded))
//     {
//         fprintf(stderr, "Failed to add love_u1 to JSON\n");
//         return -1;
//     }
//     if (!cJSON_AddStringToObject(json, "love_u2", data->u2_encoded))
//     {
//         fprintf(stderr, "Failed to add love_u2 to JSON\n");
//         return -1;
//     }
//     if (!cJSON_AddStringToObject(json, "love_v2", data->v2_encoded))
//     {
//         fprintf(stderr, "Failed to add love_v2 to JSON\n");
//         return -1;
//     }
//     if (!cJSON_AddStringToObject(json, "love_e", data->e_encoded))
//     {
//         fprintf(stderr, "Failed to add love_e to JSON\n");
//         return -1;
//     }

//     return 0;
// }
