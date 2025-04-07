#ifndef BASE64_H
#define BASE64_H

/**
 * @brief Decodes a base64 encoded string into a byte array.
 *
 * @param input The base64 encoded input string to decode
 * @param length The length of the input string
 * @param decoded_length Pointer to store the length of the decoded output
 * @return char* Pointer to the decoded byte array (must be freed by caller),
 *         or NULL if decoding fails
 */
char *base64_decode(const char *input, int length, int *decoded_length);

/**
 * @brief Encodes a string into Base64 format
 *
 * This function takes a string input and converts it to its Base64 encoded representation.
 * The resulting string is allocated dynamically and needs to be freed by the caller.
 *
 * @param input The string to be encoded
 * @param length The length of the input string
 * @return char* A newly allocated string containing the Base64 encoded result.
 *               The caller is responsible for freeing this memory.
 *               Returns NULL if memory allocation fails.
 */
char *base64_encode(const char *input, int length);

#endif 