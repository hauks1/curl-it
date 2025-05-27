/**
 * @file json.h
 * @brief Lightweight JSON serialization.
 *
 * This library provides a set of functions to create JSON objects and arrays
 * with minimal memory allocation, suitable for embedded systems and IoT devices.
 */

#ifndef JSON_H
#define JSON_H

#define JSON_BUFFER_SIZE 4096
#include <stddef.h>
#include "relic/relic.h"
#include "../utils/bad_string.h"

/**
 * @struct json_t
 * @brief Structure for JSON serialization using a pre-allocated buffer.
 */
typedef struct json
{
    char *buffer;    /**< Pre-allocated buffer to store the JSON string */
    size_t capacity; /**< Total capacity of the buffer */
    size_t pos;      /**< Current position in the buffer */
    int error;       /**< Error flag (0 for success, -1 for error) */
} json_t;

/**
 * @brief Initialize a JSON serializer with a pre-allocated buffer.
 *
 * @param json Pointer to a json_t structure to initialize
 * @param buffer Pre-allocated buffer to store the JSON string
 * @param capacity Size of the pre-allocated buffer
 */
void json_init(json_t *json, char *buffer, size_t capacity);

/**
 * @brief Check if there's enough capacity in the buffer.
 *
 * @param json Pointer to a json_t structure
 * @param needed Number of bytes needed for the next operation
 * @return 0 on success, -1 if buffer capacity would be exceeded
 */
int json_check_capacity(json_t *json, size_t needed);

/**
 * @brief Write a string to the JSON buffer.
 *
 * @param json Pointer to a json_t structure
 * @param str String to write to the buffer
 * @return 0 on success, -1 on error
 */
int json_write(json_t *json, const char *str);

/**
 * @brief Add a comma to the JSON buffer.
 *
 * @param json Pointer to a json_t structure
 * @return 0 on success, -1 on error
 */
int json_add_comma(json_t *json);

/**
 * @brief Start a JSON object by writing '{'.
 *
 * @param json Pointer to a json_t structure
 * @return 0 on success, -1 on error
 */
int json_start_object(json_t *json);

/**
 * @brief End a JSON object by writing '}' and removing trailing comma if present.
 *
 * @param json Pointer to a json_t structure
 * @return 0 on success, -1 on error
 */
int json_end_object(json_t *json);

/**
 * @brief Start a JSON array by writing '['.
 *
 * @param json Pointer to a json_t structure
 * @return 0 on success, -1 on error
 */
int json_start_array(json_t *json);

/**
 * @brief End a JSON array by writing ']' and removing trailing comma if present.
 *
 * @param json Pointer to a json_t structure
 * @return 0 on success, -1 on error
 */
int json_end_array(json_t *json);

/**
 * @brief Add a key to a JSON object.
 *
 * @param json Pointer to a json_t structure
 * @param key Key name to add
 * @return 0 on success, -1 on error
 */
int json_add_key(json_t *json, const char *key);

/**
 * @brief Add a string value to the JSON buffer (with quotes).
 *
 * @param json Pointer to a json_t structure
 * @param str String value to add
 * @return 0 on success, -1 on error
 */
int json_add_string(json_t *json, const char *str);

/**
 * @brief Add a number value to the JSON buffer.
 *
 * @param json Pointer to a json_t structure
 * @param number Number value to add
 * @return 0 on success, -1 on error
 */
int json_add_number(json_t *json, unsigned long long number);

/**
 * @brief Add a key-value pair with string value and trailing comma.
 *
 * @param json Pointer to a json_t structure
 * @param key Key name to add
 * @param value String value to add
 * @return 0 on success, -1 on error
 */
int json_add_key_value_string(json_t *json, const char *key, const char *value);

/**
 * @brief Add a key-value pair with number value and trailing comma.
 *
 * @param json Pointer to a json_t structure
 * @param key Key name to add
 * @param value Number value to add
 * @return 0 on success, -1 on error
 */
int json_add_key_value_number(json_t *json, const char *key, unsigned long long value);

/**
 * @brief Add a string array to the JSON buffer.
 *
 * @param json Pointer to a json_t structure
 * @param key Key name for the array
 * @param vals Array of string values
 * @param count Number of elements in the array
 * @return 0 on success, -1 on error
 */
int json_add_string_array(json_t *json, const char *key, const char **vals, int count);

/**
 * @brief Add a number array to the JSON buffer.
 *
 * @param json Pointer to a json_t structure
 * @param key Key name for the array
 * @param vals Array of number values
 * @param count Number of elements in the array
 * @return 0 on success, -1 on error
 */
int json_add_number_array(json_t *json, const char *key, const unsigned long long *vals, int count);

/**
 * @brief Test function to verify JSON serialization functionality.
 *
 * @return 0 on success, -1 on error
 */
int test_json();

#endif /* JSON_H */