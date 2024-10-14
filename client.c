#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <b64/cencode.h>
#include <b64/cdecode.h>
#include <curl/curl.h>
#include <cjson/cJSON.h> 
#include <time.h>
#include <math.h>
#include "relic/relic.h"


char* base64_encode(const char* input, int length) {
    base64_encodestate state;
    base64_init_encodestate(&state);

    int encoded_length = 4 * ((length + 2) / 3); // Base64 encoded length
    char* encoded = (char*)malloc(encoded_length + 1); // +1 for null terminator
    int len = base64_encode_block(input, length, encoded, &state);
    len += base64_encode_blockend(encoded + len, &state);
    encoded[len] = '\0'; // Null-terminate the encoded string

    return encoded;
}

char* base64_decode(const char* input, int length, int* decoded_length) {
    base64_decodestate state;
    base64_init_decodestate(&state);

    char* decoded = (char*)malloc(length * 3 / 4 + 1); // Allocate enough space for decoded data
    *decoded_length = base64_decode_block(input, length, decoded, &state);
    decoded[*decoded_length] = '\0'; // Null-terminate the decoded string

    return decoded;
}

int relic_init() {
    if (core_init() == RLC_ERR) {
        core_clean();
        return RLC_ERR;
    }
    if (pc_param_set_any() != RLC_OK) {
        core_clean();
        return RLC_ERR;
    }
    return RLC_OK;
}

int relic_cleanup() {
    core_clean();
    return RLC_OK;
}

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
/* MAIN */
int main(int argc, char *argv[]){
    /* Initialize the RELIC library */ 
    relic_init();
    /* Initialize the needed variables */
    g2_t pk;
    g2_null(pk);
    g2_new(pk);

    g1_t sig;
    g1_null(sig);
    g1_new(sig);
    
    bn_t sk,m;
    bn_null(sk);
    bn_new(sk);
    bn_null(m);
    bn_new(m);

    /* Generate key pair */
    int res = cp_mklhs_gen(sk,pk);
    assert(res == RLC_OK);
    printf("PUBLIC KEY:\n");
    g2_print(pk);

    /* Format and encode the public key  */ 
    int pk_len = g2_size_bin(pk,1);
    unsigned char pk_buf[pk_len];
    g2_write_bin(pk_buf, pk_len, pk, 1); // Write the public key to the byte array  
    char* pk_b64 = base64_encode((char*)pk_buf, pk_len);
    
    /* Generate and format message*/
    int num_messages = 10;
    uint8_t messages[num_messages]; 
    int data_gen_res = gen_data(messages,num_messages);
    assert(data_gen_res == 0);
    bn_read_bin(m, messages, 10);
    printf("PRINT BN_T M:");
    bn_print(m);

    /* Random dataset id, id and tag */
    const char *data_set_id = "123";
    const char *id = "456";
    const char *tag = "789";

    /* Sign the data */
    res = cp_mklhs_sig(sig,m,data_set_id,id,tag,sk);
    assert(res == RLC_OK);
    printf("SIGNATURE\n");
    g1_print(sig);

    /* Convert the signature and encode signature */
    int sig_len = g1_size_bin(sig,1);
    unsigned char sig_buf[sig_len];
    g1_write_bin(sig_buf, sig_len, sig, 1); // Write the signature to the byte array
    char* sig_b64 = base64_encode((char*)sig_buf, sig_len);

    /* The code before the curling is test code*/
    int decoded_sig_len;
    unsigned char* decoded_sig = (unsigned char*)base64_decode(sig_b64, strlen(sig_b64), &decoded_sig_len);
    if (decoded_sig == NULL) {
        fprintf(stderr, "Failed to decode signature\n");
        return -1;
    }
    g1_t sig_converted;
    g1_null(sig_converted);
    g1_new(sig_converted);
    g1_read_bin(sig_converted, decoded_sig, decoded_sig_len);
    printf("Converted Signature:\n");
    g1_print(sig_converted);
    
    CURL *curl_server;
    CURLcode res_server;
    curl_server = curl_easy_init();

    /* Commence curling to the server */ 
    if(curl_server) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        // JSON setup and sending
        cJSON *json_obj = cJSON_CreateObject();
      
        if(!prepare_req_server_num(json_obj,messages,10,sig_b64,sig_len,(char *)data_set_id,(char *)id,(char *)tag,pk_b64)){
            fprintf(stderr, "Failed to prepare request\n");
            return -1;
        }
        printf("%s\n",cJSON_Print(json_obj));
        char *json_str = cJSON_Print(json_obj);
        // Specify the URL
        curl_easy_setopt(curl_server, CURLOPT_URL, "http://localhost:12345/new");
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
        cJSON_Delete(json_obj); 
        free(sig_b64);
        
    }
  
    // Clean up the RELIC library
    relic_cleanup();
    return 0;
}
