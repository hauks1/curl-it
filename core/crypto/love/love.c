#include <relic/relic.h>

#include "love.h"
#include "../../utils/utils.h"
#include "../../utils/base64.h"

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
    // r
    size_t r_out_len;
    int r_len = bn_size_bin(love_data->r);
    unsigned char r_buf[r_len];
    bn_write_bin(r_buf, r_len, love_data->r);
    love_data->r_encoded = base64_enc((char *)r_buf, r_len, &r_out_len);
    if (love_data->r_encoded == NULL)
    {
        fprintf(stderr, "Failed to encode r\n");
        return -1;
    }
    // u1
    size_t u1_out_len;
    int u1_len = g1_size_bin(love_data->u1, 1);
    unsigned char u1_buf[u1_len];
    g1_write_bin(u1_buf, u1_len, love_data->u1, 1);
    love_data->u1_encoded = base64_enc((char *)u1_buf, u1_len, &u1_out_len);
    if (love_data->u1_encoded == NULL)
    {
        fprintf(stderr, "Failed to encode u1\n");
        return -1;
    }
    // u2
    size_t u2_out_len;
    int u2_len = g2_size_bin(love_data->u2, 1);
    unsigned char u2_buf[u2_len];
    g2_write_bin(u2_buf, u2_len, love_data->u2, 1);
    love_data->u2_encoded = base64_enc((char *)u2_buf, u2_len, &u2_out_len);
    if (love_data->u2_encoded == NULL)
    {
        fprintf(stderr, "Failed to encode u2\n");
        return -1;
    }
    // v2
    size_t v2_out_len;
    int v2_len = g2_size_bin(love_data->v2, 1);
    unsigned char v2_buf[v2_len];
    g2_write_bin(v2_buf, v2_len, love_data->v2, 1);
    love_data->v2_encoded = base64_enc((char *)v2_buf, v2_len, &v2_out_len);
    if (love_data->v2_encoded == NULL)
    {
        fprintf(stderr, "Failed to encode v2\n");
        return -1;
    }
    // e
    size_t e_out_len;
    int e_len = gt_size_bin(love_data->e, 1);
    unsigned char e_buf[e_len];
    gt_write_bin(e_buf, e_len, love_data->e, 1);
    love_data->e_encoded = base64_enc((char *)e_buf, e_len, &e_out_len);
    if (love_data->e_encoded == NULL)
    {
        fprintf(stderr, "Failed to encode e\n");
        return -1;
    }
    return 0;
}