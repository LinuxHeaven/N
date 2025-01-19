#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MULTICAST_GROUP "239.255.255.250" // Arbitrary multicast group
#define MULTICAST_PORT 12345              // Arbitrary UDP port
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int sockfd;
    struct sockaddr_in multicast_addr;
    char buffer[BUFFER_SIZE];
    time_t current_time;

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) error("socket() failed");

    // Set up the multicast address structure
    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(MULTICAST_PORT);
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

    printf("Multicast server started. Sending time packets...\n");

    while (1) {
        // Get the current time
        current_time = time(NULL);
        snprintf(buffer, sizeof(buffer), "Current time: %s", ctime(&current_time));

        // Send the message to the multicast group
        if (sendto(sockfd, buffer, strlen(buffer), 0, 
                   (struct sockaddr *)&multicast_addr, sizeof(multicast_addr)) < 0) {
            error("sendto() failed");
        }

        printf("Sent: %s", buffer);
        sleep(1); // Send every second
    }

    close(sockfd);
    return 0;
}
