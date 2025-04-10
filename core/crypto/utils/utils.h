/**
 * @file crypto_utils.h
 * @brief This file contains utility functions for the core cryptographic functions.
 */

#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <relic/relic.h>
#include "../../message/message.h"

extern char base64_enctable[];
// extern char base64_dectable[256];
void base64_build_dectable();
size_t base64_out_len(size_t in_len);
char *base64_enc(char *data, size_t input_length, size_t *output_length);
// char *base64_dec(char *data, size_t input_length, size_t *output_length);

/**
 * @brief Initializes the RELIC library core and sets pairing parameters.
 *
 * This function initializes the core components of the RELIC library and sets
 * the pairing parameters to any available configuration. If the core
 * initialization fails or if setting the pairing parameters fails, the
 * function cleans up the core components and returns an error code.
 *
 * @return RLC_OK if initialization and parameter setting are successful,
 *         RLC_ERR otherwise.
 */
int relic_init();

/**
 * @brief Cleans up the cryptographic core.
 *
 * This function performs cleanup operations for the cryptographic core by
 * calling the `core_clean()` function. It ensures that all resources
 * allocated by the cryptographic core are properly released.
 *
 * @return RLC_OK on successful cleanup.
 */
int relic_cleanup();

/**
 * @brief Converts a decoded signature into a g1_t type.
 * @note Rememeber to that the caller is responsible for freeing g1_t
 *
 * This function initializes a g1_t type variable and reads a decoded
 * signature into it. It uses the Relic library's functions to handle
 * the g1_t type and to read the binary data of the signature.
 *
 * @param new_sig The g1_t type variable where the decoded signature will be stored.
 * @param decoded_sig The decoded signature in a character array format.
 * @param len The length of the decoded signature.
 * @return RLC_OK on success, or an error code if an exception is caught.
 */
int convert_to_g1(g1_t new_sig, char *decoded_sig, dig_t len);

/**
 * @brief Decodes a base64 encoded string into its original form
 * @note The returned buffer needs to be freed by the caller to avoid memory leaks
 *
 * @param input The base64 encoded string to decode
 * @param length The length of the input string
 * @param decoded_length Pointer to store the length of the decoded output
 *
 * @return char* pointer to the decoded string (dynamically allocated, caller must free)
 * or NULL if decoding fails
 */
char *base64_decode(const char *input, int length, int *decoded_length);

/**
 * @brief Encodes a string into base64 format
 * @note The returned buffer needs to be freed by the caller to avoid memory leaks
 *
 * @param input The string to encode
 * @param length The length of the input string
 *
 * @return char* pointer to the base64 encoded string (dynamically allocated, caller must free)
 * or NULL if encoding fails
 */
char *base64_encode(const char *input, int length);

/**
 * @brief Encodes digital signatures into base64 format.
 *
 * @param msg Pointer to the message structure containing the signatures
 * @param master Array of pointers to store binary signatures
 * @param master_decoded Array of pointers to store base64 encoded signatures
 * @param num_data_points Number of signatures to process
 *
 * @return 0 on success, -1 on memory allocation or encoding failure.
 */
int encode_signatures(message_t *msg, unsigned char *master[], char *master_decoded[], int num_data_points);

#endif