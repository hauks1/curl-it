#include "json.h"
#include "../utils/bad_string.h"
#include "../utils/base64.h"
#include <cjson/cJSON.h>
#include "relic/relic.h"

void json_init(json_t *json, char *buffer, size_t capacity)
{
    json->buffer = buffer;
    json->buffer[0] = '\0';
    json->capacity = capacity;
    json->pos = 0;
    json->error = 0;
}

int json_check_capacity(json_t *json, size_t needed)
{
    if (json->pos + needed >= json->capacity)
        return json->error = -1;

    return 0;
}

int json_write(json_t *json, const char *str)
{
    size_t len = bad_strlen(str);
    if (json_check_capacity(json, len) != 0)
    {
        json->error = -1;
        return json->error;
    }

    bad_strcpy(json->buffer + json->pos, str);
    json->pos += len;
    json->buffer[json->pos] = '\0';

    return 0;
}

int json_add_comma(json_t *json)
{
    return json_write(json, ",");
}

int json_start_object(json_t *json)
{
    return json_write(json, "{");
}

int json_end_object(json_t *json)
{
    // Remove trailing comma if exists
    if (json->pos > 0 && json->buffer[json->pos - 1] == ',')
    {
        json->pos--;
        json->buffer[json->pos] = '\0';
    }
    return json_write(json, "}");
}

int json_start_array(json_t *json)
{
    return json_write(json, "[");
}

int json_end_array(json_t *json)
{
    // Remove trailing comma if exists
    if (json->pos > 0 && json->buffer[json->pos - 1] == ',')
    {
        json->pos--;
        json->buffer[json->pos] = '\0';
    }
    return json_write(json, "]");
}

int json_add_key(json_t *json, const char *key)
{
    if (json_write(json, "\"") != 0)
        return json->error = -1;
    if (json_write(json, key) != 0)
        return json->error = -1;
    return json_write(json, "\":");
}

int json_add_string(json_t *json, const char *str)
{
    if (json_write(json, "\"") != 0)
        return json->error = -1;
    if (json_write(json, str) != 0)
        return json->error = -1;
    return json_write(json, "\"");
}

// Modified to separate adding the number from adding the comma
int json_add_number(json_t *json, unsigned long long number)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%llu", number);
    return json_write(json, buffer);
}

// Add key-value pair with automatic comma
int json_add_key_value_string(json_t *json, const char *key, const char *value)
{
    if (json_add_key(json, key) != 0)
        return json->error = -1;
    if (json_add_string(json, value) != 0)
        return json->error = -1;
    return json_add_comma(json);
}

// Add key-value pair with automatic comma
int json_add_key_value_number(json_t *json, const char *key, unsigned long long value)
{
    if (json_add_key(json, key) != 0)
        return json->error = -1;
    if (json_add_number(json, value) != 0)
        return json->error = -1;
    return json_add_comma(json);
}

int json_add_string_array(json_t *json, const char *key, const char **vals, int count)
{
    if (count == 0)
        return json->error = -1;
    if (json_add_key(json, key) != 0)
        return json->error = -1;
    if (json_start_array(json) != 0)
        return json->error = -1;
    for (int i = 0; i < count; i++)
    {
        if (json_add_string(json, vals[i]) != 0)
            return json->error = -1;
        // Check if it's the last element
        if (i < count - 1)
        {
            if (json_add_comma(json) != 0)
                return json->error = -1;
        }
    }
    if (json_end_array(json) != 0)
        return json->error = -1;
    return json_add_comma(json); // Add comma after the array
}

int json_add_number_array(json_t *json, const char *key, const unsigned long long *vals, int count)
{
    if (count == 0)
        return json->error = -1;
    if (json_add_key(json, key) != 0)
        return json->error = -1;
    if (json_start_array(json) != 0)
        return json->error = -1;
    for (int i = 0; i < count; i++)
    {
        if (json_add_number(json, vals[i]) != 0)
            return json->error = -1;
        // Check if it's the last element
        if (i < count - 1)
        {
            if (json_add_comma(json) != 0)
                return json->error = -1;
        }
    }
    if (json_end_array(json) != 0)
        return json->error = -1;
    return json_add_comma(json);
}

int test_json()
{
    // Test with relic compatibility
    if (core_init() == RLC_ERR)
    {
        core_clean();
        return RLC_ERR;
    }
    if (pc_param_set_any() != RLC_OK)
    {
        core_clean();
        return RLC_ERR;
    }
    /* Generate the secret and public key */
    g2_t pk;
    bn_t sk;
    char *pk_b64 = NULL;
    g2_null(pk);
    bn_null(sk);
    g2_new(pk);
    bn_new(sk);

    /* Generate key pair */
    int res = cp_mklhs_gen(sk, pk);
    if (res != 0)
    {
        fprintf(stderr, "Failed to generate keys\n");
        return -1;
    }
    /* Format and encode the public key */
    int pk_len = g2_size_bin(pk, 1);
    unsigned char pk_buf[pk_len];
    g2_write_bin(pk_buf, pk_len, pk, 1);
    pk_b64 = base64_encode((char *)pk_buf, pk_len);
    if (pk_b64 == NULL)
    {
        fprintf(stderr, "Failed to encode public key\n");
        return -1;
    }
    char buffer[JSON_BUFFER_SIZE];
    json_t json;
    json_init(&json, buffer, sizeof(buffer));

    json_start_object(&json);

    // Using the new helper functions with automatic commas
    json_add_key_value_string(&json, "name", "john");
    json_add_key_value_number(&json, "age", 30);

    // Add a string array
    const char *array[] = {"apple", "banana", "cherry"};
    json_add_string_array(&json, "fruits", array, 3);
    // Add a number array
    unsigned long long numbers[] = {1, 2, 3, 4, 5};
    json_add_number_array(&json, "numbers", numbers, 5);
    // Add a nested object
    json_add_key(&json, "address");
    json_start_object(&json);
    json_add_key_value_string(&json, "city", "New York");
    json_add_key_value_string(&json, "zip", "10001");
    json_end_object(&json);
    json_add_comma(&json); // Add comma after the object
    json_add_key_value_string(&json, "pk", pk_b64);
    json_end_object(&json);
    printf("Original json buffer %s\n", json.buffer);
    // Test compability with cJSON
    cJSON *json_obj = cJSON_Parse(json.buffer);
    if (json_obj == NULL)
    {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return -1;
    }
    // Print the JSON object
    char *json_str = cJSON_Print(json_obj);
    if (json_str == NULL)
    {
        printf("Error printing JSON: %s\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    printf("Parsed cJSON:\n%s\n", json_str);
    // Add to json object

    return 0;
}
// int main()
// {
//     test_json();
//     return 0;
// }