#include "relic/relic.h"
#include "utils/crypto.h"
#include "utils/request.h"
#include "utils/testing.h"
#include <assert.h>
#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

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
  printf("  --raw            Send raw data without signatures\n");
  printf("  --verbose        Enable verbose output\n");
  printf("  --help           Display this help message\n");
}

/* MAIN */
int main(int argc, char *argv[])
{
  relic_init();
  /* Parse command line arguments */
  int use_float = 0;
  int test_mode = 0;
  int verbose = 0;
  int raw_mode = 0;
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
    else if (strcmp(argv[i], "--verbose") == 0)
    {
      verbose = 1;
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

  if (verbose)
  {
    printf("Configuration:\n");
    printf("- Iterations: %d\n", iterations_count);
    printf("- Testing mode: %s\n", test_mode ? "enabled" : "disabled");
    printf("- Data type: %s\n", use_float ? "float" : "integer");
    printf("- Signature mode: %s\n", raw_mode ? "disabled (raw data)" : "enabled");
  }

  /* Initialize the RELIC library if we're using signatures */
  g2_t pk;
  bn_t sk;
  char *pk_b64 = NULL;

  if (!raw_mode)
  {

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
    unsigned char pk_buf[pk_len];
    g2_write_bin(pk_buf, pk_len, pk, 1);
    pk_b64 = base64_encode((char *)pk_buf, pk_len);
  }

  int iterations = 0;
  while (iterations < iterations_count)
  {
    uint64_t scale = 1;

    /* Allocate the data points */
    dig_t *data_points = (dig_t *)malloc(sizeof(dig_t) * NUM_DATA_POINTS);
    if (data_points == NULL)
    {
      fprintf(stderr, "Could not allocate data points\n");
      return -1;
    }

    /* Start measuring end-to-end latency if in test mode */
    clock_t start_end_to_end;
    if (test_mode)
    {
      start_end_to_end = clock();
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
      if (verbose)
        printf("Generating integer data points...\n");
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

    if (raw_mode)
    {
      /* Raw data mode - skip signatures */
      raw_message_t *message = (raw_message_t *)malloc(sizeof(raw_message_t));
      if (message == NULL)
      {
        fprintf(stderr, "Could not allocate message\n");
        cJSON_Delete(json_obj);
        free(data_points);
        return -1;
      }

      /* Initialize the message */
      int init_res = init_raw_message(message, data_points, NUM_DATA_POINTS);
      if (init_res != 0)
      {
        fprintf(stderr, "Failed to initialize raw message\n");
        free(message);
        cJSON_Delete(json_obj);
        free(data_points);
        return -1;
      }
      if (verbose) printf("Preparing raw data request...\n");
      /* Perform testing if in test mode */
      metrics_t raw_metrics;
      test_config_t test_config = {
          .num_data_points = NUM_DATA_POINTS,
          .num_messages = MAX_ITERATIONS,
          .scale = scale,
          .is_sig = 0,
      };
      clock_t start_raw, end_raw;
      if (test_mode){
        start_raw = clock();
      }
      int prepare_res = prepare_raw_req_server(json_obj, message, data_points, NUM_DATA_POINTS, scale);
      if (prepare_res != 0)
      {
        fprintf(stderr, "Failed to prepare raw request\n");
        cJSON_Delete(json_obj);
        free(data_points);
        return -1;
      }
      if(test_mode){
        end_raw = clock();
        raw_metrics = get_metrics(start_raw,end_raw,sizeof(raw_message_t),"prepare",test_config);
        log_metrics_to_csv(&test_config,&raw_metrics);
      }
      /* Clean up message resources */
      cleanup_raw_message(message, NUM_DATA_POINTS);
      free(message);
    }
    else
    {
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
      /* Perform testing if in test mode */
      metrics_t signing_metrics, encoding_metrics, preparing_metrics;
      test_config_t test_config = {
          .num_data_points = NUM_DATA_POINTS,
          .num_messages = MAX_ITERATIONS,
          .scale = scale,
          .is_sig = 1,
      };

      /* Sign the data points */
      clock_t start_sig, end_sig;
      if (test_mode)
      {
        start_sig = clock();
      }

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

      if (test_mode)
      {
        end_sig = clock();
        signing_metrics = get_metrics(start_sig, end_sig, sizeof(message_t), "signing", test_config);
        log_metrics_to_csv(&test_config, &signing_metrics);
      }

      /* Encode signatures */
      unsigned char *master_sig_buf[NUM_DATA_POINTS];
      char *master_decoded_sig_buf[NUM_DATA_POINTS];
      clock_t start_encode, end_encode;

      if (test_mode)
      {
        start_encode = clock();
      }

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

      if (test_mode)
      {
        end_encode = clock();
        encoding_metrics = get_metrics(start_encode, end_encode, sizeof(message_t), "encoding", test_config);
        log_metrics_to_csv(&test_config, &encoding_metrics);
      }

      /* Prepare request */
      clock_t start_prepare, end_prepare;
      if (test_mode)
      {
        start_prepare = clock();
      }

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

      if (test_mode)
      {
        end_prepare = clock();
        preparing_metrics = get_metrics(start_prepare, end_prepare, sizeof(message_t), "preparing", test_config);
        log_metrics_to_csv(&test_config, &preparing_metrics);
      }

      /* Clean up message resources */
      cleanup_message(message, NUM_DATA_POINTS);
      free(message);

      // Free encoded signatures
      for (int i = 0; i < NUM_DATA_POINTS; i++)
      {
        free(master_decoded_sig_buf[i]);
      }
    }

    /* Send to server */
    if (verbose)
      printf("Sending request to server...\n");
    int curl_res;
    if (!raw_mode)
    {
      curl_res = curl_to_server("http://129.242.236.85:12345/new", json_obj);
      if (curl_res != 0)
      {
        fprintf(stderr, "Failed to curl to server\n");
        cJSON_Delete(json_obj);
        free(data_points);
        return -1;
      }
    }
    else
    {
      curl_res = curl_to_server("http://129.242.236.85:12345/raw", json_obj);
      if (curl_res != 0)
      {
        fprintf(stderr, "Failed to curl to server\n");
        cJSON_Delete(json_obj);
        free(data_points);
        return -1;
      }
    }

    /* Print end-to-end metrics if in test mode */
    if (test_mode)
    {
      clock_t end_end_to_end = clock();
      printf("-------End to end latency metrics-------\n");
      printf("End to end latency: %f\n", calculate_latency(end_end_to_end - start_end_to_end, 1));
    }

    /* Cleanup for this iteration */
    cJSON_Delete(json_obj);
    free(data_points);

    iterations++;

    if (verbose && iterations % 100 == 0)
    {
      printf("Completed %d/%d iterations\n", iterations, iterations_count);
    }
  }

  /* Final cleanup */
  if (!raw_mode)
  {
    free(pk_b64);
    g2_free(pk);
    bn_free(sk);
    relic_cleanup();
  }

  return 0;
}