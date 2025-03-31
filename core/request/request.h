#ifndef REQUEST_H
#define REQUEST_H
#include <cjson/cJSON.h>
#include "../message/message.h"
#include "../crypto/love/love.h"
/**
 * @brief Prepares a JSON request object for server communication containing data points, signatures, and metadata
 *
 * @param json_obj The JSON object to populate with request data
 * @param message Message structure containing tags and IDs
 * @param master_decoded_sig_buf Array of decoded signatures
 * @param data_points Array of data points to be sent
 * @param num_data_points Number of data points in the arrays
 * @param pk_b64 Base64 encoded public key string
 * @param sig_len Length of signatures
 * @param scale Scale factor for data processing
 * @param func Function name/identifier string
 *
 * @return 0 on success, -1 on failure
 *
 * @details This function creates a structured JSON request containing:
 *          - Data points array
 *          - Corresponding signatures array
 *          - Tags for each data point
 *          - Dataset identifier
 *          - Public key
 *          - Scale factor
 *          - Function identifier
 *          Each component is validated during creation and the function will
 *          return an error if any JSON object creation fails.
 */
int prepare_request_server(cJSON *json_obj, message_t *message, unsigned char *master_decoded_sig_buf[], dig_t data_points[], size_t num_data_points, char *pk_b64, int sig_len, uint64_t scale, char *func);

/**
 * @brief Adds LOVE (Level Of VErification) data to a JSON object.
 *
 * This function takes a cJSON object and a love_data_t structure containing encoded
 * LOVE parameters and adds them to the JSON object as string key-value pairs.
 * The following LOVE parameters are added:
 * - love_r
 * - love_u1
 * - love_u2
 * - love_v2
 * - love_e
 *
 * @param json Pointer to the cJSON object to add data to
 * @param data Pointer to the love_data_t structure containing encoded LOVE parameters
 *
 * @return 0 on success, -1 on failure (NULL pointers or JSON add failures)
 */
int add_love_data_json(cJSON *json, love_data_t *data);

#endif