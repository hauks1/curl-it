#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <errno.h>

#include "send.h"
#include "../utils/bad_string.h"
#include "../utils/base64.h"
#include <stdio.h>

int curl_to_server(const char *url, cJSON *json)
{
    CURL *curl_server;
    CURLcode res_server;

    curl_server = curl_easy_init();
    if (curl_server)
    {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        char *json_str = cJSON_Print(json);
        // Specify the URL
        curl_easy_setopt(curl_server, CURLOPT_URL, url);
        // Specify the data to be sent
        curl_easy_setopt(curl_server, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl_server, CURLOPT_POSTFIELDS, json_str);
        // Perform the request
        res_server = curl_easy_perform(curl_server);
        if (res_server != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res_server));
            return -1;
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl_server);
        cJSON_free(json_str);
        return 0;
    }
    return -1;
}

int connect_to_server(char *server_ip, int server_port)
{
    struct sockaddr_in server_addr;
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("sock creation failed");
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Connection error: %s\n", strerror(errno));
        perror("Connection failed");
        printf("errno: %d\n", errno);
        return -1;
    }
    return sock;
}

int parse_http_body(char *body, const char *response)
{
    if (response == NULL)
    {
        perror("Invalid response\n");
        return -1;
    }
    // Find the start of the body
    const char *body_start = NULL;
    for (const char *ptr = response; *ptr != '\0'; ptr++)
    {
        if (ptr[0] == '\r' && ptr[1] == '\n' && ptr[2] == '\r' && ptr[3] == '\n')
        {
            body_start = ptr + 4; // Skip "\r\n\r\n"
            break;
        }
    }

    if (body_start == NULL)
    {
        perror("Invalid response format\n");
        return -1;
    }
    // Copy the responsebody to the provided buffer
    size_t body_length = strlen(body_start) - 1;
    if (body_length >= BUFFER_SIZE)
    {
        perror("Response body too large\n");
        return -1;
    }
    memcpy(body, body_start, body_length + 1);
    body[body_length] = '\0'; // Null-terminate the string
    return 0;
}

int create_GET_request(request_t *req, int sockfd, char *path, char *host)
{
    if (req == NULL)
    {
        perror("Invalid request\n");
        return -1;
    }
    // Initialize the request
    memset(req, 0, sizeof(request_t));
    // socket
    req->socket = sockfd;
    // method
    req->method[0] = 'G';
    req->method[1] = 'E';
    req->method[2] = 'T';
    req->method[3] = '\0';
    // path
    bad_strcpy(req->path, path);
    // host
    bad_strcpy(req->host, host);
    return 0;
}

int format_GET_request(char *formated_req, request_t *req)
{
    if (req == NULL)
        return -1;

    // Format the GET request
    int len;
    if ((len = snprintf(formated_req, BUFFER_SIZE, "%s %s HTTP/1.1\r\n"
                                                   "Host: %s\r\n"
                                                   "\r\n",
                        req->method, req->path, req->host)) < 0)
        return -1;
    return len;
}

int http_GET(char *response, request_t *req, size_t response_size)
{
    if (req->socket < 0)
        return -1;

    static char request[BUFFER_SIZE];

    int req_len = format_GET_request(request, req);
    if (req_len < 0)
        return -1;
    // Send request in a single operation
    if (send(req->socket, request, req_len, 0) < 0){
        printf("Send failed\n");
        return -1;

    }

    // Receive response in a single read if possible
    ssize_t bytes_received = recv(req->socket, response, response_size - 1, 0);
    if (bytes_received < 0)
    {
        perror("recv failed");
        return -1;
    }
    if (bytes_received <= 0)
        return -1;

    response[bytes_received] = '\0';

    const char *body_start = strstr(response, "\r\n\r\n");
    if (!body_start)
        return bytes_received;

    // Check for Content-Length to see if we need more data
    const char *cl_header = strstr(response, "Content-Length:");
    if (!cl_header)
        return bytes_received;

    // Parse content length
    int content_length = bad_atoi(cl_header + 16);
    size_t header_size = (body_start + 4) - response;
    size_t body_received = bytes_received - header_size;

    // If we already have the complete response, return
    if (body_received >= content_length)
        return bytes_received;

    // We need to read more data - but only the exact amount needed
    size_t remaining = content_length - body_received;
    if (remaining > response_size - bytes_received - 1)
    {
        remaining = response_size - bytes_received - 1;
    }

    // Read the exact remaining data in a single recv() call if possible
    ssize_t additional = recv(req->socket,
                              response + bytes_received,
                              remaining,
                              MSG_WAITALL); // Wait for all data

    if (additional > 0)
    {
        bytes_received += additional;
    }

    response[bytes_received] = '\0';
    return bytes_received;
}
int test_connection()
{
    // Open connection to the server
    int sock;
    if ((sock = connect_to_server(LOCAL_SERVER_IP, SERVER_PORT)) < 0)
    {
        printf("HERE\n");
        printf("Failed to connect to server1\n");
        return -1;
    }
    printf("Sock: %d\n", sock);
    // Create the request
    request_t req;
    if (create_GET_request(&req, sock, "/ping", LOCAL_SERVER_IP) != 0)
    {
        close(sock);
        return -1;
    }
    // Send the ping GET request
    char response[BUFFER_SIZE];
    int res = http_GET(response, &req, sizeof(response));
    if (res < 0)
    {
        perror("Failed to send GET request\n");
        close(sock);
        return -1;
    }
    // Look after "Pong"
    char response_body[BUFFER_SIZE];
    if (parse_http_body(response_body, response) < 0)
    {
        perror("Failed to parse response body\n");
        close(sock);
        return -1;
    }

    if (strcmp(response_body, "pong") != 0)
    {
        perror("Failed to connect to server\n");
        close(sock);
        return -1;
    }
    // Close the socket
    close(sock);
    return 0;
}

