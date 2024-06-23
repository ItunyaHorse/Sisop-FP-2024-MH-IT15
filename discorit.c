#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PORT 8080
#define IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    char *Jenis = argv[1];
    char *Insert = argv[2];
    char *Jenis2 = argv[3];
    char *Insert2 = argv[4];
    char Input[1024];
    char Lokasi[1024] = "";

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return EXIT_FAILURE;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return EXIT_FAILURE;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return EXIT_FAILURE;
    }

    // File Checker
    FILE* file_ptr = fopen("/home/mken/SISOPraktikum/DiscordIT/DiscorIT/users.csv", "r");
    if (file_ptr == NULL) {
        file_ptr = fopen("/home/mken/SISOPraktikum/DiscordIT/DiscorIT/users.csv", "w");
    }
    fclose(file_ptr);

    snprintf(buffer, sizeof(buffer), "%s", Jenis);
    send(sock, buffer, strlen(buffer), 0);

    snprintf(buffer, sizeof(buffer), "%s", Insert);
    send(sock, buffer, strlen(buffer), 0);

    snprintf(buffer, sizeof(buffer), "%s", Insert2);
    send(sock, buffer, strlen(buffer), 0);

    bzero(buffer, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    if (strcmp(Jenis, "LOGIN") == 0) {
        while (1) {
            if (strlen(Lokasi) > 0) {
                printf("[%s/%s]: ", Insert, Lokasi);
            } 
            else {
                printf("[%s]: ", Insert);
            }
            
            fgets(Input, sizeof(Input), stdin);
            Input[strcspn(Input, "\n")] = 0;

            send(sock, Input, strlen(Input), 0);
            bzero(buffer, sizeof(buffer));
            recv(sock, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);

            if (strncmp(Input, "JOIN ", 5) == 0) {
                sscanf(buffer, "JOINED %s", Lokasi);
            } 
            else if (strcmp(Input, "EXIT") == 0) {
                if (strlen(Lokasi) > 0) {
                    Lokasi[0] = '\0'; 
                } 
                else {
                    break;
                }
            }
        }
    }

    close(sock);
    return 0;
}
