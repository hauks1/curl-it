#ifndef LOVE_H
#define LOVE_H

#include <relic/relic.h>
#include "../../utils/base64.h"

/**
 * @brief Data structure for LOVE scheme
 *
 * This structure contains the cryptographic elements used in the LOVE protocol:
 * - Random values and group elements used for encryption
 * - Computed values derived from the random elements
 * - Base64 encoded versions of all elements for network transmission
 *
 * The structure stores both the raw cryptographic elements (r, u1, u2, v2, e)
 * and their base64 encoded string representations for transmission over the network.
 *
 * @note All encoded strings must be freed when no longer needed to prevent memory leaks
 */
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

/**
 * @brief Data structure for request data for love pairing
 *
 *
 * @param r1 First challenge value
 * @param r2 Second challenge value
 * @param v1 First group element
 * @param g Group element from verifier
 * @param g2 Second group element from verifier
 * @param w2 Third group element
 * @param c Array of pairing results from first challenge
 * @param e Array of pairing results from second challenge
 */

typedef struct pairing_data
{
    /* For request */
    bn_t r1; // first challenge
    bn_t r2; // second challenge
    g1_t v1;
    g1_t g;
    g2_t g2;
    g2_t w2;
    /* Results of the pairings */
    gt_t c[2];
    gt_t e[2];

} pairing_data_t;

/**
 * @brief Initializes a love_data structure with RELIC elements and encoded fields
 *
 * This function initializes a love_data_t structure by:
 * - Checking for valid pointer
 * - Initializing RELIC elements (r, u1, u2, v2, e) using null and new functions
 * - Setting all encoded fields pointers to NULL
 *
 * @param love_data Pointer to the love_data_t structure to initialize
 * @return 0 on success, -1 if love_data pointer is NULL
 */

int init_love_data(love_data_t *love_data);
/**
 * @brief Cleans up and frees resources associated with a love_data structure
 *
 * This function frees the RELIC elements (r, u1, u2, v2, e) and their encoded string
 * representations in the love_data_t structure. It also checks for NULL pointers
 * before attempting to free resources.
 *
 * @param love_data Pointer to the love_data_t structure to clean up
 */
void cleanup_love_data(love_data_t *love_data);

/**
 * @brief Generates LOVE precomputation parameters
 *
 * This function generates the LOVE precomputation parameters using the RELIC library.
 * It computes the values of r, u1, u2, v2, and e, and encodes them in base64 format
 * for transmission. The function checks for NULL pointers and returns an error code
 * if any issues occur during the process.
 *
 * @param love_data Pointer to the love_data_t structure containing the parameters
 * @return 0 on success, -1 on failure (e.g., NULL pointer or RELIC function error)
 */
int generate_love_precomputation(love_data_t *love_data);

#endif