int create_POST_request(request_t *req, int sockfd, char *path, char *host, const char *data)
{
    if (req == NULL || data == NULL)
    {
        perror("Invalid request\n");
        return -1;
    }
    // Initialize the request
    memset(req, 0, sizeof(request_t));
    // socket
    req->socket = sockfd;
    // method
    req->method[0] = 'P';
    req->method[1] = 'O';
    req->method[2] = 'S';
    req->method[3] = 'T';
    req->method[4] = '\0';
    // path
    bad_strcpy(req->path, path);
    // host
    bad_strcpy(req->host, host);
    // content type
    bad_strncpy(req->content_type, "Content-Type: application/json", sizeof(req->content_type) + bad_strlen("Content-Type: application/json") + 1);
    // contetn length
    req->content_length = bad_strlen(data);
    // data
    if (req->content_length >= sizeof(req->data))
    {
        perror("Data too large\n");
        return -1;
    }
    memcpy(req->data, data, req->content_length);
    req->data[req->content_length] = '\0';
    return 0;
}
int format_POST_request(char *formated_req, request_t *req)
{
    if (req == NULL)
        return -1;

    // Format the POST request
    int res = snprintf(formated_req, BUFFER_SIZE, "%s %s HTTP/1.1\r\n"
                                                  "Host: %s\r\n"
                                                  "Content-Type: %s\r\n"
                                                  "Content-Length: %zu\r\n"
                                                  "\r\n%s",
                       req->method, req->path, SERVER_IP, req->content_type,
                       req->content_length, req->data);

    if (res < 0)
        return -1;

    // Check if truncation occurred (res >= BUFFER_SIZE)
    if ((size_t)res >= BUFFER_SIZE)
        return -1;

    return res; // Return the formatted length for more efficient send
}

int setup_POST(char *request, int sock, request_t *req, const char *data, char *path, char *host)
{
    if (req == NULL)
    {
        perror("Invalid request\n");
        return -1;
    }
    // Create the request
    if (create_POST_request(req, sock, path, host, data) != 0)
    {
        close(sock);
        return -1;
    }

    return 0;
}

int http_POST(char *response, request_t *req, size_t response_size)
{
    if (!req || req->socket < 0 || !response || response_size == 0)
        return -1;

    // Format the request
    char request[BUFFER_SIZE];
    int format_result = format_POST_request(request, req);
    if (format_result < 0)
        return -1;

    // Send request in a single operation
    if (send(req->socket, request, format_result, 0) < 0)
        return -1;

    // Receive initial response
    ssize_t bytes_received = recv(req->socket, response, response_size - 1, 0);
    if (bytes_received <= 0)
        return -1;

    response[bytes_received] = '\0';

    // Check for Content-Length to see if we need more data
    const char *body_start = strstr(response, "\r\n\r\n");
    if (!body_start)
        return bytes_received;

    const char *cl_header = strstr(response, "content-length:");
    if (!cl_header)
    {
        return bytes_received; 
    }

    // Parse content length
    int content_length = bad_atoi(cl_header + 16);
    size_t header_size = (body_start + 4) - response;
    size_t body_received = bytes_received - header_size;

    // If we already have the complete response, return
    if (body_received >= (size_t)content_length)
        return bytes_received;

    // We need more data - calculate exactly how much
    size_t remaining = content_length - body_received;
    if (remaining > response_size - bytes_received - 1)
        remaining = response_size - bytes_received - 1;

    // Read the remaining data in a single operation
    ssize_t additional = recv(req->socket,
                              response + bytes_received,
                              remaining,
                              MSG_WAITALL);

    if (additional > 0)
        bytes_received += additional;

    response[bytes_received] = '\0';
    return bytes_received;
}