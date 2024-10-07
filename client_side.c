#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <regex.h> // for regex

#include <unistd.h> // for sleep() usleep()

#include <pthread.h>
#include <arpa/inet.h> // for socket
#include <errno.h> // for socket tracebility

#include "functions.h"

#define PORT 8080 // Port

//
// Client side
//

void *requestServer(void *arg) {
    int clientSocket = *((int *)arg);
    
    // Proceed ping test
    double averageLatency;
    if ( false == pingRequest(clientSocket, &averageLatency) ) {
        pthread_exit(NULL);
    }

    // Proceed upload to server
    double uploadSpeed = 0.0;
    if ( false == sendFile(clientSocket, &uploadSpeed) ) {
        pthread_exit(NULL);
    }
        
    // Handle download from server
    if ( false == recvFile(clientSocket) ) {
        pthread_exit(NULL);
    }
    
    // Receive download speed
    double downloadSpeed = 0.0;
    if ( recv(clientSocket, &downloadSpeed, 8, 0) < 0 ) {
        pthread_exit(NULL);
    }

    // Close socket
    close(clientSocket);
    
    // Show transmission speed
    printf("Average latency (micro second): %.2f\n", averageLatency);
    printf("Upload Speed (Mbps): %.2f\n", uploadSpeed);
    printf("Download Speed (Mbps): %.2f\n", downloadSpeed);
    puts("");

    pthread_exit(NULL);
}

void *createClient(void *arg) {
    const char* server_ip = (const char *)arg;

    int clientSocket;
    struct sockaddr_in serverAddr;
    pthread_t tid;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error: Client side could not create socket");
        pthread_exit(NULL);
    }

    // Initialize server address structure
    serverAddr.sin_family = AF_INET;
    if ( NULL == server_ip ) {
        serverAddr.sin_addr.s_addr = inet_addr("127.168.0.1");
    }
    else {
        serverAddr.sin_addr.s_addr = inet_addr(server_ip);
    }
    serverAddr.sin_port = htons(PORT);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error: Client side connection failed");
        pthread_exit(NULL);
    }

    // Create thread to send message
    if (pthread_create(&tid, NULL, requestServer, &clientSocket) != 0) {
        perror("Error: Client side could not create thread");
        pthread_exit(NULL);
    }

    // Wait for thread to complete
    pthread_join(tid, NULL);

    pthread_exit(NULL);
}

//
// Regex check
//

bool isMatch(const char* input) {
    // Define the regex pattern for IPv4
    const char* pattern = "^([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]).([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]).([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]).([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$";

    regex_t regex;
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret != 0) {
        regfree(&regex);
        return false;
    }

    ret = regexec(&regex, input, 0, NULL, 0);
    regfree(&regex);
    if (ret != 0) {
        return false;
    }
   return true;
}

//
// Main
//

int main(int argc, char *argv[])
{
    // Parse input
    const char* server_ip = NULL;
    if ( argc > 2 || ( argc == 2 && false == isMatch(argv[1]) ) ) {
        printf("Usage: ./client_side <server side ip>\n");
        exit(EXIT_FAILURE);
    }
    else {
        server_ip = argv[1];
    }

    // Create client thread
    pthread_t clt;
    if (pthread_create(&clt, NULL, createClient, (void*)server_ip) != 0) {
        perror("Error: Could not create Client side");
        exit(EXIT_FAILURE);
    }
    
    // Join thread
    pthread_join(clt, NULL);
    
   return EXIT_SUCCESS;
}