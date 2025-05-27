#include "request.h"

int prepare_req_server(json_t *json, message_t *message, char *master_decoded_sig_buf[],
                       dig_t data_points[], size_t num_data_points,
                       char *pk_b64, int sig_len, uint64_t scale, char *func)
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
    // Close the JSON object
    if (json_end_object(json) != 0)
    {
        fprintf(stderr, "Failed to end JSON object\n");
        return -1;
    }

    return 0;
}
