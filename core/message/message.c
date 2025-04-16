#include "message.h"

// Random string
void rand_str(char *dest, size_t length)
{
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0)
    {
        size_t index = (double)rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

int init_message(message_t *message, dig_t data_points[],
                 size_t num_data_points)
{
    if (message == NULL)
    {
        fprintf(stderr, "Message pointer is NULL\n");
        return -1;
    }
    for (int i = 0; i < num_data_points; i++)
    {
        bn_null(message->data_points[i]);
        bn_new(message->data_points[i]);
        bn_set_dig(message->data_points[i], data_points[i]);

        // Initialize signature
        g1_null(message->sigs[i]);
        g1_new(message->sigs[i]);

        // Geneterate random strings not using UUID
        char tag_str[MAX_ID_LENGTH];
        rand_str(tag_str, MAX_ID_LENGTH - 1);
        bad_strncpy(message->tags[i], tag_str, sizeof(message->tags[i]));
    }
    // Device id
    bad_strncpy(message->ids[0], DEVICE_ID, sizeof(DEVICE_ID));
    // Set data_set_id
    bad_strncpy(message->data_set_id, TEST_DATABASE, sizeof(message->data_set_id));

    return 0;
}
void print_message(message_t *msg)
{
    for (int i = 0; i < NUM_DATA_POINTS; i++)
    {
        printf("Data point: %d\n", i);
        bn_print(msg->data_points[i]);
        printf("Signature: %d\n", i);
        g1_print(msg->sigs[i]);
        printf("ID: %s\n", msg->ids[i]);
        printf("Tag: %s\n", msg->tags[i]);
    }
    printf("Data set id: %s\n", msg->data_set_id);
}
void cleanup_message(message_t *message, size_t num_data_points)
{
    if (message == NULL)
        return;

    for (size_t i = 0; i < num_data_points; i++)
    {
        bn_free(message->data_points[i]);
        g1_free(message->sigs[i]);
    }
}
int gen_dig_data_points(dig_t data_points[], size_t num_data_points)
{
    srand(time(NULL));
    int range = 40;
    int min = 2;
    for (int i = 0; i < num_data_points; i++)
    {
        data_points[i] = (dig_t)(rand() % range + min);
    }
    return 0;
}
int gen_float_data_points(double data_points[], size_t num_data_points)
{
    srand(time(NULL));
    float a = 35.0;
    // Generate random float data points from 0 to 35
    for (int i = 0; i < num_data_points; i++)
    {
        data_points[i] = ((double)rand() / (double)RAND_MAX) * a;
    }
    return 0;
}
