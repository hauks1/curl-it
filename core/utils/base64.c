#include "base64.h"
#include <stdlib.h>

char base64_dectable[256];
char base64_enctable[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                          'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                          'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                          'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                          'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                          'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                          'w', 'x', 'y', 'z', '0', '1', '2', '3',
                          '4', '5', '6', '7', '8', '9', '+', '/'};

size_t base64_out_len(size_t in_len) { return 4 * ((in_len + 2) / 3); }

void base64_build_dectable()
{
    // Initialize the decoding table
    for (int i = 0; i < 256; i++)
    {
        base64_dectable[i] = -1;
    }
    for (int i = 0; i < 64; i++)
    {
        base64_dectable[(unsigned char)base64_enctable[i]] = i;
    }
}

char *base64_enc(char *data, size_t input_length, size_t *output_length)
{
    *output_length = base64_out_len(input_length);
    char *encoded = malloc(*output_length + 1);
    if (encoded == NULL)
    {
        return NULL;
    }

    // Divide the input into 3 byte blocks
    for (int i = 0; i < input_length; i += 3)
    {
        // Get the 3 bytes
        unsigned char b3[3];
        b3[0] = data[i];
        b3[1] = (i + 1 < input_length) ? data[i + 1] : 0;
        b3[2] = (i + 2 < input_length) ? data[i + 2] : 0;
        // Combine 3 bytes into a 24 bit number
        unsigned int b24 = (b3[0] << 16) + (b3[1] << 8) + b3[2];
        // Encode the 24 bit number into 4 base64 characters
        encoded[i / 3 * 4] = base64_enctable[(b24 >> 18) & 0x3F];
        encoded[i / 3 * 4 + 1] = base64_enctable[(b24 >> 12) & 0x3F];
        encoded[i / 3 * 4 + 2] = (i + 1 < input_length) ? base64_enctable[(b24 >> 6) & 0x3F] : '=';
        encoded[i / 3 * 4 + 3] = (i + 2 < input_length) ? base64_enctable[b24 & 0x3F] : '=';
    }

    // Null-terminate the encoded string
    encoded[*output_length] = '\0';
    return encoded;
}