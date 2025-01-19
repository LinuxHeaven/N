#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MULTICAST_GROUP "239.255.255.250" // Same multicast group as the server
#define MULTICAST_PORT 12345              // Same UDP port as the server
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int sockfd;
    struct sockaddr_in local_addr;
    struct ip_mreq multicast_request;
    char buffer[BUFFER_SIZE];
    socklen_t addrlen = sizeof(local_addr);

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) error("socket() failed");

    // Set up the local address structure
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(MULTICAST_PORT);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the local address
    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        error("bind() failed");
    }

    // Join the multicast group
    multicast_request.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_request.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
                   &multicast_request, sizeof(multicast_request)) < 0) {
        error("setsockopt() failed");
    }

    printf("Multicast client started. Waiting for packets...\n");

    while (1) {
        // Receive messages from the multicast group
        int nbytes = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, 
                              (struct sockaddr *)&local_addr, &addrlen);
        if (nbytes < 0) error("recvfrom() failed");

        buffer[nbytes] = '\0'; // Null-terminate the received message
        printf("Received: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
