/**
 * @file message.h
 * @brief Header file defining message structures and functions for handling data points.
 *
 * This file contains definitions for message structures used to store and transmit
 * data points along with their signatures and metadata. It includes helper functions as well.
 */
#ifndef MESSAGE_H
#define MESSAGE_H

#include <relic/relic.h>
#include "../utils/bad_string.h"

#define FUNC "doubling"
#define DEVICE_ID "12345"
#define TEST_DATABASE "test.db"
#define NUM_DATA_POINTS 30
#define NUM_MESSAGES 1
#define MAX_ID_LENGTH 37
#define MAX_TAG_LENGTH 37
#define MAX_DATA_SET_ID_LENGTH 37
#define MAX_SIGNATURE_LENGTH 128

/**
 * @brief Structure representing a message containing data points, signatures, and associated metadata
 *
 * This structure holds an array of data points along with their corresponding signatures,
 * identifiers, tags, and a dataset identifier. It is used for message transmission and
 * storage in the system.
 *
 * @note The size of arrays is determined by preprocessor constants:
 *       NUM_DATA_POINTS: Number of data points in the message
 *       MAX_ID_LENGTH: Maximum length of each ID string
 *       MAX_TAG_LENGTH: Maximum length of each tag string
 *       MAX_DATA_SET_ID_LENGTH: Maximum length of dataset ID string
 */
typedef struct message
{
    bn_t data_points[NUM_DATA_POINTS];
    g1_t sigs[NUM_DATA_POINTS];
    char ids[NUM_DATA_POINTS][MAX_ID_LENGTH];
    char tags[NUM_DATA_POINTS][MAX_TAG_LENGTH];
    char data_set_id[MAX_DATA_SET_ID_LENGTH];
} message_t;

/**
 * @brief Initializes a message structure with data points
 *
 * @param message Pointer to the message structure to initialize
 * @param data_points Array containing the data points to be stored in the message
 * @param num_data_points Number of data points in the array
 *
 * @return int Returns 0 on success, negative value on error
 */
int init_message(message_t *message, dig_t data_points[],
                 size_t num_data_points);

/**
 * @brief Cleans up and frees resources associated with a message structure
 *
 * @param message Pointer to the message structure to clean up
 * @param num_data_points Number of data points in the message to clean up
 */
void cleanup_message(message_t *message, size_t num_data_points);

/**
 * @brief Prints the contents of a message structure neatly
 *
 * @param msg Pointer to the message structure to print
 */
void print_message(message_t *msg);

/**
 * @brief Generates an array of random dig_t numbers between 2 and 40.
 *
 * @param data_points Array to store the generated random float values
 * @param num_data_points Number of random values to generate
 *
 * @return Returns 0 on successful execution
 */

int gen_dig_data_points(dig_t data_points[], size_t num_data_points);
/**
 * @brief Generates an array of random floating-point numbers between 0 and 35.0
 *
 * @param data_points Array where the generated random values will be stored
 * @param num_data_points Number of random values to generate
 *
 * @return Returns 0 on success
 */
int gen_float_data_points(double data_points[], size_t num_data_points);

#endif