#include "crypto.h"
#include "relic/relic.h"
#include <b64/cencode.h>
#include <b64/cdecode.h>
#include "request.h"
/**
 * @file crypto_utils.c
 * @brief Utility functions for cryptographic operations and wrappers for different relic based operations.
 *
 * This file contains utility functions that the handlers use to do cryptographic functions.
 */


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
int relic_init() {
    if (core_init() == RLC_ERR) {
        core_clean();
        return RLC_ERR;
    }
    if (pc_param_set_any() != RLC_OK) {
        core_clean();
        return RLC_ERR;
    }
    return RLC_OK;
}
/**
 * @brief Cleans up the cryptographic core.
 *
 * This function performs cleanup operations for the cryptographic core by
 * calling the `core_clean()` function. It ensures that all resources
 * allocated by the cryptographic core are properly released.
 *
 * @return RLC_OK on successful cleanup.
 */

int relic_cleanup() {
    core_clean();
    return RLC_OK;
}
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
 * 
 * @return RLC_OK on success, or an error code if an exception is caught.
 */

int convert_to_g1(g1_t new_sig, char *decoded_sig,dig_t len){
    // Initialize the signature
    g1_null(new_sig);
    g1_new(new_sig);
    // Read the signature into the g1_t
    RLC_TRY {
        g1_read_bin(new_sig, (unsigned char *)decoded_sig, len);
    } RLC_CATCH_ANY {
        RLC_THROW(ERR_CAUGHT);
    }
    return RLC_OK;
} 

/* B64 decode and encode */
char* base64_decode(const char* input, int length, int* decoded_length) {
    base64_decodestate state;
    base64_init_decodestate(&state);

    char* decoded = (char*)malloc(length * 3 / 4 + 1); // Allocate enough space for decoded data
    *decoded_length = base64_decode_block(input, length, decoded, &state);
    decoded[*decoded_length] = '\0'; // Null-terminate the decoded string

    return decoded;
}

char* base64_encode(const char* input, int length) {
    base64_encodestate state;
    base64_init_encodestate(&state);

    int encoded_length = 4 * ((length + 2) / 3); // Base64 encoded length
    char* encoded = (char*)malloc(encoded_length + 1); // +1 for null terminator
    int len = base64_encode_block(input, length, encoded, &state);
    len += base64_encode_blockend(encoded + len, &state);
    encoded[len] = '\0'; // Null-terminate the encoded string

    return encoded;
}

int gen_keys(bn_t sk, g2_t pk){
    bn_null(sk);
    bn_new(sk);
    g2_null(pk);
    g2_new(pk);
    
    int res = cp_mklhs_gen(sk,pk);
    if(res != RLC_OK){
        return -1;
    }
    return 0;
}

int sign_data_points(message_t *message,bn_t sk, size_t num_data_points){
    /* Sign all the datapoints */
    for (int i = 0; i < num_data_points; i++) {
        int res_sign = cp_mklhs_sig(message->sigs[i], message->data_points[i], message->data_set_id, message->ids[0], message->tags[i], sk);
        if (res_sign != RLC_OK) {
            fprintf(stderr, "Could not sign message\n");
            return -1;
        }
    }
    return 0;

}
int encode_signatures(message_t *msg, unsigned char *master[], char *master_decoded[], int num_data_points){
    int sig_len = g1_size_bin(msg->sigs[0], 1);
    /* Convert the signature and encode signature */
    for (size_t i = 0; i < num_data_points; i++) {

        // Allocate memory for the signature buffer
        master[i] = (unsigned char*)malloc(sig_len * sizeof(unsigned char));
        if (master[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for master_sig_buf[%zu]\n", i);
            return -1;
        }

        g1_write_bin(master[i], sig_len, msg->sigs[i], 1); // Write the signature to the byte array
        // Encode the signature
        master_decoded[i] = base64_encode((char *)master[i], sig_len);
        if (master_decoded[i] == NULL) {
            fprintf(stderr, "Failed to encode signature %zu\n", i);
            return -1;
        }
    }
    return 0;
}


