/**
 * @file send.h
 * @brief Header file for sending requests to a server
 *
 * This header file contains function declarations and type definitions
 * for sending requests to a server using sockets and libcurl.
 *
 * @note This file is part of the client core module.
 *
 */
#ifndef SEND_H
#define SEND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "../utils/bad_string.h"

#define BUFFER_SIZE 4096 * 2
#define SERVER_PORT 12345
#define SERVER_IP "129.242.236.85"
#define LOCAL_SERVER_IP "127.0.0.1"

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

#endif