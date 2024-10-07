#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h> // for socket
#include <errno.h> // for socket tracebility

#include <sys/time.h> // for timeval gettimeofday()

#define PING_TIME 5
#define DATA_SIZE (1024 * 1024) // 1 MBytes of data
