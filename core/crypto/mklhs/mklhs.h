#ifndef MKLHS_H
#define MKLHS_H

#include <relic/relic.h>
#include "../../message/message.h"

/**
 * @brief Generates private and public key pair for MKLHS scheme
 *
 * @param sk Output secret key
 * @param pk public key
 *
 * @return int Returns 0 on success, non-zero value on failure
 *
 */
int gen_keys(bn_t sk, g2_t pk);

/**
 * @brief Signs multiple data points in a message using the secret key
 *
 * @param message Pointer to the message structure containing data points to be signed
 * @param sk Secret key used for signing the data points
 * @param num_data_points Number of data points to be signed
 *
 * @return int Returns 0 on success, non-zero value on failure
 */
int sign_data_points(message_t *message, bn_t sk, size_t num_data_points);

#endif