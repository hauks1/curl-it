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

int prepare_req_server_num(cJSON *server_obj,uint8_t *messages,int messages_len, char *sig_b64, int sig_len, char *data_set_id, char *id, char *tag,char *pk_b64){
    cJSON *messages_array = cJSON_CreateArray();
    if (messages_array == NULL) {
        fprintf(stderr, "Failed to create JSON array\n");
        return -1;
    }
    for (int i = 0; i < messages_len; i++) {
        cJSON *message_item = cJSON_CreateNumber(messages[i]);
        if (message_item == NULL) {
            fprintf(stderr, "Failed to create JSON number\n");
            cJSON_Delete(messages_array);
            return -1;
        }
        cJSON_AddItemToArray(messages_array, message_item);
    }

    if (!cJSON_AddItemToObject(server_obj, "messages", messages_array)) {
        fprintf(stderr, "Failed to add messages array to JSON object\n");
        cJSON_Delete(messages_array);
        return -1;
    }
    
    if(!cJSON_AddStringToObject(server_obj, "signature",sig_b64)){
        fprintf(stderr, "Failed to add signature to JSON object\n");
        return -1;
    }
    if(!cJSON_AddNumberToObject(server_obj, "signature_length",sig_len)){
        fprintf(stderr, "Failed to add signature length to JSON object\n");
        return -1;
    }
    if(!cJSON_AddStringToObject(server_obj, "data_set_id",data_set_id)){
        fprintf(stderr, "Failed to add data set id to JSON object\n");
        return -1;
    }
    if(!cJSON_AddStringToObject(server_obj, "id",id)){
        fprintf(stderr, "Failed to add id to JSON object\n");
        return -1;
    }
    if(!cJSON_AddStringToObject(server_obj, "tag",tag)){
        fprintf(stderr, "Failed to add tag to JSON object\n");
        return -1;
    }
    if(!cJSON_AddStringToObject(server_obj, "public_key",pk_b64)){
        fprintf(stderr, "Failed to add public key to JSON object\n");
        return -1;
    }
    return 1;
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