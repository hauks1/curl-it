#include <curl/curl.h>
#include <cjson/cJSON.h>

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