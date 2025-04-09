#include <b64/cdecode.h>
#include <b64/cencode.h>
#include <stdlib.h>

/* B64 decode and encode */
char *base64_decode(const char *input, int length, int *decoded_length)
{
    base64_decodestate state;
    base64_init_decodestate(&state);

    char *decoded = (char *)malloc(length * 3 / 4 +
                                   1); // Allocate enough space for decoded data
    *decoded_length = base64_decode_block(input, length, decoded, &state);
    decoded[*decoded_length] = '\0'; // Null-terminate the decoded string

    return decoded;
}

char *base64_encode(const char *input, int length)
{
    base64_encodestate state;
    base64_init_encodestate(&state);

    int encoded_length = 4 * ((length + 2) / 3);        // Base64 encoded length
    char *encoded = (char *)malloc(encoded_length + 1); // +1 for null terminator
    int len = base64_encode_block(input, length, encoded, &state);
    len += base64_encode_blockend(encoded + len, &state);
    encoded[len] = '\0'; // Null-terminate the encoded string

    return encoded;
}