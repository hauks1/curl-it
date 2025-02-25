#ifndef TESTING_H 
#define TESTING_H
#include <time.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>

#include "relic/relic.h"


#define MAX_ITERATIONS 10
#define KB 1024
#define MS 1000

/* Latency structure */
typedef struct {
    int num_operations;
    double latency_per_ms; // total latency in ms
    double latency_per_op; // average latency per operation
    double end_to_end_latency; 
    char stage_name[20];
}latency_metrics_t;

typedef struct metrics {
    // Latency metrics
    double latency_ms; // total latency in milliseconds
    double latency_per_data_point; // average latency per data point

    // Throughput metrics
    double operations_per_second;
    double kb_per_second;
    size_t total_bytes;

    // Operation metadata
    char operation_name[64];
    int num_operations;
    double total_time_ms;
}metrics_t;

typedef struct test_config {
    size_t num_data_points;
    size_t num_messages; // This is number of iterations in reallity... 
    uint64_t scale;
    int is_sig; // determine if the data has signature or not
}test_config_t;

/* Functions*/
metrics_t get_metrics(clock_t start, clock_t end, size_t size,char *operation_name,test_config_t test_config);
void print_metric(metrics_t metrics);
void print_config(test_config_t test_config);

/* ------TROUGHPUT--------- */
/* Calculates throughput in KB/s */
double calculate_throughput_kb(size_t size, double time_taken);
/* Calculates throughput in number of operations per seconds (num_op/sec) */
double calculate_throughput_op(int num_op, double time_taken);

/* ------LATENCY--------- */
void print_latency_metrics(latency_metrics_t metrics);
latency_metrics_t get_latency_metrics(clock_t start, clock_t end, int num_op, char *stage_name);
double calculate_latency(clock_t cycles, double num_op);



/* ------ LOG --------- */
int log_metrics_to_csv(test_config_t *test_config,metrics_t *metrics);

#endif 