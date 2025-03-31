/** @brief   */
#ifndef SEND_H
#define SEND_H
#include <cjson/cJSON.h>

/**
 * @brief Sends JSON data to a specified server URL using libcurl
 *
 * This function initializes a curl session and sends a POST request with JSON data
 * to the specified URL. It handles the HTTP headers, data serialization, and cleanup.
 *
 * @param url The destination URL where the JSON data will be sent
 * @param json A pointer to a cJSON object containing the data to be sent
 *
 * @return Returns 0 on successful transmission, -1 on failure
 *         (e.g., curl initialization failure or transmission error)
 * 
 * @note The function automatically handles memory management for curl resources
 *       and JSON string conversion
 */
int curl_to_server(const char *url, cJSON *json);
#endif