#include <stdio.h>
#include <stdlib.h>
#include "relic/relic.h"
/* Internal includes */
#include "core/send/send.h"
#include "core/request/json.h"
#include "core/message/message.h"
#include "core/request/request.h"
#include "core/crypto/mklhs/mklhs.h"
#include "core/utils/base64.h"
#include "core/utils/utils.h"

#ifdef TEST_MODE
#include "testing/testing.h"
#endif

/* MAIN */
int main(int argc, char *argv[])
{
  int iterations = 0;
  int iterations_count = 1000;
  relic_init();
  int sockfd = connect_to_server(SERVER_IP, SERVER_PORT);
  if (sockfd < 0)
  {
    printf("Failed to connect to server\n");
    return -1;
  }
  g2_t pk;
  bn_t sk;
  g2_null(pk);
  bn_null(sk);
  g2_new(pk);
  bn_new(sk);
  char *pk_b64_custom = NULL;
  while (iterations < iterations_count)
  {
#ifdef TEST_MODE
    struct timeval start_setup_keys = timer_start();
#endif
    /* Generate key pair */
    int res = gen_keys(sk, pk);
    if (res != 0)
    {
      fprintf(stderr, "Failed to generate keys\n");
      return -1;
    }
    /* Format and encode the public key */
    int pk_len = g2_size_bin(pk, 1);
    char pk_buffer[pk_len];
    g2_write_bin((uint8_t *)pk_buffer, pk_len, pk, 1);
    size_t encoded_len;
    pk_b64_custom = base64_enc(pk_buffer, pk_len, &encoded_len);
    iterations++;
#ifdef TEST_MODE
    timer_end(start_setup_keys, "genkeys");
#endif
  }
  iterations = 0;
  while (iterations < iterations_count)
  {
#ifdef TEST_MODE
    struct timeval start_init = timer_start();
#endif
    uint64_t scale = 1;
    /* Allocate the data points */
    dig_t *data_points = (dig_t *)malloc(sizeof(dig_t) * NUM_DATA_POINTS);
    if (data_points == NULL)
    {
      fprintf(stderr, "Could not allocate data points\n");
      return -1;
    }
    /* Generate data points based on arguments */
    int gen_res = gen_dig_data_points(data_points, NUM_DATA_POINTS);
    if (gen_res != 0)
    {
      fprintf(stderr, "Failed to generate data points\n");
      return -1;
    }
    message_t *message = (message_t *)malloc(sizeof(message_t));
    if (message == NULL)
    {
      fprintf(stderr, "Could not allocate message\n");
      free(data_points);
      return -1;
    }
    /* Initialize the message */
    int init_res = init_message(message, data_points, NUM_DATA_POINTS);
    if (init_res != 0)
    {
      fprintf(stderr, "Failed to initialize message\n");
      free(message);
      free(data_points);
      return -1;
    }
#ifdef TEST_MODE
    timer_end(start_init, "init");
    struct timeval start_sign = timer_start();
#endif
    /* Sign the data points */
    int sign_res = sign_data_points(message, sk, NUM_DATA_POINTS);
    if (sign_res != 0)
    {
      fprintf(stderr, "Failed to sign data points\n");
      cleanup_message(message, NUM_DATA_POINTS);
      free(message);
      free(data_points);
      return -1;
    }
#ifdef TEST_MODE
    timer_end(start_sign, "sign");
#endif
    /* Encode signatures */
    unsigned char *master_sig_buf[NUM_DATA_POINTS];
    char *master_decoded_sig_buf[NUM_DATA_POINTS];
#ifdef TEST_MODE
    struct timeval start_encode = timer_start();
#endif
    int sig_len = g1_size_bin(message->sigs[0], 1);
    int encode_res = encode_signatures(message, master_sig_buf,
                                       master_decoded_sig_buf, NUM_DATA_POINTS);
    if (encode_res != 0)
    {
      fprintf(stderr, "Failed to encode signatures\n");
      cleanup_message(message, NUM_DATA_POINTS);
      free(message);
      free(data_points);
      return -1;
    }
#ifdef TEST_MODE
    timer_end(start_encode, "encode");
    struct timeval start_prepare = timer_start();
#endif
    /* Print the JSON object */
    char json_buffer[JSON_BUFFER_SIZE]; // Adjust size based on expected data volume
    json_t json;
    json_init(&json, json_buffer, sizeof(json_buffer));
    int prepare_json = prepare_req_server(&json, message, master_decoded_sig_buf,
                                          data_points, NUM_DATA_POINTS, pk_b64_custom, sig_len,
                                          scale, FUNC);
    if (prepare_json != 0)
    {
      fprintf(stderr, "Failed to prepare request\n");
      cleanup_message(message, NUM_DATA_POINTS);
      free(message);
      free(data_points);
      return -1;
    }
#ifdef TEST_MODE
    timer_end(start_prepare, "prepare");
    struct timeval start_req = timer_start();
#endif
    /* Clean up message resources */
    cleanup_message(message, NUM_DATA_POINTS);
    free(message);

    // Free encoded signatures
    for (int i = 0; i < NUM_DATA_POINTS; i++)
    {
      free(master_decoded_sig_buf[i]);
      free(master_sig_buf[i]);
    }
    // Format and send POST
    char request[BUFFER_SIZE];
    request_t req;
    if (setup_POST(request, sockfd, &req, json.buffer, "/new", SERVER_IP) != 0)
    {
      fprintf(stderr, "Failed to setup POST request\n");
      free(data_points);
      return -1;
    }
    char response[BUFFER_SIZE];
    int res = http_POST(response, &req, sizeof(response));
    if (res < 0)
    {
      fprintf(stderr, "Failed to send POST request\n");
      free(data_points);
      return -1;
    }
    // ok
    printf("ok\n");
#ifdef TEST_MODE
    timer_end(start_req, "request");
#endif
    /* Cleanup for this iteration */
    free(data_points);
    iterations++;
  }
  free(pk_b64_custom);
  return 0;
}