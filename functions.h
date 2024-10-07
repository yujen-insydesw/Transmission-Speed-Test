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

// Caculate transfer speed
// input: (bytes , micro second)
// output: bits/second
// = (input bytes) * 8.0 * 1/1000,000 / ((duration micro second) * 1/1000,000)
// = (inout Mega bits) / (second)
const double calculateTransferSpeed(const double totalDataTransferred, const double totalTimeTaken) {
    if (totalTimeTaken > 0) {
        return totalDataTransferred * 8.0 / totalTimeTaken;
    } else {
        perror("Error: total time taken must be greater than zero\n");
        return 0.0;
    }
}

// Send data (or file) and Caculate speed
bool sendFile(const int clientSocket, double* speed) {
    // Prepare data
    char* buffer = (char*)malloc(DATA_SIZE);
    memset(buffer, 'A', DATA_SIZE);

    struct timeval start;
    gettimeofday(&start, NULL);

    // Send data (tripple round for averaging the performance)
    ssize_t bytesSend = 0;
    bytesSend += send(clientSocket, buffer, DATA_SIZE, 0);
    bytesSend += send(clientSocket, buffer, DATA_SIZE, 0);
    bytesSend += send(clientSocket, buffer, DATA_SIZE, 0);
    send(clientSocket, "V", 1, 0);
    if (bytesSend < 0) {
        parseErrno();
        return false;
    }

    printf("Send %zd bytes\n\n", bytesSend);

    // Wait for ASK
    char* ack_buffer = (char*)malloc(4);
    if ( recv(clientSocket, ack_buffer, 4, 0) < 0 ) {
        parseErrno();
        return false;
    }
    ack_buffer[3] = '\0';

    struct timeval end;
    gettimeofday(&end, NULL);

    // Caculate speed
    long unsigned duration = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    *speed = calculateTransferSpeed(bytesSend, duration);
    /*
    printf("Total time taken: %lu micro seconds\n", duration);
    printf("Speed (Mbps): %.2f\n", *speed);
    puts("");
    */

    // Release
    free(buffer);

    // Validate
    return speed > 0;
}

// Recv data (or file)
bool recvFile(const int clientSocket) {
    // Receive data
    char buffer[DATA_SIZE + 1];
    ssize_t bytesRead, totalRead;
    while ( (bytesRead = recv(clientSocket, buffer, DATA_SIZE + 1, 0)) > 0) {
        totalRead += bytesRead;
        if ('V' == buffer[bytesRead - 1]) {
            break;
        }
        // %.*s: *用来指定宽度，对应一个整数。
        //（点）与后面的数合起来是指定必须输出这个宽度，
        // 如果所输出的字符串长度大于这个数，则按此宽度输出，如果小于，则输出實際長度
        //printf("recv: %.*s\n", (int)bytesRead, buffer);
    }
    if (bytesRead < 0) {
        parseErrno();
        return false;
    }
    buffer[totalRead] = '\0';
    
    printf("Received %zd bytes\n\n", totalRead);    

    // Send ASK
    const char* ack_data = "ACK";
    send(clientSocket, ack_data, strlen(ack_data), 0);
    
    return true;
}
