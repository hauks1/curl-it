#ifndef CRYPTO_H
#define CRYPTO_H
#include <relic/relic.h> // Include the Relic library header

/* Structure to hold LOVE precomputation data */
typedef struct love_data
{
    bn_t r;  // Random scalar for LOVE
    g1_t u1; // Random G1 element
    g2_t u2; // Random G2 element
    g2_t v2; // Computed G2 element: v2 = (r^-1 mod q) * u2
    gt_t e;  // Pairing result: e = e(u1, u2)
    
    // Base64 encoded versions for transmission
    char *r_encoded;  // Base64 encoded r
    char *u1_encoded; // Base64 encoded u1
    char *u2_encoded; // Base64 encoded u2
    char *v2_encoded; // Base64 encoded v2
    char *e_encoded;  // Base64 encoded e
} love_data_t;

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

