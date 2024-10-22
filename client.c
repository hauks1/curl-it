#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <curl/curl.h>
#include <cjson/cJSON.h> 
#include <uuid/uuid.h>
#include "relic/relic.h"
#include "utils/crypto.h"
#include "utils/parsing.h"


/* Function to initialize the messages */
int init_message(message_t *message, dig_t data_points[], size_t num_data_points) {
    if (message == NULL) {
        fprintf(stderr, "Message pointer is NULL\n");
        return -1;
    }

    for (int i = 0; i < num_data_points; i++) {
        bn_null(message->data_point[i]);
        bn_new(message->data_point[i]);
        bn_set_dig(message->data_point[i], data_points[i]);

        // Initialize signature
        g1_null(message->sigs[i]);
        g1_new(message->sigs[i]);

        // Create random UUIDs
        uuid_t uuid, date;
        uuid_generate(uuid);
        uuid_generate_time(date);
        char uuid_str[37], date_str[37];
        uuid_unparse(uuid, uuid_str);
        uuid_unparse(date, date_str);

        strncpy(message->ids[i], uuid_str, sizeof(message->ids[i]));
        strncpy(message->tags[i], date_str, sizeof(message->tags[i]));
    }

    // Set data_set_id
    strncpy(message->data_set_id, TEST_DATABASE, sizeof(message->data_set_id));

    return 0;
}
/* Function to neatly print a message struct */
void print_message(message_t *msg) {
    for (int i = 0; i < NUM_DATA_POINTS; i++) {
        printf("Data point: %d\n", i);
        bn_print(msg->data_point[i]);
        printf("Signature: %d\n", i);
        g1_print(msg->sigs[i]);
        printf("ID: %s\n", msg->ids[i]);
        printf("Tag: %s\n", msg->tags[i]);
    }
    printf("Data set id: %s\n", msg->data_set_id);
} 


void curl_to_server(const char *url,cJSON *json){
    CURL *curl_server;
    CURLcode res_server;

    curl_server = curl_easy_init();
    if(curl_server) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
     
        printf("%s\n",cJSON_Print(json));
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
        }
        curl_easy_cleanup(curl_server);
        cJSON_free(json_str); 
    }
}


/* MAIN */
int main(int argc, char *argv[]){
    /* Initialize the RELIC library */ 
    relic_init();

    /* Generate the secret and public key */
    g2_t pk;
    bn_t sk;

    /* Generate key pair */
    int res = gen_keys(sk,pk);
    if(res != 0){
        fprintf(stderr,"Failed to generate keys\n");
        return -1;
    }
    printf("PUBLIC KEY:\n");
    g2_print(pk);
    printf("SECRET KEY:\n");
    bn_print(sk);

    /* Format and encode the public key  */ 
    int pk_len = g2_size_bin(pk,1);
    unsigned char pk_buf[pk_len];
    g2_write_bin(pk_buf, pk_len, pk, 1); // Write the public key to the byte array  
    char* pk_b64 = base64_encode((char*)pk_buf, pk_len);

    /* Generate ONE message from random datapoints */
    message_t *message = (message_t *)malloc(sizeof(message_t));
    if (message == NULL){
        fprintf(stderr,"Could not allocate message\n");
        return -1;
    }
    dig_t data_points[7] = {2,2,2,2,2,2,2};
    size_t num_data_points = NUM_DATA_POINTS;
    int init_res = init_message(message, data_points, num_data_points);
    if (init_res != 0) {
        fprintf(stderr, "Failed to initialize message\n");
        return -1;
    }
    print_message(message);
    while(1){

        int sign_res = sign_data_points(message,sk,NUM_DATA_POINTS);
        if(sign_res != 0){
            fprintf(stderr,"Failed to sign data points\n");
            return -1;
        }
        
        // Array that holds all the signatures
        unsigned char *master_sig_buf[NUM_DATA_POINTS];
        char *master_decoded_sig_buf[NUM_DATA_POINTS];

        int sig_len = g1_size_bin(message->sigs[0], 1);
        int encode_res = encode_signatures(message, master_sig_buf, master_decoded_sig_buf, num_data_points);
        if (encode_res != 0) {
            fprintf(stderr, "Failed to encode signatures\n");
            return -1;
        }  
        /* Commence curling to the server */ 
        cJSON *json_obj = cJSON_CreateObject();
        int prepare = prepare_request_server(json_obj,message,master_decoded_sig_buf,data_points,num_data_points,pk_b64,sig_len);
        if(prepare != 0){
            fprintf(stderr,"Failed to prepare request\n");
            return -1;
        }
        curl_to_server("http://localhost:12345/new",json_obj);
        cJSON_Delete(json_obj); 
        sleep(5);
    }

    
  
    // Clean up the RELIC library
    relic_cleanup();
    return 0;
}
