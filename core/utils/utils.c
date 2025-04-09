#include "utils.h"
#include "base64.h"

int relic_init()
{
    if (core_init() == RLC_ERR)
    {
        core_clean();
        return RLC_ERR;
    }
    if (pc_param_set_any() != RLC_OK)
    {
        core_clean();
        return RLC_ERR;
    }
    return RLC_OK;
}

int relic_cleanup()
{
    core_clean();
    return RLC_OK;
}

int convert_to_g1(g1_t new_sig, char *decoded_sig, dig_t len)
{
    // Initialize the signature
    g1_null(new_sig);
    g1_new(new_sig);
    // Read the signature into the g1_t
    RLC_TRY
    {
        g1_read_bin(new_sig, (unsigned char *)decoded_sig, len);
    }
    RLC_CATCH_ANY
    {
        RLC_THROW(ERR_CAUGHT);
    }
    return RLC_OK;
}

int encode_signatures(message_t *msg, unsigned char *master[], char *master_decoded[], int num_data_points)
{
    int sig_len = g1_size_bin(msg->sigs[0], 1);
    /* Convert the signature and encode signature */
    for (size_t i = 0; i < num_data_points; i++)
    {

        // Allocate memory for the signature buffer
        master[i] = (unsigned char *)malloc(sig_len * sizeof(unsigned char));
        if (master[i] == NULL)
        {
            fprintf(stderr, "Memory allocation failed for master_sig_buf[%zu]\n", i);
            return -1;
        }

        g1_write_bin(master[i], sig_len, msg->sigs[i], 1); // Write the signature to the byte array
        // Encode the signature
        size_t sig_len_encoded;
        master_decoded[i] = base64_enc((char *)master[i], sig_len, &sig_len_encoded);
        if (master_decoded[i] == NULL)
        {
            fprintf(stderr, "Failed to encode signature %zu\n", i);
            return -1;
        }
    }
    return 0;
}
