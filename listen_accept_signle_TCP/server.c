#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void func(int connfd) {
    char buff[MAX];
    int n;

    for (;;) {
        bzero(buff, MAX);
        
        // Read the message from client and copy it to the buffer
        read(connfd, buff, sizeof(buff));
        printf("From client: %s", buff);

        // If the message is "exit", terminate the connection
        if (strncmp("exit", buff, 4) == 0) {
            printf("To client: exit\n");
            write(connfd, "exit", sizeof("exit"));
            printf("Server Exit...\n");
            break;
        }

        // Send response to the client
        bzero(buff, MAX);
        strcpy(buff, "hello");
        write(connfd, buff, sizeof(buff));
        printf("To client: %s\n", buff);
    }
}

int main() {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    } else {
        printf("Socket successfully created..\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // Assign IP and port
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Bind the socket
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("Socket binding failed...\n");
        exit(0);
    } else {
        printf("Socket successfully binded..\n");
    }

    // Listen for a client
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    } else {
        printf("Server listening..\n");
    }
    len = sizeof(cli);

    // Accept a client connection
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("Server accept failed...\n");
        exit(0);
    } else {
        printf("Server accept the client...\n");
    }

    // Function for communication between client and server
    func(connfd);

    // Close the socket
    close(sockfd);
    return 0;
}
