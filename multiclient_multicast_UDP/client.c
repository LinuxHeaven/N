#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_MSG_SIZE 1024

void DieWithError(char *errorMessage);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in multicastAddr, localAddr;
    struct ip_mreq multicastRequest;
    char buffer[MAX_MSG_SIZE];
    unsigned short multicastPort;
    char *multicastIP;
    socklen_t addrLen;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <Multicast Address> <Port>\n", argv[0]);
        exit(1);
    }

    multicastIP = argv[1];
    multicastPort = atoi(argv[2]);

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        DieWithError("socket() failed");
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(multicastPort);
    if (bind(sock, (struct sockaddr *)&localAddr, sizeof(localAddr)) < 0)
        DieWithError("bind() failed");
    multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastIP);
    multicastRequest.imr_interface.s_addr = INADDR_ANY;
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&multicastRequest, sizeof(multicastRequest)) < 0)
        DieWithError("setsockopt() failed");

    printf("Receiver ready to receive multicast messages on %s:%d...\n", multicastIP, multicastPort);
    while (1)
    {
        addrLen = sizeof(multicastAddr);
        int n = recvfrom(sock, buffer, MAX_MSG_SIZE, 0, (struct sockaddr *)&multicastAddr, &addrLen);
        if (n < 0)
            DieWithError("recvfrom() failed");

        buffer[n] = '\0';
        printf("Received message: %s\n", buffer);
    }

    close(sock);
    return 0;
}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

// ./multicast_receiver 239.255.255.250 12345