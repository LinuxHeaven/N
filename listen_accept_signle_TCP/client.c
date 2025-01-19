#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> // For inet_pton
#include <unistd.h>    // For close()
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Function to handle communication with the server
void func(int sockfd) {
    char buff[MAX];
    int n;

    for (;;) {
        bzero(buff, sizeof(buff)); // Clear the buffer
        printf("Enter the string (type 'exit' to quit): ");
        n = 0;

        // Read input from the user
        while ((buff[n++] = getchar()) != '\n')
            ;
        buff[n - 1] = '\0'; // Replace the newline character with null terminator

        // Send the input to the server
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff)); // Clear the buffer for the response

        // Read the server's response
        read(sockfd, buff, sizeof(buff));
        printf("From Server: %s\n", buff);

        // Break the loop if the server or client sends "exit"
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    } else {
        printf("Socket successfully created..\n");
    }
    bzero(&servaddr, sizeof(servaddr)); // Clear server address structure

    // Assign IP and PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    // Convert IP address to binary and store it in the address structure
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        exit(0);
    }

    // Connect to the server
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        exit(0);
    } else {
        printf("Connected to the server..\n");
    }

    // Function for communication with the server
    func(sockfd);

    // Close the socket
    close(sockfd);
    return 0;
}
