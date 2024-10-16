#ifndef CRYPTO_H
#define CRYPTO_H

#include <relic/relic.h> // Include the Relic library header
/* The headerfile for the crypto_utils*/
int relic_init();
int relic_cleanup();
int convert_to_g1(g1_t new_sig,char *decoded_sig,dig_t len);
char* base64_decode(const char* input, int length, int* decoded_length);
char* base64_encode(const char* input, int length);

#endif

