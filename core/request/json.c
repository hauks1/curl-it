#include "json.h"

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
