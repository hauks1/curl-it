// #include <relic/relic.h> // Include the Relic library header
// #include "request.h"
// #include "crypto.h"
// #include <sys/resource.h>
// #include <sys/types.h>
// #include <unistd.h> 
// #include <pthread.h>
// #include <stdbool.h>
// /* Struct holding relevant memory information */
// typedef struct {
//     const char* function_name;
//     long initial_rss;
//     long peak_rss;
//     long final_rss;
//     long initial_vm;
//     long peak_vm;
//     long final_vm;
//   } memory_profile_t;
  
//   int isolate_function_memory(const char* function_name, int (*func)(void*), void *args ) {
//     pid_t pid = fork();
    
//     if (pid == -1) {
//         perror("fork failed\n");
//         return -1;
//     } 
//     else if (pid == 0) { // Child process
//         pid_t child_pid = getpid();
//         printf("%s CHILD PROCESS PID: %d\n", function_name, child_pid);
        
//         // Get initial memory usage
//         char statm_path[64];
//         sprintf(statm_path, "/proc/%d/statm", child_pid);
//         FILE *statm_before = fopen(statm_path, "r");
//         long vm_before, rss_before;
//         fscanf(statm_before, "%ld %ld", &vm_before, &rss_before);
//         fclose(statm_before);
        
//         long page_size = sysconf(_SC_PAGESIZE);
//         printf("Initial memory: RSS=%ld KB, VM=%ld KB\n", 
//                (rss_before * page_size) / 1024, 
//                (vm_before * page_size) / 1024);
        
//         // Execute the function
//         int result = func(args);
        
//         // Get final memory usage
//         FILE *statm_after = fopen(statm_path, "r");
//         long vm_after, rss_after;
//         fscanf(statm_after, "%ld %ld", &vm_after, &rss_after);
//         fclose(statm_after);
        
//         printf("Final memory: RSS=%ld KB, VM=%ld KB\n", 
//                (rss_after * page_size) / 1024, 
//                (vm_after * page_size) / 1024);
//         printf("Delta: RSS=%ld KB, VM=%ld KB\n", 
//                ((rss_after - rss_before) * page_size) / 1024,
//                ((vm_after - vm_before) * page_size) / 1024);
        
//         exit(result);
//     } 
//     else { // Parent process
//         int status;
//         waitpid(pid, &status, 0);
//         return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
//     }
//   }
  
//   // Memory profiling wrappers for core functions
  
//   // Function wrappers for gen_keys
//   typedef struct {
//     bn_t *sk;
//     g2_t *pk;
//   } gen_keys_args_t;
  
//   int gen_keys_wrapper(void *args) {
//     gen_keys_args_t *params = (gen_keys_args_t *)args;
//     return gen_keys(params->sk, params->pk);
//   }
  
//   // Function wrappers for sign_data_points
//   typedef struct {
//     message_t *message;
//     bn_t *sk;
//     size_t num_data_points;
//   } sign_data_points_args_t;
  
//   int sign_data_points_wrapper(void *args) {
//     sign_data_points_args_t *params = (sign_data_points_args_t *)args;
//     return sign_data_points(params->message, params->sk, params->num_data_points);
//   }
  
//   // Function wrappers for encode_signatures
//   typedef struct {
//     message_t *message;
//     unsigned char **master_sig_buf;
//     char **master_decoded_sig_buf;
//     size_t num_data_points;
//   } encode_signatures_args_t;
  
//   int encode_signatures_wrapper(void *args) {
//     encode_signatures_args_t *params = (encode_signatures_args_t *)args;
//     return encode_signatures(params->message, params->master_sig_buf, 
//                             params->master_decoded_sig_buf, params->num_data_points);
//   }
  
//   // Function wrappers for prepare_request_server
//   typedef struct {
//     cJSON *json_obj;
//     message_t *message;
//     char **master_decoded_sig_buf;
//     dig_t *data_points;
//     size_t num_data_points;
//     char *pk_b64;
//     int sig_len;
//     uint64_t scale;
//     const char *func_name;
//   } prepare_request_args_t;
  
