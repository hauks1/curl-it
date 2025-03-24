#include "relic/relic.h"
#include "utils/crypto.h"
#include "utils/request.h"
#include "utils/memory.h"
#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#define MAX_DATA_POINTS 30
#define SERVER_URL

#ifdef TEST_MODE
#include "utils/testing.h"
#endif

/* Function to initialize the messages */
int init_message(message_t *message, dig_t data_points[],
                 size_t num_data_points)
{
  if (message == NULL)
  {
    fprintf(stderr, "Message pointer is NULL\n");
    return -1;
  }
  for (int i = 0; i < num_data_points; i++)
  {
    bn_null(message->data_points[i]);
    bn_new(message->data_points[i]);
    bn_set_dig(message->data_points[i], data_points[i]);

    // Initialize signature
    g1_null(message->sigs[i]);
    g1_new(message->sigs[i]);

    // Create random tag's
    uuid_t date;
    uuid_generate_time(date);
    char date_str[37];
    uuid_unparse(date, date_str);

    strncpy(message->tags[i], date_str, sizeof(message->tags[i]));
  }
  uuid_t uuid;
  uuid_generate(uuid);
  char uuid_str[37];
  uuid_unparse(uuid, uuid_str);
  strncpy(message->ids[0], DEVICE_ID, sizeof(DEVICE_ID));
  // Set data_set_id
  strncpy(message->data_set_id, TEST_DATABASE, sizeof(message->data_set_id));

  return 0;
}

/* Function to init raw messages aka wihtout a signature */
int init_raw_message(raw_message_t *message, dig_t data_points[],
                     size_t num_data_points)
{
  if (message == NULL)
  {
    fprintf(stderr, "Message pointer is NULL\n");
    return -1;
  }
  for (int i = 0; i < num_data_points; i++)
  {
    bn_null(message->data_points[i]);
    bn_new(message->data_points[i]);
    bn_set_dig(message->data_points[i], data_points[i]);

    // Create random tag's
    uuid_t date;
    uuid_generate_time(date);
    char date_str[37];
    uuid_unparse(date, date_str);

    strncpy(message->tags[i], date_str, sizeof(message->tags[i]));
  }
  uuid_t uuid;
  uuid_generate(uuid);
  char uuid_str[37];
  uuid_unparse(uuid, uuid_str);
  strncpy(message->ids[0], DEVICE_ID, sizeof(DEVICE_ID));
  // Set data_set_id
  strncpy(message->data_set_id, TEST_DATABASE, sizeof(message->data_set_id));

  return 0;
}

/* Function to neatly print a message struct */
void print_message(message_t *msg)
{
  for (int i = 0; i < NUM_DATA_POINTS; i++)
  {
    printf("Data point: %d\n", i);
    bn_print(msg->data_points[i]);
    printf("Signature: %d\n", i);
    g1_print(msg->sigs[i]);
    printf("ID: %s\n", msg->ids[i]);
    printf("Tag: %s\n", msg->tags[i]);
  }
  printf("Data set id: %s\n", msg->data_set_id);
}

void cleanup_message(message_t *message, size_t num_data_points)
{
  if (message == NULL)
    return;

  for (size_t i = 0; i < num_data_points; i++)
  {
    bn_free(message->data_points[i]);
    g1_free(message->sigs[i]);
  }
}
/* Function to neatly print a message struct */
void print_raw_message(raw_message_t *msg)
{
  for (int i = 0; i < NUM_DATA_POINTS; i++)
  {
    printf("Data point: %d\n", i);
    bn_print(msg->data_points[i]);
    printf("ID: %s\n", msg->ids[i]);
    printf("Tag: %s\n", msg->tags[i]);
  }
  printf("Data set id: %s\n", msg->data_set_id);
}

void cleanup_raw_message(raw_message_t *message, size_t num_data_points)
{
  if (message == NULL)
    return;

  for (size_t i = 0; i < num_data_points; i++)
  {
    bn_free(message->data_points[i]);
  }
}

