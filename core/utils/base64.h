#ifndef BASE64_H
#define BASE64_H

#include <stddef.h>

extern char base64_enctable[];
extern char base64_dectable[256];
void base64_build_dectable();
size_t base64_out_len(size_t in_len);
char *base64_enc(char *data, size_t input_length, size_t *output_length);

#endif