#ifndef PARSING_H 
#define PARSING_H
#include <cjson/cJSON.h>
#include <relic/relic.h>

#define TEST_DATABASE "test.db"
#define DEVICE_ID "12345"
#define NUM_DATA_POINTS 30
#define NUM_MESSAGES 1
#define FUNC "doubling"

/* Regular message containing signatures */
typedef struct message {
    bn_t data_points[NUM_DATA_POINTS];
    g1_t sigs[NUM_DATA_POINTS];
    char ids[NUM_DATA_POINTS][37];
    char tags[NUM_DATA_POINTS][37];
    char data_set_id[37];
}message_t;
/* A raw message not containing signatures (for baseline purposes) */
typedef struct raw_message{
    bn_t data_points[NUM_DATA_POINTS];
    char ids[NUM_DATA_POINTS][37];
    char tags[NUM_DATA_POINTS][37];
    char data_set_id[37];
}raw_message_t;
#include "crypto.h"

/* Function that prepares request wihtout the signature and corresponding metadata */
int prepare_raw_req_server(cJSON *json_obj,raw_message_t *message,dig_t data_points[], size_t num_data_points, dig_t scale);
/* Function to prepare the entire request */
int prepare_request_server(cJSON *json_obj,message_t *message,unsigned char *master_decoded_sig_buf[],dig_t data_points[], size_t num_data_points, char *pk_b64,int sig_len,u_int64_t scale,char *func);
/* Adds the LOVE data to the request */
int add_love_data_json(cJSON *json,love_data_t *data);
/* Function to generate datapoints as digits (dig_t) */
int gen_dig_data_points(dig_t data_points[], size_t num_data_points);
/* Function to generate datapoints as floats */
int gen_float_data_points(double data_points[], size_t num_data_points);
/* Function that curls the server */
int curl_to_server(const char *url,cJSON *json);
/* Function to ask for pairings */




#endif