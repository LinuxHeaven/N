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
    struct sockaddr_in multicastAddr;
    char *multicastIP;
    unsigned short multicastPort;
    char *sendString;
    unsigned char multicastTTL;
    unsigned int sendStringLen;

    if ((argc < 4) || (argc > 5))
    {
        fprintf(stderr, "Usage: %s <Multicast Address> <Port> <Send String> [<TTL>]\n", argv[0]);
        exit(1);
    }

    multicastIP = argv[1];
    multicastPort = atoi(argv[2]);
    sendString = argv[3];
    
    if (argc == 5)
        multicastTTL = atoi(argv[4]);
    else
        multicastTTL = 1;
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &multicastTTL, sizeof(multicastTTL)) < 0)
        DieWithError("setsockopt() failed");
    memset(&multicastAddr, 0, sizeof(multicastAddr));
    multicastAddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, multicastIP, &multicastAddr.sin_addr) <= 0) {
        DieWithError("inet_pton() failed");
    }
    multicastAddr.sin_port = htons(multicastPort);
    sendStringLen = strlen(sendString);
    while (1)
    {
        if (sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)&multicastAddr, sizeof(multicastAddr)) != sendStringLen)
            DieWithError("sendto() sent a different number of bytes than expected");

        printf("Sent message: %s\n", sendString);
        sleep(3);
    }
    return 0;
}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

// ./multicast_sender 239.255.255.250 12345 "Hello, Multicast!" 1
