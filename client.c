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

int main(int argc, char *argv[]){
    // Initialize the RELIC library
    if (core_init() == RLC_ERR){
        core_clean();
        // Write to stderr
        fprintf(stderr, "Error initializing RELIC library\n");
        return -1;
    }
    // Set up the parameters for the pairing-based cryptography
    if (pc_param_set_any() != RLC_OK) {
        core_clean();
        fprintf(stderr, "Error setting up RELIC parameters\n");
        return -1;
    }
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
    // print the public key and secret key
   

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
    // Print the message
    bn_print(m);
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
    // Print the signature
    g1_print(sig);
    int sig_len = g1_size_bin(sig,1);
    unsigned char sig_buf[sig_len];
    g1_write_bin(sig_buf, sig_len, sig, 1); // Write the signature to the byte array
    

    // Encode the signature in Base64
    char* sig_b64 = base64_encode((char*)sig_buf, sig_len);
    printf("Base64 Encoded Signature: %s\n", sig_b64);

    CURL *curl;
    CURLcode resu;
    curl = curl_easy_init();

    /* Commence the curling*/ 
    if(curl) {

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: ");

        // JSON setup and sending
        cJSON *json_obj = cJSON_CreateObject();
        cJSON *messages_array = cJSON_CreateArray();
        for (int i = 0; i < 4; i++) {
            cJSON_AddItemToArray(messages_array, cJSON_CreateString(messages[i]));
        }
        cJSON_AddItemToObject(json_obj, "messages", messages_array);
        cJSON_AddStringToObject(json_obj, "signature",sig_b64);
        cJSON_AddNumberToObject(json_obj, "signature_length",sig_len);
        char *json_str = cJSON_Print(json_obj);
        // Specify the URL
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:12345/signature-post");
        // Specify the data to be sent
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);
        // Perform the request
        resu = curl_easy_perform(curl);
        if(resu != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(resu));
        }
        curl_easy_cleanup(curl);
        cJSON_free(json_str); 
        cJSON_Delete(json_obj); 
        free(sig_b64);
    }

    // Clean up the RELIC library
    core_clean();
    return 0;
}
