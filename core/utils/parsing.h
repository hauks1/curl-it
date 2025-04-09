#ifndef PARSING_H
#define PARSING_H

#define NUM_DATAPOINTS 30
#define NUM_MESSAGES 1
#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include "relic/relic.h"
#include "../crypto/love/love.h"
#include "../crypto/verify/verify.h"

typedef struct crypto_data crypto_data_t;
/* Struct to hold intermediate values after parsing the request */
typedef struct parse_data
{
  // labels
  const char *data_set_id;
  const char *ids[NUM_DATAPOINTS];
  const char *tags[NUM_DATAPOINTS];
  // result
  char *t;
  // signature
  char *eval_sig;
  // public key
  char *public_key;
  // coefficients
  char *coefs;
  int num_data_points;
} parse_t;

/**
 * @brief Converts a binary signature to a G1 point
 *
 * This function takes a binary signature and converts it to an elliptic curve point
 * on the G1 group. It initializes a new G1 point and reads the binary signature into it.
 *
 * @param new_sig    Output parameter for the converted G1 signature point
 * @param decoded_sig Input binary signature to convert
 * @param len        Length of the input binary signature
 *
 * @return RLC_OK on success, or an error code if signature conversion fails
 *
 * @throws ERR_CAUGHT if there is an error during binary to G1 conversion
 */
int convert_to_g1(g1_t new_sig, char *decoded_sig, dig_t len);

/**
 * @brief Retrieves and parses JSON data into parse_data and love_data structures
 *
 * This function extracts metadata and LOVE protocol data from a JSON object and populates
 * the corresponding structures. It handles parsing of dataset information, IDs, tags,
 * cryptographic parameters and LOVE protocol parameters.
 *
 * @param parse_data Pointer to parse_t structure to store general parsed data
 * @param love_data Pointer to love_data_t structure to store LOVE protocol data
 * @param json_data Pointer to cJSON object containing the data to parse
 *
 * @return 0 on success, -1 on failure with error message printed to stderr
 *
 * The function extracts the following data:
 * - Dataset ID and metadata
 * - IDs and tags for data points
 * - Cryptographic parameters (mu, eval_sig, public key, coefficients)
 * - LOVE protocol parameters (r, u1, u2, v2, e)
 *
 * Each missing required field will result in an error and function termination
 */
int retrieve_json_data(parse_t *parse_data, love_data_t *love_data, cJSON *json_data);

/**
 * @brief Decodes base64-encoded cryptographic data and LOVE protocol data into their respective data structures
 *
 * This function takes base64 encoded data from parsed_data and love_data structs and decodes them into
 * their corresponding binary formats stored in crypto_data and love_data structs. The following elements
 * are decoded:
 *
 * From parsed_data:
 * - Public key
 * - Evaluated signature
 * - t value
 * - Coefficients
 *
 * From love_data:
 * - r (randomness)
 * - u1 (G1 element)
 * - u2 (G2 element)
 * - v2 (G2 element)
 * - e (GT element)
 *
 * @param crypto_data Pointer to crypto_data_t struct where decoded cryptographic elements will be stored
 * @param parsed_data Pointer to parse_t struct containing the base64 encoded input data
 * @param love_data Pointer to love_data_t struct containing both encoded and decoded LOVE protocol data
 *
 * @return 0 on successful decoding of all elements
 * @return -1 if any decoding operation fails
 *
 * @note This function allocates memory for temporary buffers during base64 decoding,
 *       but frees them before returning
 */
int decode_parsed_data(crypto_data_t *crypto_data, parse_t *parsed_data, love_data_t *love_data);

/**
 * @brief Parses server response data into pairing data structure
 *
 * This function takes a raw server response and parses it into the pairing_data_t structure.
 * The response contains serialized data for c[0], c[1], e[0], and e[1] elements, each preceded
 * by their size in bytes as uint32_t.
 *
 * @param pairing_data Pointer to the pairing_data_t structure to store the parsed data
 * @param response Pointer to the raw response data buffer
 * @param response_len Length of the response data buffer in bytes
 *
 * @return 0 on successful parsing, -1 on error
 *
 * The expected response format is:
 * - uint32_t size of c[0]
 * - c[0] data
 * - uint32_t size of c[1]
 * - c[1] data
 * - uint32_t size of e[0]
 * - e[0] data
 * - uint32_t size of e[1]
 * - e[1] data
 *
 * @note All elements are read using gt_read_bin() function
 */
int parse_server_response(pairing_data_t *pairing_data, const char *response, size_t response_len);

int socket_parse_server_response(pairing_data_t *pairing_data, const char *response, size_t response_len);

/**
 * @brief Initializes a parse_t structure by setting all its pointers to NULL
 *
 * This function takes a parse_t structure and initializes all its member pointers
 * to NULL. It also sets the number of data points to 0. This ensures the structure
 * starts in a known clean state before being populated with actual data.
 *
 * @param parse_data Pointer to the parse_t structure to be initialized
 *
 * Fields initialized:
 * - data_set_id: Set to NULL
 * - ids[]: Array of pointers all set to NULL
 * - tags[]: Array of pointers all set to NULL
 * - t: Set to NULL
 * - eval_sig: Set to NULL
 * - public_key: Set to NULL
 * - coefs: Set to NULL
 * - num_data_points: Set to 0
 */
void init_parsed_data(parse_t *parse_data);

/**
 * @brief Prints all fields of a parse_t structure to stdout
 *
 * This function prints the contents of a parse_t structure in a formatted way,
 * displaying the data set ID, number of data points, tags for each data point,
 * T value, evaluation signature, public key and coefficients.
 *
 * @param parse_data Pointer to the parse_t structure containing the data to be printed
 *
 * Example output:
 * Parsed data:
 * Data set id: <id>
 * Number of data points: <n>
 * Tag: <tag1>
 * Tag: <tag2>
 * ...
 * T: <t_value>
 * Eval sig: <signature>
 * Public key: <key>
 * Coefficients: <coefs>
 */
void print_parsed_data(parse_t *parse_data);

#endif