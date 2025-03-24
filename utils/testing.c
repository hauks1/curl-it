#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include "testing.h"

/* ------- Throughput -----------*/
double calculate_throughput_kb(size_t size, double time_taken){
    return (size/time_taken) / KB;
}
double calculate_throughput_op(int num_op, double time_taken){
    return (num_op/time_taken);
}

/* ------- Latency -----------*/
void print_latency_metrics(latency_metrics_t metrics){
    printf("Stage: %s\n",metrics.stage_name);
    printf("Total operations %d\n",metrics.num_operations);
    printf("Latency per ms: %f\n",metrics.latency_per_ms);
    printf("Latency per op: %f\n",metrics.latency_per_op);
}
double calculate_latency(clock_t cycles, double num_op){
    return (double)cycles*num_op;
}
/* Only giving me the latency in ms after*/
metrics_t get_latency_metrics(clock_t start, clock_t end, char *stage_name){
    metrics_t metrics;
    metrics.latency = (double)(end - start); //clock cycles
    metrics.latency_ms = (double)(end - start) *MS / CLOCKS_PER_SEC; // total latency in ms 
    strncpy(metrics.operation_name,stage_name,sizeof(metrics.operation_name));
    return metrics;
}
metrics_t get_metrics(clock_t start, clock_t end, size_t size,char *operation_name,test_config_t test_config){
    metrics_t metrics;
    metrics.latency_ms = (double)(end - start) *MS / CLOCKS_PER_SEC; // total latency in ms 
    metrics.latency_per_data_point = metrics.latency_ms / test_config.num_data_points; // average latency per operation
    metrics.operations_per_second = calculate_throughput_op(test_config.num_data_points,metrics.latency_ms);
    metrics.kb_per_second = calculate_throughput_kb(size,metrics.latency_ms);
    metrics.total_bytes = size;
    strncpy(metrics.operation_name, operation_name, sizeof(metrics.operation_name)-1);
    metrics.operation_name[sizeof(metrics.operation_name)-1] = '\0';
    metrics.num_operations = test_config.num_data_points;
    metrics.total_time_ms = metrics.latency_ms;
    return metrics;
}
void print_metric(metrics_t metrics){
    printf("Operation: %s\n",metrics.operation_name);
    printf("Total latency: %f\n",metrics.latency_ms);
    printf("Latency per data point: %f\n",metrics.latency_per_data_point);
    printf("Operations per second: %f\n",metrics.operations_per_second);
    printf("KB per second: %f\n",metrics.kb_per_second);
    printf("Total bytes: %ld\n",metrics.total_bytes);
}   
void print_config(test_config_t test_config){
    printf("Number of data points: %ld\n",test_config.num_data_points);
    printf("Number of messages: %ld\n",test_config.num_messages);
    printf("Scale: %ld\n",test_config.scale);
}

/* ------- Memory -----------*/

/* ------- Log to file -----------*/
int log_metrics_to_csv(test_config_t *test_config, metrics_t *metrics){

    char filename[sizeof(metrics->operation_name)+ 9];
    if (!test_config->is_sig){
        sprintf(filename,"%s_raw.csv",metrics->operation_name);
    }
    else{
        sprintf(filename,"%s.csv",metrics->operation_name);
    }
    // Format file to csv file name based on operation
    FILE *file = fopen(filename,"a");
    if(!file){
        fprintf(stderr,"Failed to open file\n");
        return -1;
    }
    fseek(file,0,SEEK_END);
    if(ftell(file) == 0){
        // Format the csv header with operation metrics given name 
        fprintf(file, "Latency (ms),Latency/Data Point (ms/data_point),Operations/Milliseconds,KB/s,Total Bytes\n");
    }
    // Log metrics to file
    fprintf(file,"%f,%f,%f,%f,%ld\n",metrics->latency_ms,metrics->latency_per_data_point,metrics->operations_per_second,metrics->kb_per_second,metrics->total_bytes);
    fclose(file);
    return 0;
}
int log_latency_metrics_to_csv(test_config_t *test_config, metrics_t *metrics){
    char filename[sizeof(metrics->operation_name)+ 9];
    if (!test_config->is_sig){
        sprintf(filename,"%s_raw.csv",metrics->operation_name);
    }
    else{
        sprintf(filename,"%s.csv",metrics->operation_name);
    }
    // Format file to csv file name based on operation
    FILE *file = fopen(filename,"a");
    if(!file){
        fprintf(stderr,"Failed to open file\n");
        return -1;
    }
    fseek(file,0,SEEK_END);
    if(ftell(file) == 0){
        // Format the csv header with operation metrics given name 
        fprintf(file, "Latency (ms), Latency (clock ticks)\n");
    }
    // Log metrics to file
    fprintf(file,"%f,%f\n",metrics->latency_ms,metrics->latency);
    fclose(file);
    return 0;
}