void print_usage(char *program_name)
{
  printf("Usage: %s [options]\n", program_name);
  printf("Options:\n");
  printf("  --float          Generate float data points\n");
  printf("  --iterations N   Run N iterations (default: 1000)\n");
  printf("  --test           Enable performance testing mode\n");
  printf("  --memory           Enable memory testing mode\n");
  printf("  --raw            Send raw data without signatures\n");
  printf("  --verbose        Enable verbose output\n");
  printf("  --help           Display this help message\n");
}

/* Initialize LOVE data structure */
int init_love_data(love_data_t *love_data)
{
  if (love_data == NULL)
  {
    fprintf(stderr, "LOVE data pointer is NULL\n");
    return -1;
  }

  // Initialize RELIC elements
  bn_null(love_data->r);
  g1_null(love_data->u1);
  g2_null(love_data->u2);
  g2_null(love_data->v2);
  gt_null(love_data->e);

  bn_new(love_data->r);
  g1_new(love_data->u1);
  g2_new(love_data->u2);
  g2_new(love_data->v2);
  gt_new(love_data->e);

  // Initialize encoded fields to NULL
  love_data->r_encoded = NULL;
  love_data->u1_encoded = NULL;
  love_data->u2_encoded = NULL;
  love_data->v2_encoded = NULL;
  love_data->e_encoded = NULL;

  return 0;
}

