// request.h - Add this file if it doesn't exist or update it

#ifndef REQUEST_H
#define REQUEST_H

#include "json.h"
#include "../message/message.h"
#include "../crypto/love/love.h"

// Define TEST_DATABASE if not already defined
#ifndef TEST_DATABASE
#define TEST_DATABASE "test.db"
#endif

// /**
//  * @brief Add LOVE data to a cJSON object
//  *
//  * @param json JSON object to add data to
//  * @param data LOVE data structure
//  * @return 0 on success, -1 on error
//  */
// int add_love_data_json(cJSON *json, love_data_t *data);

/**
 * @brief Add LOVE data to a custom JSON structure
 *
 * @param json Custom JSON structure to add data to
 * @param data LOVE data structure
 * @return 0 on success, -1 on error
 */
int add_love_data_json_alt(json_t *json, love_data_t *data);

/**
//  * @brief Prepare a request using cJSON library
//  *
//  * @param json_obj cJSON object to fill
//  * @param message Message structure containing IDs and tags
//  * @param master_decoded_sig_buf Array of base64-encoded signatures
//  * @param data_points Array of data points
//  * @param num_data_points Number of data points
//  * @param pk_b64 Base64-encoded public key
//  * @param sig_len Signature length
//  * @param scale Scale factor
//  * @param func Function name
//  * @return 0 on success, -1 on error
//  */
// int prepare_request_server(cJSON *json_obj, message_t *message,
//                            unsigned char *master_decoded_sig_buf[],
//                            dig_t data_points[], size_t num_data_points,
//                            char *pk_b64, int sig_len, uint64_t scale, char *func);

/**
 * @brief Prepare a request using custom JSON implementation
 *
 * @param json Custom JSON structure to fill
 * @param message Message structure containing IDs and tags
 * @param master_decoded_sig_buf Array of base64-encoded signatures
 * @param data_points Array of data points
 * @param num_data_points Number of data points
 * @param pk_b64 Base64-encoded public key
 * @param sig_len Signature length
 * @param scale Scale factor
 * @param func Function name
 * @param love_data LOVE data structure (can be NULL)
 * @return 0 on success, -1 on error
 */
int prepare_req_server(json_t *json, message_t *message, char *master_decoded_sig_buf[],
                       dig_t data_points[], size_t num_data_points,
                       char *pk_b64, int sig_len, uint64_t scale, char *func,
                       love_data_t *love_data);

#endif /* REQUEST_H */