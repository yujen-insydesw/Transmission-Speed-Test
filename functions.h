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

// Parse errno
void parseErrno() {
    switch(errno) {
    case ECONNRESET: // Connection reset by the peer
        perror("Error: connection reset by the peer\n");
    case ETIMEDOUT: // Timeout
        perror("Error: connection timeout\n");
    case ENOTCONN: // The socket is not connected.
        perror("Error: the socket is not connected\n");
    case EPIPE: // The connection has been broken
        perror("Error: connection broken\n");
    default:        // Other
        printf("Error: %s\n", strerror(errno));
    }
}

// Caculate average latency
// input: (micro second, times)
// output: micro second
const double calculateAverageLatency(const int totalPingSpan, const int times) {
    if (times > 0) {
        return totalPingSpan / times;
    } else {
        perror("Error: times must be greater than zero\n");
        return 0.0;
    }
}

// Ping send
bool pingSend(const int clientSocket) {
    const char* req = "ACK";
    if ( send(clientSocket, req, strlen(req), 0) < 0 ) {
        parseErrno();
        return false;
    }
    return true;
}

// Ping recv
bool pingRecv(const int clientSocket) {
    char* ack_buffer = (char*)malloc(4);
    if ( recv(clientSocket, ack_buffer, 4, 0) < 0 ) {
        parseErrno();
        return false;
    }
    ack_buffer[3] = '\0';
    return true;
}

// Ping test request
bool pingRequest(const int clientSocket, double* averageLatency) {
    long unsigned totalSpan = 0;

    for (int round = 0; round < PING_TIME; round++) {
        struct timeval start;
        gettimeofday(&start, NULL);

        // Ping
        if ( false == pingSend(clientSocket) ) {
            return false;
        }

        // Wait for ASK
        if ( false == pingRecv(clientSocket) ) {
            return false;
        }

        struct timeval end;
        gettimeofday(&end, NULL);

        totalSpan += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    }

    *averageLatency = calculateAverageLatency(totalSpan, PING_TIME);    
    
    return true;
}

// Ping test response
bool pingResponse(const int clientSocket) {
    for (int round = 0; round < PING_TIME; round++) {
        // Wait for ping
        if ( false == pingRecv(clientSocket) ) {
            return false;
        }

        // Send ASK
        if ( false == pingSend(clientSocket) ) {
            return false;
        }
    }
    return true;
}
