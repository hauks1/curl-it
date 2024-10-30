#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <curl/curl.h>
#include <cjson/cJSON.h> 
#include <time.h>
#include <math.h>
#include "parsing.h"

int prepare_request_server(cJSON *json_obj,message_t *message,unsigned char *master_decoded_sig_buf[],dig_t data_points[], size_t num_data_points, char *pk_b64,int sig_len,u_int64_t scale){
    // JSON setup and sending
    cJSON *signatures = cJSON_CreateArray();
    cJSON *messages = cJSON_CreateArray();
    cJSON *tags = cJSON_CreateArray();
    cJSON *data_set_id = cJSON_CreateString(TEST_DATABASE);
    cJSON *public_key = cJSON_CreateString(pk_b64);
    cJSON *scale_json = cJSON_CreateNumber(scale);
    for(size_t i=0; i < num_data_points; i++){
        cJSON *sig = cJSON_CreateString(master_decoded_sig_buf[i]);
        cJSON_AddItemToArray(signatures, sig);
        cJSON *msg = cJSON_CreateNumber(data_points[i]);
        cJSON_AddItemToArray(messages, msg);
        cJSON *tag = cJSON_CreateString(message->tags[i]);
        cJSON_AddItemToArray(tags, tag);
    }
    cJSON *id = cJSON_CreateString(message->ids[0]);
    cJSON_AddItemToObject(json_obj, "id",id);
    cJSON_AddItemToObject(json_obj, "datapoints", messages);
    if (messages == NULL) {
        fprintf(stderr, "Failed to create JSON array for messages\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddItemToObject(json_obj, "signatures", signatures);
    if (signatures == NULL) {
        fprintf(stderr, "Failed to create JSON array for signatures\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddNumberToObject(json_obj, "signature_length", sig_len);
    if (id == NULL) {
        fprintf(stderr, "Failed to create JSON array for ids\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddItemToObject(json_obj, "tags", tags);
    if (tags == NULL) {
        fprintf(stderr, "Failed to create JSON array for tags\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddItemToObject(json_obj, "data_set_id", data_set_id);
    if (data_set_id == NULL) {
        fprintf(stderr, "Failed to create JSON string for data_set_id\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddItemToObject(json_obj, "public_key", public_key);
    if (public_key == NULL) {
        fprintf(stderr, "Failed to create JSON string for public_key\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    cJSON_AddItemToObject(json_obj, "scale", scale_json);
    if (scale_json == NULL) {
        fprintf(stderr, "Failed to create JSON number for scale\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    return 0;
}
/* Function to generate some random data, REMEMEBR TO FREE THE MESSAGES and LEN */
int gen_data(uint8_t *messages,int num_messages){
    messages = (uint8_t *)malloc(sizeof(uint8_t)*num_messages);
    if(messages == NULL){
        fprintf(stderr,"Could not allocate messages\n");
        return -1;
    }
    srand(time(NULL)); // init 
    for(int i = 0; i < num_messages; i++){
        uint8_t random_num = rand();
        messages[i] = random_num; // Does not matter if some or all of the numbers are the same
    }
    return 0;
}