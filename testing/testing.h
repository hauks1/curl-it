#ifndef TESTING_H
#define TESTING_H

#include <stdio.h>
#include <sys/time.h>

#include "relic/relic.h"

struct timeval timer_start();
void timer_end(struct timeval start, const char *test_name);

#endif