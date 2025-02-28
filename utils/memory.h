#ifndef MEMORY_PROFILER_H
#define MEMORY_PROFILER_H

#include "request.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cjson/cJSON.h>

/* Function wrapper declarations */
int gen_keys_wrapper(void *args);
int sign_data_points_wrapper(void *args);
int encode_signatures_wrapper(void *args);
int prepare_request_wrapper(void *args);
int prepare_raw_req_wrapper(void *args);
int curl_to_server_wrapper(void *args);

/* Main memory isolation function */
int isolate_function_memory(const char* function_name, int (*func)(void*), void *args);

/* Profiling function declarations */
void profile_gen_keys(bn_t sk, g2_t pk);
void profile_sign_data_points(message_t *message, bn_t sk, size_t num_data_points);
void profile_encode_signatures(message_t *message, unsigned char **master_sig_buf, 
                            char **master_decoded_sig_buf, size_t num_data_points);
void profile_prepare_request(cJSON *json_obj, message_t *message, char **master_decoded_sig_buf,
                          dig_t *data_points, size_t num_data_points, char *pk_b64,
                          int sig_len, uint64_t scale, const char *func_name);
void profile_prepare_raw_req(cJSON *json_obj, raw_message_t *message, dig_t *data_points,
                          size_t num_data_points, uint64_t scale);
void profile_curl_to_server(const char *url, cJSON *json_obj);


#endif 