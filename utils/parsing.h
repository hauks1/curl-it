#ifndef PARSING_H 
#define PARSING_H
#include <cjson/cJSON.h>
#include <relic/relic.h>

#define TEST_DATABASE "test.db"
#define NUM_DATA_POINTS 5
#define NUM_MESSAGES 1


int prepare_req_server_num(cJSON *server_obj,uint8_t *messages,int messages_len, char *sig_b64, int sig_len, char *data_set_id, char *id, char *tag,char *pk_b64,uint64_t scale);
int gen_data(uint8_t *messages,int num_messages);

typedef struct message {
    bn_t data_points[NUM_DATA_POINTS];
    g1_t sigs[NUM_DATA_POINTS];
    char ids[NUM_DATA_POINTS][37];
    char tags[NUM_DATA_POINTS][37];
    char data_set_id[37];
}message_t;


#endif