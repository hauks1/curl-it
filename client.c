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
/**
 * @brief Prepares a JSON object with server request details.
 *
 * This function takes a JSON object, an array of messages, a base64-encoded signature,
 * and the length of the signature, and populates the JSON object with these details.
 *
 * @param server_obj A pointer to a cJSON object that will be populated with the request details.
 * @param messages An array of strings containing the messages to be added to the JSON object.
 * @param sig_b64 A string containing the base64-encoded signature.
 * @param sig_len An integer representing the length of the signature.
 * 
 * @return Returns 1 on success, 0 on failure.
 */
int prepare_req_server(cJSON *server_obj,char **messages, char *sig_b64, int sig_len){
    cJSON *messages_array = cJSON_CreateArray();
    for (int i = 0; i < 4; i++) {
        cJSON_AddItemToArray(messages_array, cJSON_CreateString(messages[i]));
    }
    
    if(!cJSON_AddItemToObject(server_obj, "messages", messages_array)){
        return 0;
    }
    if(!cJSON_AddStringToObject(server_obj, "signature",sig_b64)){
        return 0;
    }
    if(!cJSON_AddNumberToObject(server_obj, "signature_length",sig_len)){
        return 0;
    }
    return 1;
}
/**
 * @brief Prepares a JSON object with verifier information.
 *
 * This function adds various fields to a given cJSON object, including
 * data set ID, ID, tag, signature, signature length, and a public key.
 * The public key is converted to a byte array, encoded in Base64, and
 * then added to the JSON object.
 *
 * @param verifier_obj The cJSON object to which the fields will be added.
 * @param data_set_id The data set ID to be added to the JSON object.
 * @param id The ID to be added to the JSON object.
 * @param tag The tag to be added to the JSON object.
 * @param sig_b64 The Base64 encoded signature to be added to the JSON object.
 * @param sig_len The length of the signature.
 * @param pk The public key to be converted, encoded, and added to the JSON object.
 * 
 * @return int Returns 1 on success, 0 on failure.
 */
int prepare_req_verifier(cJSON *verifier_obj, char *data_set_id, char *id, char *tag, char *sig_b64, int sig_len, g2_t pk){

    if(!cJSON_AddStringToObject(verifier_obj, "data_set_id",data_set_id)){
        return 0;
    }
    if(!cJSON_AddStringToObject(verifier_obj, "id",id)){
        return 0;
    }
    if(!cJSON_AddStringToObject(verifier_obj, "tag",tag)){
        return 0;
    }
    if(!cJSON_AddStringToObject(verifier_obj, "signature",sig_b64)){
        return 0;
    }
    if(!cJSON_AddNumberToObject(verifier_obj, "signature_length",sig_len)){
        return 0;
    }
    // Convert the public key to a byte array
    int pk_len = g2_size_bin(pk,1);
    unsigned char pk_buf[pk_len];
    g2_write_bin(pk_buf, pk_len, pk, 1); // Write the public key to the byte array
    // Encode the public key in Base64
    char* pk_b64 = base64_encode((char*)pk_buf, pk_len);
    if(!cJSON_AddStringToObject(verifier_obj, "public_key",pk_b64)){
        return 0;
    }

    return 1;
}


int main(int argc, char *argv[]){
    // Initialize the RELIC library
    relic_init();
    bn_t sk;
    g2_t pk;
    // Initialize variables to null state
    bn_null(sk);
    g2_null(pk);
    // Allocate memory for variables
    bn_new(sk);
    g2_new(pk);

    // Generate key pair
    int res = cp_mklhs_gen(sk,pk);
    assert(res == RLC_OK);

    // Message array with some messages 
    char *str_m1 = "69";
    char *str_m2 = "42";
    char *str_m3 = "12";
    char *str_m4 = "45";
    // Add the messages to the array
    char *messages[4] = {str_m1,str_m2,str_m3,str_m4};
    // Convert str_m to bn_t
    bn_t m;
    bn_null(m);
    bn_new(m);
    char temp[1024] = {0};
    for (int i = 1; i < 2; i++) {
        strcat(temp,messages[i]);
    }
    bn_read_str(m, temp, strlen(temp),10);
    // Sign the message
    g1_t sig;
    g1_null(sig);
    g1_new(sig);
    // Random dataset identifier, id and tag
    const char *data_set_id = "123";
    const char *id = "456";
    const char *tag = "789";
    // Sign the data
    res = cp_mklhs_sig(sig,m,data_set_id,id,tag,sk);
    assert(res == RLC_OK);
    // Convert the signature to a byte array
    int sig_len = g1_size_bin(sig,1);
    unsigned char sig_buf[sig_len];
    g1_write_bin(sig_buf, sig_len, sig, 1); // Write the signature to the byte array
    // Encode the signature in Base64
    char* sig_b64 = base64_encode((char*)sig_buf, sig_len);    
    // Set up the CURL request
    CURL *curl_server, *curl_verifier;
    CURLcode res_server, res_verifier;
    curl_server = curl_easy_init();
    curl_verifier = curl_easy_init();

    /* Commence curling to the server */ 
    if(curl_server) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        // JSON setup and sending
        cJSON *json_obj = cJSON_CreateObject();
        if(prepare_req_server(json_obj,messages,sig_b64,sig_len) == 0){
            fprintf(stderr, "Failed to prepare request\n");
            return 0;
        }
        char *json_str = cJSON_Print(json_obj);
        // Specify the URL
        curl_easy_setopt(curl_server, CURLOPT_URL, "http://localhost:12345/signature-post");
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
    if(curl_verifier){
        
    }


    // Clean up the RELIC library
    relic_cleanup();
    return 0;
}
