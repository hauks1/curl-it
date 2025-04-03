/** @brief   */
#ifndef SEND_H
#define SEND_H
#include <cjson/cJSON.h>

#define BUFFER_SIZE 4096 * 2
#define MAX_STRDUP_SIZE 1024
#define MAX_PATH_SIZE 8
#define SERVER_IP "129.242.236.85"
#define SERVER_PORT 12345

/**
 * @brief Struct to hold request information
 *
 * This struct contains the necessary information to send a request to the server.
 * It includes the host, port, method, path, headers, content type,
 * content length, and data to be sent.
 */
typedef struct request
{
    int socket;
    char host[48];
    int port;
    char method[8];
    char path[10];
    char content_type[32];
    size_t content_length;
    char data[BUFFER_SIZE];
} request_t;

// Enum for errors
typedef enum
{
    REQUEST_SUCCESS = 0,
    REQUEST_FAILURE = -1,
} request_error_t;

// // request buffer
// static char request_buffer[BUFFER_SIZE];
/**
 * @brief Sends JSON data to a specified server URL using libcurl
 *
 * This function initializes a curl session and sends a POST request with JSON data
 * to the specified URL. It handles the HTTP headers, data serialization, and cleanup.
 *
 * @param url The destination URL where the JSON data will be sent
 * @param json A pointer to a cJSON object containing the data to be sent
 *
 * @return Returns 0 on successful transmission, -1 on failure
 *         (e.g., curl initialization failure or transmission error)
 *
 * @note The function automatically handles memory management for curl resources
 *       and JSON string conversion
 */
int curl_to_server(const char *url, cJSON *json);

/**
 * @brief Finds the path in the URL
 *
 * This function extracts the path from the given URL.
 *
 * @param path A pointer to a buffer where the extracted path will be stored
 * @param url The URL from which to extract the path
 *
 * @return Returns 0 on success, -1 on failure
 */
int find_path(char *path, const char *url);

/**
 * @brief Parses the HTTP body from the response
 *
 * This function parses the HTTP body from the given response.
 *
 * @param body A pointer to a buffer where the parsed body will be stored
 * @param response The HTTP response string
 *
 * @return Returns 0 on success, -1 on failure
 */
int parse_http_body(char *body, const char *response);

/**
 * @brief Formats the HTTP header for the request
 *
 * This function formats the HTTP header for the request.
 *
 * @param request A pointer to a buffer where the formatted header will be stored
 * @param req A pointer to the request_t struct containing request information
 */
int format_http_request(char *request, request_t *req);

/**
 * @brief Sends a request to the server using a socket
 *
 * This function sends a request to the server using a socket connection.
 *
 * @param response A pointer to a buffer where the response will be stored
 * @param obj A pointer to a cJSON object containing the data to be sent
 * @param url The URL to send the request to
 * @param method The HTTP method (e.g., GET, POST)
 *
 * @return Returns 0 on success, -1 on failure
 */
int socket_to_server(char *response, cJSON *obj, const char *url, char *method);

/**
 * @brief Connects to the server using TCP/IP
 *
 * This function establishes a connection to the server using the specified host and port.
 *
 * @return Returns socket on acomplishment, -1 on failure
 */
int connect_to_server(char *server_ip, int server_port);

/**
 * @brief Sends a GET request to the specified path
 *
 * This function sends a GET request to the specified path and stores the response.
 *
 * @param response A pointer to a buffer where the response will be stored
 * @param req A pointer to the request_t struct containing request information
 * @param response_size The size of the response buffer
 *
 * @note The path is the last part of the URL, e.g., "/new" as the server is already defined
 * @return Returns 0 on success, -1 on failure
 */
int http_GET(char *response, request_t *req, size_t response_size);

/**
 * @brief Creates a POST request
 *
 * This function creates a POST request with the specified data.
 *
 * @param req A pointer to the request_t struct containing request information
 * @param sockfd The socket descriptor for the connection to the server
 * @param data The data to be sent in the POST request
 *
 * @return Returns 0 on success, -1 on failure
 */
int setup_POST(char *request, int sock, request_t *req, const char *data, char *path, char *host);

/**
 * @brief Sends a POST request to the specified path with data
 *
 * This function sends a POST request to the specified path with the provided data.
 *
 * @param socket The socket descriptor for the connection to the server
 * @param path The path to send the POST request to
 * @param data The data to be sent in the POST request
 * @param response A pointer to a buffer where the response will be stored
 * @param response_size The size of the response buffer
 *
 * @return Returns 0 on success, -1 on failure
 */
int http_POST(char *response, request_t *req, size_t response_size);

/**
 * @brief Tests the connection to the server
 *
 * This function tests the connection to the server by sending a ping GET request.
 *
 * @return Returns 0 on success, -1 on failure
 */
int test_connection();

// /* STRING HELPERS */
// /**
//  * @brief Calculates the length of a string
//  *
//  * This function calculates the length of a given string.
//  *
//  * @param s The string whose length is to be calculated
//  * @return The length of the string
//  */
// strlen(const char *s);
// /**
//  * @brief Duplicates a string into a buffer
//  *
//  * This function duplicates a given string into an allocated buffer.
//  *
//  * @param s The string to duplicate
//  * @param n The maximum number of characters to copy
//  * @return A pointer to the duplicated string in the static buffer
//  * @note The buffer has a fixed size defined by MAX_STRDUP_SIZE.
//  */
// char *strndup(const char *s, size_t n);

// /**
//  * @brief Copies a string from source to destination with a maximum length
//  *
//  * This function copies a string from the source to the destination buffer,
//  * ensuring that the destination buffer does not overflow.
//  *
//  * @param dest The destination buffer
//  * @param src The source string
//  * @param n The maximum number of characters to copy
//  * @return 0 for success, -1 for failure
//  */
// int strncpy(char *dest, const char *src, size_t n);

#endif