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
#include "request.h"

int prepare_request_server(cJSON *json_obj,message_t *message,unsigned char *master_decoded_sig_buf[],dig_t data_points[], size_t num_data_points, char *pk_b64,int sig_len,u_int64_t scale,char *func){
    // JSON setup and sending
    cJSON *signatures = cJSON_CreateArray();
    cJSON *messages = cJSON_CreateArray();
    cJSON *tags = cJSON_CreateArray();
    cJSON *data_set_id = cJSON_CreateString(TEST_DATABASE);
    cJSON *public_key = cJSON_CreateString(pk_b64);
    cJSON *scale_json = cJSON_CreateNumber(scale);
    cJSON *func_json = cJSON_CreateString(func);

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
    cJSON_AddItemToObject(json_obj, "function", func_json);
    if (func_json == NULL) {
        fprintf(stderr, "Failed to create JSON string for function\n");
        cJSON_Delete(json_obj);
        return -1;
    }
    return 0;
}
int gen_dig_data_points(dig_t data_points[], size_t num_data_points){
    srand(time(NULL)); 
    int range = 40;
    int min = 0;
    for(int i = 0; i < num_data_points; i++){
        data_points[i] = (dig_t)(rand() % range + min);
    }
    return 0;
}
int gen_float_data_points(double data_points[], size_t num_data_points){
    srand(time(NULL)); 
    float a = 35.0;
    // Generate random float data points from 0 to 35 
    for(int i = 0; i < num_data_points; i++){
        data_points[i] = ((double)rand() / (double)RAND_MAX) * a;
    }
    return 0;
}
int curl_to_server(const char *url,cJSON *json){
    CURL *curl_server;
    CURLcode res_server;

    curl_server = curl_easy_init();
    if(curl_server) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
     
        char *json_str = cJSON_Print(json);
        // Specify the URL
        curl_easy_setopt(curl_server, CURLOPT_URL, url);
        // Specify the data to be sent
        curl_easy_setopt(curl_server, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl_server, CURLOPT_POSTFIELDS, json_str);
        // Perform the request
        res_server = curl_easy_perform(curl_server);
        if(res_server != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res_server));
            return -1;
        }
        curl_easy_cleanup(curl_server);
        cJSON_free(json_str); 
        return 0;
    }
    return -1;
}
