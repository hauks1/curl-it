#ifndef CRYPTO_H
#define CRYPTO_H

#include <relic/relic.h> // Include the Relic library header
#include "request.h"
/* The headerfile for the crypto_utils*/
int relic_init();
int relic_cleanup();
int gen_keys(bn_t sk, g2_t pk);
int convert_to_g1(g1_t new_sig,char *decoded_sig,dig_t len);
char* base64_decode(const char* input, int length, int* decoded_length);
char* base64_encode(const char* input, int length);
int sign_data_points(message_t *message,bn_t sk, size_t num_data_points);
int encode_signatures(message_t *msg, unsigned char *master[], char *master_decoded[], int num_data_points);


#endif

