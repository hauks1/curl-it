#ifndef PARSING_H 
#define PARSING_H
#include <cjson/cJSON.h>
#include <relic/relic.h>

#define TEST_DATABASE "test.db"
#define DEVICE_ID "12345"
#define NUM_DATA_POINTS 5
#define NUM_MESSAGES 1
#define FUNC "doubling"

typedef struct message {
    bn_t data_points[NUM_DATA_POINTS];
    g1_t sigs[NUM_DATA_POINTS];
    char ids[NUM_DATA_POINTS][37];
    char tags[NUM_DATA_POINTS][37];
    char data_set_id[37];
}message_t;

/* Function to prepare the entire request */
int prepare_request_server(cJSON *json_obj,message_t *message,unsigned char *master_decoded_sig_buf[],dig_t data_points[], size_t num_data_points, char *pk_b64,int sig_len,u_int64_t scale,char *func);
/* Function to generate datapoints as digits (dig_t) */
int gen_dig_data_points(dig_t data_points[], size_t num_data_points);
/* Function to generate datapoints as floats */
int gen_float_data_points(double data_points[], size_t num_data_points);
/* Function that curls the server */
int curl_to_server(const char *url,cJSON *json);



#endif