//   int prepare_request_wrapper(void *args) {
//     prepare_request_args_t *params = (prepare_request_args_t *)args;
//     return prepare_request_server(
//         params->json_obj, params->message, params->master_decoded_sig_buf,
//         params->data_points, params->num_data_points, params->pk_b64,
//         params->sig_len, params->scale, params->func_name
//     );
//   }
  
//   // Function wrappers for prepare_raw_req_server
//   typedef struct {
//     cJSON *json_obj;
//     raw_message_t *message;
//     dig_t *data_points;
//     size_t num_data_points;
//     uint64_t scale;
//   } prepare_raw_req_args_t;
  
//   int prepare_raw_req_wrapper(void *args) {
//     prepare_raw_req_args_t *params = (prepare_raw_req_args_t *)args;
//     return prepare_raw_req_server(
//         params->json_obj, params->message, params->data_points,
//         params->num_data_points, params->scale
//     );
//   }
  
//   // Function wrappers for curl_to_server
//   typedef struct {
//     const char *url;
//     cJSON *json_obj;
//   } curl_args_t;
  
//   int curl_to_server_wrapper(void *args) {
//     curl_args_t *params = (curl_args_t *)args;
//     return curl_to_server(params->url, params->json_obj);
//   }
  
//   // Example usage of the wrappers with the isolate_function_memory function
  
//   // For gen_keys
//   void profile_gen_keys(bn_t sk, g2_t pk) {
//     gen_keys_args_t args;
//     args.sk = sk;
//     args.pk = pk;
    
//     if(!isolate_function_memory("key_gen", gen_keys_wrapper, &args)) perror("Could not gen key isolate\n");
    
//   }
//   // For sign_data_points
//   void profile_sign_data_points(message_t *message, bn_t sk, size_t num_data_points) {
//     sign_data_points_args_t args;
//     args.message = message;
//     args.sk = sk;
//     args.num_data_points = num_data_points;
    
//     int res = isolate_function_memory("sign_data_points", sign_data_points_wrapper, &args);
    
//   }
  
//   // For encode_signatures
//   void profile_encode_signatures(message_t *message, unsigned char **master_sig_buf, 
//                               char **master_decoded_sig_buf, size_t num_data_points) {
//     encode_signatures_args_t args;
//     args.message = message;
//     args.master_sig_buf = master_sig_buf;
//     args.master_decoded_sig_buf = master_decoded_sig_buf;
//     args.num_data_points = num_data_points;
    
//     int res = isolate_function_memory("encode_signatures", encode_signatures_wrapper, &args);
    
//   }
  
//   // For prepare_request_server
//   void profile_prepare_request(cJSON *json_obj, message_t *message, char **master_decoded_sig_buf,
//                             dig_t *data_points, size_t num_data_points, char *pk_b64,
//                             int sig_len, uint64_t scale, const char *func_name) {
//     prepare_request_args_t args;
//     args.json_obj = json_obj;
//     args.message = message;
//     args.master_decoded_sig_buf = master_decoded_sig_buf;
//     args.data_points = data_points;
//     args.num_data_points = num_data_points;
//     args.pk_b64 = pk_b64;
//     args.sig_len = sig_len;
//     args.scale = scale;
//     args.func_name = func_name;
    
//     int res = isolate_function_memory("prepare_request", prepare_request_wrapper, &args);
   
//   }
  
//   // For prepare_raw_req_server
//   void profile_prepare_raw_req(cJSON *json_obj, raw_message_t *message, dig_t *data_points,
//                             size_t num_data_points, uint64_t scale) {
//     prepare_raw_req_args_t args;
//     args.json_obj = json_obj;
//     args.message = message;
//     args.data_points = data_points;
//     args.num_data_points = num_data_points;
//     args.scale = scale;
    
//     int res = isolate_function_memory("prepare_raw_req", prepare_raw_req_wrapper, &args);
//   }
  
//   // For curl_to_server
//   void profile_curl_to_server(const char *url, cJSON *json_obj) {
//     curl_args_t args;
//     args.url = url;
//     args.json_obj = json_obj;
    
//     int res = isolate_function_memory("curl_to_server", curl_to_server_wrapper, &args);
//   }