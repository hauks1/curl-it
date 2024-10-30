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
        bn_new(message->data_points[i]);
        bn_set_dig(message->data_points[i], data_points[i]);

        // Initialize signature
        g1_null(message->sigs[i]);
        g1_new(message->sigs[i]);

        // Create random tag's
        uuid_t  date;
        uuid_generate_time(date);
        char uuid_str[37], date_str[37];
        uuid_unparse(date, date_str);

        
        strncpy(message->tags[i], date_str, sizeof(message->tags[i]));
    }
    uuid_t uuid;
    uuid_generate(uuid);
    char uuid_str[37];
    uuid_unparse(uuid, uuid_str);
    strncpy(message->ids[0], uuid_str, sizeof(message->data_set_id));
    // Set data_set_id
    strncpy(message->data_set_id, TEST_DATABASE, sizeof(message->data_set_id));

    return 0;
}
/* Function to neatly print a message struct */
void print_message(message_t *msg) {
    for (int i = 0; i < NUM_DATA_POINTS; i++) {
        printf("Data point: %d\n", i);
        bn_print(msg->data_points[i]);
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

int gen_dig_data_points(dig_t data_points[], size_t num_data_points){
    srand(time(NULL)); 
    int range = 2000;
    int min = 1000;
    for(int i = 0; i < num_data_points; i++){
        data_points[i] = (dig_t)(rand() % range + min);
    }
    return 0;
}
int gen_float_data_points(double data_points[], size_t num_data_points){
    srand(time(NULL)); 
    float a = 2.0;
    for(int i = 0; i < num_data_points; i++){
        data_points[i] = ((float)rand()/(float)(RAND_MAX)) * a;
    }
    return 0;
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
 
    /* Format and encode the public key  */ 
    int pk_len = g2_size_bin(pk,1);
    unsigned char pk_buf[pk_len];
    g2_write_bin(pk_buf, pk_len, pk, 1); // Write the public key to the byte array  
    char* pk_b64 = base64_encode((char*)pk_buf, pk_len);

    /* Allocate ONE message from random datapoints */
    message_t *message = (message_t *)malloc(sizeof(message_t));
    if (message == NULL){
        fprintf(stderr,"Could not allocate message\n");
        return -1;
    }
    /* Allocate the data points */
    dig_t *data_points = (dig_t *)malloc(sizeof(dig_t)*NUM_DATA_POINTS);
    if (data_points == NULL){
        fprintf(stderr,"Could not allocate data points\n");
        return -1;
    }
    uint64_t scale = 1;
    if (strcmp(argv[1],"float") == 0){
        scale = 1000;
        printf("Generating float data points...\n");
        double *float_data_points = (double *)malloc(sizeof(double)*NUM_DATA_POINTS);
        if (float_data_points == NULL){
            fprintf(stderr,"Could not allocate float data points\n");
            return -1;
        }
        int gen_float_res = gen_float_data_points(float_data_points,NUM_DATA_POINTS);
        if(gen_float_res != 0){
            fprintf(stderr,"Failed to generate float data points\n");
            return -1;
        }
        for(size_t i = 0; i < NUM_DATA_POINTS; i++){
            printf("Data point %zu: %f\n",i,float_data_points[i]);
        }
        printf("Scaling the data points by %ld\n",scale);
        for(size_t i = 0; i < NUM_DATA_POINTS; i++){
            data_points[i] = (dig_t)(float_data_points[i] * scale);
        }
        // Print all the digs

        for(size_t i = 0; i < NUM_DATA_POINTS; i++){
            printf("Data point %zu: %ld\n",i,data_points[i]);
        }
    }else{
        printf("Generating dig data points...\n");
        int gen_res = gen_dig_data_points(data_points,NUM_DATA_POINTS);
        if(gen_res != 0){
            fprintf(stderr,"Failed to generate data points\n");
            return -1;
        }
    }
    int init_res = init_message(message, data_points,NUM_DATA_POINTS);
    if (init_res != 0) {
        fprintf(stderr, "Failed to initialize message\n");
        return -1;
    }
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
        int encode_res = encode_signatures(message, master_sig_buf, master_decoded_sig_buf, NUM_DATA_POINTS);
        if (encode_res != 0) {
            fprintf(stderr, "Failed to encode signatures\n");
            return -1;
        }  
        /* Commence curling to the server */ 
        cJSON *json_obj = cJSON_CreateObject();
        int prepare = prepare_request_server(json_obj,message,master_decoded_sig_buf,data_points,NUM_DATA_POINTS,pk_b64,sig_len,scale);
        if(prepare != 0){
            fprintf(stderr,"Failed to prepare request\n");
            return -1;
        }
        printf("%s\n",cJSON_Print(json_obj));
        curl_to_server("http://localhost:12345/new",json_obj);
        cJSON_Delete(json_obj); 
        sleep(60);
    }

    
  
    // Clean up the RELIC library
    relic_cleanup();
    return 0;
}
