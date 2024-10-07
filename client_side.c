#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

    // Proceed upload to server
    double uploadSpeed = 0.0;
        
    // Handle download from server
    
    // Receive download speed
    double downloadSpeed = 0.0;

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
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address
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
// Main
//

int main()
{
    // Create client thread
    pthread_t clt;
    if (pthread_create(&clt, NULL, createClient, NULL) != 0) {
        perror("Error: Could not create Client side");
        exit(EXIT_FAILURE);
    }
    
    // Join thread
    pthread_join(clt, NULL);
    
   return EXIT_SUCCESS;
}