/* Clean up LOVE data structure */
void cleanup_love_data(love_data_t *love_data)
{
  if (love_data == NULL)
  {
    return;
  }

  // Free RELIC elements
  bn_free(love_data->r);
  g1_free(love_data->u1);
  g2_free(love_data->u2);
  g2_free(love_data->v2);
  gt_free(love_data->e);

  // Free encoded strings
  if (love_data->r_encoded)
    free(love_data->r_encoded);
  if (love_data->u1_encoded)
    free(love_data->u1_encoded);
  if (love_data->u2_encoded)
    free(love_data->u2_encoded);
  if (love_data->v2_encoded)
    free(love_data->v2_encoded);
  if (love_data->e_encoded)
    free(love_data->e_encoded);
}
/* Generate LOVE precomputation parameters */
int generate_love_precomputation(love_data_t *love_data)
{
  if (love_data == NULL)
  {
    fprintf(stderr, "LOVE data pointer is NULL\n");
    return -1;
  }

  // Generate LOVE precomputation using the struct fields
  int result = cp_lvpub_gen(love_data->r, love_data->u1, love_data->u2,
                            love_data->v2, love_data->e);

  if (result != RLC_OK)
  {
    fprintf(stderr, "LOVE precomputation failed\n");
    return -1;
  }

  // Encode the parameters for transmission
  int r_len = bn_size_bin(love_data->r);
  unsigned char r_buf[r_len];
  bn_write_bin(r_buf, r_len, love_data->r);
  love_data->r_encoded = base64_encode((char *)r_buf, r_len);

  int u1_len = g1_size_bin(love_data->u1, 1);
  unsigned char u1_buf[u1_len];
  g1_write_bin(u1_buf, u1_len, love_data->u1, 1);
  love_data->u1_encoded = base64_encode((char *)u1_buf, u1_len);

  int u2_len = g2_size_bin(love_data->u2, 1);
  unsigned char u2_buf[u2_len];
  g2_write_bin(u2_buf, u2_len, love_data->u2, 1);
  love_data->u2_encoded = base64_encode((char *)u2_buf, u2_len);

  int v2_len = g2_size_bin(love_data->v2, 1);
  unsigned char v2_buf[v2_len];
  g2_write_bin(v2_buf, v2_len, love_data->v2, 1);
  love_data->v2_encoded = base64_encode((char *)v2_buf, v2_len);

  int e_len = gt_size_bin(love_data->e, 1);
  unsigned char e_buf[e_len];
  gt_write_bin(e_buf, e_len, love_data->e, 1);
  love_data->e_encoded = base64_encode((char *)e_buf, e_len);

  return 0;
}
/* MAIN */
int main(int argc, char *argv[])
{
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
  int use_float = 0;
  int test_mode = 0;
  int verbose = 0;
  int raw_mode = 0;
  int memory_mode = 0;
  int iterations_count = 1000;

  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--float") == 0)
    {
      use_float = 1;
    }
    else if (strcmp(argv[i], "--test") == 0)
    {
      test_mode = 1;
    }
    else if (strcmp(argv[i], "--raw") == 0)
    {
      raw_mode = 1;
    }
    else if (strcmp(argv[i], "--memory") == 0)
    {
      memory_mode = 1;
    }
    else if (strcmp(argv[i], "--iterations") == 0 && i + 1 < argc)
    {
      iterations_count = atoi(argv[i + 1]);
      i++; // Skip the next argument
    }
    else if (strcmp(argv[i], "--help") == 0)
    {
      print_usage(argv[0]);
      return 0;
    }
    else
    {
      printf("Unknown option: %s\n", argv[i]);
      print_usage(argv[0]);
      return 1;
    }
  }

  g2_t pk;
  bn_t sk;
  char *pk_b64 = NULL;

  if (!raw_mode)
  {
#ifdef TEST_MODE
    clock_t start_setup_keys = clock();
#endif
    relic_init();
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
    int sk_len = bn_size_bin(sk);
    unsigned char pk_buf[pk_len];
    g2_write_bin(pk_buf, pk_len, pk, 1);
    pk_b64 = base64_encode((char *)pk_buf, pk_len);
#ifdef TEST_MODE
    clock_t end_setup_keys = clock();
    metrics_t setup_keys = get_latency_metrics(start_setup_keys, end_setup_keys, "setup_keys");
    log_latency_metrics_to_csv(&test_config, &setup_keys);
#endif
  }

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
    if (use_float)
    {
      scale = 1000000;
      if (verbose)
        printf("Generating float data points...\n");
      double *float_data_points =
          (double *)malloc(sizeof(double) * NUM_DATA_POINTS);
      if (float_data_points == NULL)
      {
        fprintf(stderr, "Could not allocate float data points\n");
        return -1;
      }
      int gen_float_res =
          gen_float_data_points(float_data_points, NUM_DATA_POINTS);
      if (gen_float_res != 0)
      {
        fprintf(stderr, "Failed to generate float data points\n");
        return -1;
      }
    }
    else
    {
      int gen_res = gen_dig_data_points(data_points, NUM_DATA_POINTS);
      if (gen_res != 0)
      {
        fprintf(stderr, "Failed to generate data points\n");
        return -1;
      }
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
    love_data_t love_data;
    init_love_data(&love_data);
    generate_love_precomputation(&love_data);

#ifdef TEST_MODE
    clock_t start_prepare, end_prepare;
    start_prepare = clock();
#endif
    /* Prepare request */
    int prepare = prepare_request_server(
        json_obj, message, master_decoded_sig_buf, data_points, NUM_DATA_POINTS,
        pk_b64, sig_len, scale, FUNC);
    if (prepare != 0)
    {
      fprintf(stderr, "Failed to prepare request\n");
      cleanup_message(message, NUM_DATA_POINTS);
      free(message);
      cJSON_Delete(json_obj);
      free(data_points);
      return -1;
    }
    int prepare_love = add_love_data_json(json_obj,&love_data);
    if(prepare_love != 0){
      fprintf(stderr,"Failed to add love data to the json object\n");
      return -1;
    }

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
    int curl_res;
#ifdef TEST_MODE
    clock_t start_req = clock();
#endif
    printf("%s\n",cJSON_Print(json_obj));
    curl_res = curl_to_server("http://129.242.236.85:12345/new", json_obj);
    if (curl_res != 0)
    {
      fprintf(stderr, "Failed to curl to server\n");
      cJSON_Delete(json_obj);
      free(data_points);
      return -1;
    }

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