#include "relic/relic.h"
#include "core/send/send.h"
#include "core/message/message.h"
#include "core/request/request.h"
#include "core/crypto/love/love.h"
#include "core/crypto/mklhs/mklhs.h"
#include "core/crypto/utils/utils.h"
#include "core/request/json.h"

#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#ifdef TEST_MODE
#include "testing/testing.h"
#endif

/* MAIN */
int main(int argc, char *argv[])
{
  printf("Starting client...\n");
#ifdef TEST_MODE
#include "utils/testing.h"
  // Initialize test config
  test_config_t test_config;
  test_config.num_data_points = NUM_DATA_POINTS;
  test_config.num_messages = 1;
  test_config.scale = 1;
  test_config.is_sig = 1;
  printf("Running in TESTING MODE\n");
#endif
  /* Parse command line arguments */
  int iterations_count = 10;

  g2_t pk;
  bn_t sk;
  char *pk_b64 = NULL;

#ifdef TEST_MODE
  clock_t start_setup_keys = clock();
#endif
  // test_connection();
  printf("Connecting to server...\n");
  int sockfd = connect_to_server(SERVER_IP, SERVER_PORT);
  if (sockfd < 0)
  {
    printf("Failed to connect to server\n");
    return -1;
  }
  printf("Connected to server\n");
  relic_init();
  base64_build_dectable();
  /* Generate the secret and public key */
  g2_null(pk);
  bn_null(sk);
  g2_new(pk);
  bn_new(sk);

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
  g2_write_bin(pk_buffer, pk_len, pk, 1);
  size_t encoded_len;
  char *pk_b64_custom = base64_enc(pk_buffer, pk_len, &encoded_len);

#ifdef TEST_MODE
  clock_t end_setup_keys = clock();
  metrics_t setup_keys = get_latency_metrics(start_setup_keys, end_setup_keys, "setup_keys");
  log_latency_metrics_to_csv(&test_config, &setup_keys);
#endif

  int iterations = 0;
  while (iterations < iterations_count)
  {
#ifdef TEST_MODE
    clock_t start_init = clock();
    clock_t start_ete, end_ete;
    start_ete = clock();
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

    /* Create JSON object for the request */
    cJSON *json_obj = cJSON_CreateObject();
    if (json_obj == NULL)
    {
      fprintf(stderr, "Failed to create JSON object\n");
      free(data_points);
      return -1;
    }
    /* Signature mode - create and sign message */
    message_t *message = (message_t *)malloc(sizeof(message_t));
    if (message == NULL)
    {
      fprintf(stderr, "Could not allocate message\n");
      cJSON_Delete(json_obj);
      free(data_points);
      return -1;
    }

    /* Initialize the message */
    int init_res = init_message(message, data_points, NUM_DATA_POINTS);
    if (init_res != 0)
    {
      fprintf(stderr, "Failed to initialize message\n");
      free(message);
      cJSON_Delete(json_obj);
      free(data_points);
      return -1;
    }
#ifdef TEST_MODE
    clock_t end_init = clock();
    metrics_t init_metrics = get_latency_metrics(start_init, end_init, "init");
    log_latency_metrics_to_csv(&test_config, &init_metrics);
#endif
#ifdef TEST_MODE

    clock_t start_sign, end_sign;
    start_sign = clock();
#endif
    /* Sign the data points */
    int sign_res = sign_data_points(message, sk, NUM_DATA_POINTS);
    if (sign_res != 0)
    {
      fprintf(stderr, "Failed to sign data points\n");
      cleanup_message(message, NUM_DATA_POINTS);
      free(message);
      cJSON_Delete(json_obj);
      free(data_points);
      return -1;
    }
#ifdef TEST_MODE
    end_sign = clock();
    // metrics_t sign_metrics = get_metrics(start_sign, end_sign, sizeof(message_t), "sign", test_config);
    // log_metrics_to_csv(&test_config, &sign_metrics);
    metrics_t sign_metrics = get_latency_metrics(start_sign, end_sign, "sign");
    log_latency_metrics_to_csv(&test_config, &sign_metrics);
#endif
    /* Encode signatures */
    unsigned char *master_sig_buf[NUM_DATA_POINTS];
    char *master_decoded_sig_buf[NUM_DATA_POINTS];
#ifdef TEST_MODE
    clock_t start_encode, end_encode;
    start_encode = clock();
#endif
    int sig_len = g1_size_bin(message->sigs[0], 1);
    int encode_res = encode_signatures(message, master_sig_buf,
                                       master_decoded_sig_buf, NUM_DATA_POINTS);
    if (encode_res != 0)
    {
      fprintf(stderr, "Failed to encode signatures\n");
      cleanup_message(message, NUM_DATA_POINTS);
      free(message);
      cJSON_Delete(json_obj);
      free(data_points);
      return -1;
    }

#ifdef TEST_MODE
    end_encode = clock();
    // metrics_t encode_metrics = get_metrics(start_encode, end_encode, sizeof(message_t), "encode", test_config);
    // log_metrics_to_csv(&test_config, &encode_metrics);
    metrics_t encode_metrics = get_latency_metrics(start_encode, end_encode, "encode");
    log_latency_metrics_to_csv(&test_config, &encode_metrics);
#endif

#ifdef TEST_MODE
    clock_t start_love = clock();
#endif
    love_data_t love_data;
    init_love_data(&love_data);
    generate_love_precomputation(&love_data);

#ifdef TEST_MODE
    clock_t end_love = clock();
    metrics_t love_metrics = get_latency_metrics(start_love, end_love, "love");
    log_latency_metrics_to_csv(&test_config, &love_metrics);
#endif
#ifdef TEST_MODE
    clock_t start_prepare, end_prepare;
    start_prepare = clock();
#endif
    // /* Prepare request */
    // int prepare = prepare_request_server(
    //     json_obj, message, master_decoded_sig_buf, data_points, NUM_DATA_POINTS,
    //     pk_b64, sig_len, scale, FUNC);
    // if (prepare != 0)
    // {
    //   fprintf(stderr, "Failed to prepare request\n");
    //   cleanup_message(message, NUM_DATA_POINTS);
    //   free(message);
    //   cJSON_Delete(json_obj);
    //   free(data_points);
    //   return -1;
    // }
    /* Print the JSON object */
    char json_buffer[JSON_BUFFER_SIZE]; // Adjust size based on expected data volume
    json_t json;
    json_init(&json, json_buffer, sizeof(json_buffer));
    printf("JSON buffer size: %zu\n", sizeof(json_buffer));
    int prepare_json = prepare_req_server(&json, message, master_decoded_sig_buf,
                                          data_points, NUM_DATA_POINTS, pk_b64_custom, sig_len,
                                          scale, FUNC, &love_data);
    if (prepare_json != 0)
    {
      fprintf(stderr, "Failed to prepare request\n");
      cleanup_message(message, NUM_DATA_POINTS);
      free(message);
      cJSON_Delete(json_obj);
      free(data_points);
      return -1;
    }
    // int prepare_love = add_love_data_json(json_obj, &love_data);
    // if (prepare_love != 0)
    // {
    //   fprintf(stderr, "Failed to add love data to the json object\n");
    //   return -1;
    // }

#ifdef TEST_MODE
    end_prepare = clock();
    // metrics_t prepare_metrics = get_metrics(start_prepare, end_prepare, sizeof(message_t), "prepare", test_config);
    // log_metrics_to_csv(&test_config, &prepare_metrics);
    metrics_t prepare_metrics = get_latency_metrics(start_prepare, end_prepare, "prepare");
    log_latency_metrics_to_csv(&test_config, &prepare_metrics);
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

    /* Send to server */
    // int curl_res;
#ifdef TEST_MODE
    clock_t start_req = clock();
#endif
    // curl_res = curl_to_server("http://129.242.236.85:12345/new", json_obj);
    // if (curl_res != 0)
    // {
    //   fprintf(stderr, "Failed to curl to server\n");
    //   cJSON_Delete(json_obj);
    //   free(data_points);
    //   return -1;
    // }
    char request[BUFFER_SIZE];
    request_t req;
    if (setup_POST(request, sockfd, &req, json.buffer, "/new", SERVER_IP) != 0)
    {
      fprintf(stderr, "Failed to setup POST request\n");
      cJSON_Delete(json_obj);
      free(data_points);
      return -1;
    }
    char response[BUFFER_SIZE];
    int res = http_POST(response, &req, sizeof(response));
    if (res < 0)
    {
      fprintf(stderr, "Failed to send POST request\n");
      cJSON_Delete(json_obj);
      free(data_points);
      return -1;
    }
    // Print the response
    printf("Response: %s\n", response);
#ifdef TEST_MODE
    clock_t end_req = clock();
    end_ete = clock();
    // metrics_t request_metrics = get_metrics(start_req, end_req, strlen(cJSON_Print(json_obj)), "request", test_config);
    // log_metrics_to_csv(&test_config, &request_metrics);
    // end-to-end metrics
    metrics_t ete_metrics = get_latency_metrics(start_ete, end_ete, "client-to-server");
    log_latency_metrics_to_csv(&test_config, &ete_metrics);
#endif

    /* Cleanup for this iteration */
    cJSON_Delete(json_obj);
    free(data_points);

    iterations++;
  }
  free(pk_b64);
  return 0;
}