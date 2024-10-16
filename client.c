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
    bn_t test_m;
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
    bn_read_bin(m, messages, num_messages);
    
    printf("PRINT BN_T M:");
    bn_print(m);
  
    // Generate uuid 
    uuid_t uuid,ds_id,date; 
    uuid_generate(uuid);
    uuid_generate(ds_id);
    uuid_generate_time(date);

    char uuid_str[37], ds_id_str[37], date_str[37];
    uuid_unparse(uuid, uuid_str);
    uuid_unparse(ds_id, ds_id_str);
    uuid_unparse(date, date_str);

    printf("UUID: %s\n", uuid_str);
    printf("Dataset ID: %s\n", ds_id_str);
    printf("Date: %s\n", date_str);
    /* Random dataset id, id and tag */
    const char *data_set_id = uuid_str;
    const char *id = ds_id_str;
    const char *tag = date_str;

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
