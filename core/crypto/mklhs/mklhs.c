#include "mklhs.h"

int gen_keys(bn_t sk, g2_t pk)
{
    bn_null(sk);
    bn_new(sk);
    g2_null(pk);
    g2_new(pk);

    int res = cp_mklhs_gen(sk, pk);
    if (res != RLC_OK)
    {
        return -1;
    }
    return 0;
}

int sign_data_points(message_t *message, bn_t sk, size_t num_data_points)
{
    /* Sign all the datapoints */
    for (int i = 0; i < num_data_points; i++)
    {
        int res_sign = cp_mklhs_sig(message->sigs[i], message->data_points[i], message->data_set_id, message->ids[0], message->tags[i], sk);
        if (res_sign != RLC_OK)
        {
            fprintf(stderr, "Could not sign message\n");
            return -1;
        }
    }
    return 0;
}
