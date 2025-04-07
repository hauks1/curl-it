#include "parsing.h"
#include "base64.h"

int convert_to_g1(g1_t new_sig, char *decoded_sig, dig_t len)
{
  // Initialize the signature
  g1_null(new_sig);
  g1_new(new_sig);
  // Read the signature into the g1_t
  RLC_TRY { g1_read_bin(new_sig, (unsigned char *)decoded_sig, len); }
  RLC_CATCH_ANY { RLC_THROW(ERR_CAUGHT); }
  return RLC_OK;
}
int retrieve_json_data(parse_t *parse_data, love_data_t *love_data, cJSON *json_data)
{
  cJSON *metadata = cJSON_GetObjectItem(json_data, "metadata");
  if (metadata == NULL)
  {
    fprintf(stderr, "No metadata found\n");
    return -1;
  }
  /* Extract labels */
  cJSON *data_set_id = cJSON_GetObjectItem(metadata, "data_set_id");
  if (data_set_id == NULL)
  {
    fprintf(stderr, "No data_set_id found\n");
    return -1;
  }
  parse_data->data_set_id = data_set_id->valuestring;
  cJSON *id = cJSON_GetObjectItem(metadata, "id");
  if (id == NULL)
  {
    fprintf(stderr, "No ids found\n");
    return -1;
  }
  cJSON *tags = cJSON_GetObjectItem(metadata, "tags");
  if (tags == NULL)
  {
    fprintf(stderr, "No tags found\n");
    return -1;
  }
  parse_data->num_data_points = cJSON_GetArraySize(cJSON_GetObjectItem(metadata, "datapoints"));
  for (int i = 0; i < parse_data->num_data_points; i++)
  {
    cJSON *tag = cJSON_GetArrayItem(tags, i);
    if (tag == NULL)
    {
      fprintf(stderr, "No id found\n");
      return -1;
    }
    parse_data->ids[i] = id->valuestring;
    parse_data->tags[i] = tag->valuestring;
  }

  cJSON *mu_array = cJSON_GetObjectItem(metadata, "mu");
  if (mu_array == NULL)
  {
    fprintf(stderr, "Not found\n");
    return -1;
  }
  cJSON *t = cJSON_GetArrayItem(mu_array, 0);
  if (t == NULL)
  {
    fprintf(stderr, "No t found\n");
    return -1;
  }
  parse_data->t = t->valuestring;

  cJSON *eval_sig_array = cJSON_GetObjectItem(metadata, "eval_sig");
  if (eval_sig_array == NULL)
  {
    fprintf(stderr, "No eval_sig found\n");
    return -1;
  }
  cJSON *eval_sig = cJSON_GetArrayItem(eval_sig_array, 0);
  parse_data->eval_sig = eval_sig->valuestring;
  cJSON *public_key = cJSON_GetObjectItem(metadata, "public_key");
  if (public_key == NULL)
  {
    fprintf(stderr, "No public_key found\n");
    return -1;
  }
  parse_data->public_key = public_key->valuestring;
  cJSON *coefs = cJSON_GetObjectItem(metadata, "coefs");
  if (coefs == NULL)
  {
    fprintf(stderr, "No coefs found\n");
    return -1;
  }
  parse_data->coefs = coefs->valuestring;
  cJSON *coefs_len = cJSON_GetObjectItem(metadata, "coefs_len");
  if (coefs_len == NULL)
  {
    fprintf(stderr, "No coefs_len found\n");
    return -1;
  }
  /* Extract the LOVE data from the request */
  cJSON *love_r = cJSON_GetObjectItem(metadata, "love_r");
  if (love_r == NULL)
  {
    fprintf(stderr, "No love r found\n");
    return -1;
  }
  love_data->r_encoded = love_r->valuestring;

  cJSON *love_u1 = cJSON_GetObjectItem(metadata, "love_u1");
  if (love_r == NULL)
  {
    fprintf(stderr, "No love u1 found\n");
    return -1;
  }
  love_data->u1_encoded = love_u1->valuestring;

  cJSON *love_u2 = cJSON_GetObjectItem(metadata, "love_u2");
  if (love_r == NULL)
  {
    fprintf(stderr, "No love u2 found\n");
    return -1;
  }
  love_data->u2_encoded = love_u2->valuestring;

  cJSON *love_v2 = cJSON_GetObjectItem(metadata, "love_v2");
  if (love_r == NULL)
  {
    fprintf(stderr, "No love r found\n");
    return -1;
  }
  love_data->v2_encoded = love_v2->valuestring;

  cJSON *love_e = cJSON_GetObjectItem(metadata, "love_e");
  if (love_r == NULL)
  {
    fprintf(stderr, "No love r found\n");
    return -1;
  }
  love_data->e_encoded = love_e->valuestring;

  return 0;
}
int decode_parsed_data(crypto_data_t *crypto_data, parse_t *parsed_data, love_data_t *love_data)
{
  // Decode the public key
  int decoded_len;
  char *decoded_pk = base64_decode(
      parsed_data->public_key, strlen(parsed_data->public_key), &decoded_len);
  if (decoded_pk == NULL)
  {
    fprintf(stderr, "Failed to decode the public key\n");
    return -1;
  }
  g2_null(crypto_data->public_key);
  g2_new(crypto_data->public_key);
  g2_read_bin(crypto_data->public_key, (unsigned char *)decoded_pk, decoded_len);
  free(decoded_pk); // Free the decoded public key

  // Decode the evaluated signature
  char *decoded_eval_sig = base64_decode(
      parsed_data->eval_sig, strlen(parsed_data->eval_sig), &decoded_len);
  if (decoded_eval_sig == NULL)
  {
    fprintf(stderr, "Failed to decode the eval sig\n");
    return -1;
  }
  g1_null(crypto_data->signature);
  g1_new(crypto_data->signature);
  g1_read_bin(crypto_data->signature, (unsigned char *)decoded_eval_sig, decoded_len);
  g1_norm(crypto_data->signature, crypto_data->signature);
  free(decoded_eval_sig); // Free the decoded eval signature

  char *decoded_t = base64_decode(parsed_data->t, strlen(parsed_data->t), &decoded_len);
  if (decoded_t == NULL)
  {
    fprintf(stderr, "Failed to decode t\n");
    return -1;
  }
  bn_null(crypto_data->t);
  bn_new(crypto_data->t);
  bn_read_bin(crypto_data->t, (unsigned char *)decoded_t, decoded_len);
  free(decoded_t); // Free the decoded t value

  int decoded_coefs_len;
  char *decoded_coefs = base64_decode(parsed_data->coefs, strlen(parsed_data->coefs), &decoded_coefs_len);
  if (decoded_coefs == NULL)
  {
    fprintf(stderr, "Failed to decode the coefs\n");
    return -1;
  }
  for (int i = 0; i < parsed_data->num_data_points; i++)
  {
    bn_null(crypto_data->fs[i]);
    bn_new(crypto_data->fs[i]);
    bn_read_bin(crypto_data->fs[i], (unsigned char *)decoded_coefs, decoded_coefs_len);
    crypto_data->fsptrs[i] = &crypto_data->fs[i];
    crypto_data->flen[i] = parsed_data->num_data_points;
  }
  free(decoded_coefs); // Free the decoded coefficients

  /* LOVE DATA */
  // r
  int decoded_r_len;
  char *decoded_r = base64_decode(
      love_data->r_encoded, strlen(love_data->r_encoded), &decoded_r_len);
  if (decoded_r == NULL)
  {
    fprintf(stderr, "Failed to decode the randomness from LOVE \n");
    return -1;
  }
  bn_null(love_data->r);
  bn_new(love_data->r);
  bn_read_bin(love_data->r, (unsigned char *)decoded_r, decoded_r_len);
  free(decoded_r);
  // u1
  int decoded_u1_len;
  char *decoded_u1 = base64_decode(
      love_data->u1_encoded, strlen(love_data->u1_encoded), &decoded_u1_len);
  if (decoded_u1 == NULL)
  {
    fprintf(stderr, "Failed to decode the randomness from LOVE \n");
    return -1;
  }
  g1_null(love_data->u1);
  g1_new(love_data->u1);
  g1_read_bin(love_data->u1, (unsigned char *)decoded_u1, decoded_u1_len);
  free(decoded_u1);
  // u2
  int decoded_u2_len;
  char *decoded_u2 = base64_decode(
      love_data->u2_encoded, strlen(love_data->u2_encoded), &decoded_u2_len);
  if (decoded_u2 == NULL)
  {
    fprintf(stderr, "Failed to decode the randomness from LOVE \n");
    return -1;
  }
  g2_null(love_data->u2);
  g2_new(love_data->u2);
  g2_read_bin(love_data->u2, (unsigned char *)decoded_u2, decoded_u2_len);
  free(decoded_u2);
  // v2
  int decoded_v2_len;
  char *decoded_v2 = base64_decode(
      love_data->v2_encoded, strlen(love_data->v2_encoded), &decoded_v2_len);
  if (decoded_v2 == NULL)
  {
    fprintf(stderr, "Failed to decode the randomness from LOVE \n");
    return -1;
  }
  g2_null(love_data->v2);
  g2_new(love_data->v2);
  g2_read_bin(love_data->v2, (unsigned char *)decoded_v2, decoded_v2_len);
  free(decoded_v2);
  // e
  int decoded_e_len;
  char *decoded_e = base64_decode(
      love_data->e_encoded, strlen(love_data->e_encoded), &decoded_e_len);
  if (decoded_e == NULL)
  {
    fprintf(stderr, "Failed to decode the randomness from LOVE \n");
    return -1;
  }
  gt_null(love_data->e);
  gt_new(love_data->e);
  gt_read_bin(love_data->e, (unsigned char *)decoded_e, decoded_e_len);
  free(decoded_e);

  return 0;
}
int parse_server_response(pairing_data_t *pairing_data, const char *response, size_t response_len)
{
  if (pairing_data == NULL || response == NULL || response_len == 0)
  {
    fprintf(stderr, "Invalid arguments to parse_server_response\n");
    return -1;
  }

  // Make sure the response is big enough to contain at least one size value
  if (response_len < sizeof(uint32_t))
  {
    fprintf(stderr, "Response too small: %zu bytes\n", response_len);
    return -1;
  }

  size_t offset = 0;

  // Read c[0] size
  uint32_t c0_size;
  memcpy(&c0_size, response + offset, sizeof(uint32_t));
  offset += sizeof(uint32_t);

  // Make sure we have enough data for c[0]
  if (offset + c0_size > response_len)
  {
    fprintf(stderr, "Response truncated for c[0]\n");
    return -1;
  }

  // Read c[0]
  gt_read_bin(pairing_data->c[0], (const unsigned char *)(response + offset), c0_size);
  offset += c0_size;

  // Make sure we have enough data for c[1] size
  if (offset + sizeof(uint32_t) > response_len)
  {
    fprintf(stderr, "Response truncated for c[1] size\n");
    return -1;
  }

  // Read c[1] size
  uint32_t c1_size;
  memcpy(&c1_size, response + offset, sizeof(uint32_t));
  offset += sizeof(uint32_t);

  // Make sure we have enough data for c[1]
  if (offset + c1_size > response_len)
  {
    fprintf(stderr, "Response truncated for c[1]\n");
    return -1;
  }

  // Read c[1]
  gt_read_bin(pairing_data->c[1], (const unsigned char *)(response + offset), c1_size);
  offset += c1_size;

  // Read e[0]
  uint32_t e0_size;
  memcpy(&e0_size, response + offset, sizeof(uint32_t));
  offset += sizeof(uint32_t);

  // Make sure we have enough data for c[0]
  if (offset + e0_size > response_len)
  {
    fprintf(stderr, "Response truncated for c[0]\n");
    return -1;
  }

  // Read e[0]
  gt_read_bin(pairing_data->e[0], (const unsigned char *)(response + offset), e0_size);
  offset += e0_size;

  // Read e[0]
  uint32_t e1_size;
  memcpy(&e1_size, response + offset, sizeof(uint32_t));
  offset += sizeof(uint32_t);

  // Make sure we have enough data for c[0]
  if (offset + e1_size > response_len)
  {
    fprintf(stderr, "Response truncated for c[0]\n");
    return -1;
  }

  // Read e[1]
  gt_read_bin(pairing_data->e[1], (const unsigned char *)(response + offset), e1_size);
  offset += e1_size;

  printf("Successfully parsed %zu bytes of response data\n", offset);

  return 0;
}

void init_parsed_data(parse_t *parse_data)
{
  parse_data->data_set_id = NULL;
  for (int i = 0; i < NUM_DATAPOINTS; i++)
  {
    parse_data->ids[i] = NULL;
    parse_data->tags[i] = NULL;
  }
  parse_data->t = NULL;
  parse_data->eval_sig = NULL;
  parse_data->public_key = NULL;
  parse_data->coefs = NULL;
  parse_data->num_data_points = 0;
}
void print_parsed_data(parse_t *parse_data)
{
  printf("Parsed data:\n");
  printf("Data set id: %s\n", parse_data->data_set_id);
  printf("Number of data points: %d\n", parse_data->num_data_points);
  for (int i = 0; i < parse_data->num_data_points; i++)
  {
    printf("Tag: %s\n", parse_data->tags[i]);
  }
  printf("T: %s\n", parse_data->t);
  printf("Eval sig: %s\n", parse_data->eval_sig);
  printf("Public key: %s\n", parse_data->public_key);
  printf("Coefficients: %s\n", parse_data->coefs);
}
