#include "testing.h"

// Start timing and return the start time
struct timeval timer_start()
{
    struct timeval start;
    gettimeofday(&start, NULL);
    return start;
}

// End timing and print the result
void timer_end(struct timeval start, const char *test_name)
{
    struct timeval end;
    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_usec - start.tv_usec) / 1000000.0;
    double elapsed_ms = elapsed * 1000; // Convert to milliseconds
    char filename[sizeof(test_name) + 24];
    snprintf(filename, sizeof(filename), "data/%s.csv", test_name);
    FILE *file = fopen(filename, "a");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file %s\n", filename);
        return;
    }
    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0)
    {
        fprintf(file, "Execution Time(ms)\n");
    }
    fprintf(file, "%f\n", elapsed_ms);
    fclose(file);
}
