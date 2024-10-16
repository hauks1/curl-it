#ifndef PARSING_H 
#define PARSING_H
#include <cjson/cJSON.h>

int prepare_req_server_num(cJSON *server_obj,uint8_t *messages,int messages_len, char *sig_b64, int sig_len, char *data_set_id, char *id, char *tag,char *pk_b64);
int gen_data(uint8_t *messages,int num_messages);


#endif