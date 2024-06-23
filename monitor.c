#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>

#define MAX_BUFFER_SIZE 1024
#define PORT 8080
#define USERS_CSV_PATH "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/users.csv"

void penuliscsv(int lastid, const char *nama, const char *pass, const char *role, const char *path) {
    FILE* file_ptr = fopen(path, "a");
    if (file_ptr == NULL) {
        perror("Failed to open csv file");
        return;
    }

    fprintf(file_ptr, "%d,%s,%s,%s\n", lastid, nama, pass, role);
    fclose(file_ptr);
}

int get_next_user_id() {
    FILE *file_ptr = fopen(USERS_CSV_PATH, "r");
    if (file_ptr == NULL) {
        return 1;
    }
    char line[256];
    int last_id = 0;
    while (fgets(line, sizeof(line), file_ptr)) {
        sscanf(line, "%d", &last_id);
    }
    fclose(file_ptr);
    return last_id + 1;
}

int ganda(const char *username) {
    FILE *file_ptr = fopen(USERS_CSV_PATH, "r");
    if (file_ptr == NULL) {
        perror("Failed to open users.csv");
        return -1;
    }

    char line[1400];
    while (fgets(line, sizeof(line), file_ptr)) {
        char *cari = strtok(line, ",");
        while(cari != NULL) {
            cari = strtok(NULL, ",");
            if (cari == NULL) {
                break;
            }
            if (strcmp(cari, username) == 0) {
                fclose(file_ptr);
                return 1;
            }
        }
    }

    fclose(file_ptr);
    return 0;
}

void terima(int socket_fd, char *buffer, char *tujuan) {
    bzero(buffer, MAX_BUFFER_SIZE);

    if (recv(socket_fd, buffer, MAX_BUFFER_SIZE, 0) < 0) {
        perror("Receive failed");
        close(socket_fd);
    }

    strcpy(tujuan, buffer);
}

int directory_exists(const char *path) {
    struct stat info;

    if (stat(path, &info) != 0) {
        return 0;
    } 
    else if (info.st_mode & S_IFDIR) {
        return 1;
    } 
    else {
        return 0;
    }
}

void list_directory(const char *path, char *output_buffer) {
    struct dirent *entry;
    struct stat statbuf;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    bzero(output_buffer, MAX_BUFFER_SIZE);
    while ((entry = readdir(dp))) {
        char full_path[1024];

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (stat(full_path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                strncat(output_buffer, entry->d_name, MAX_BUFFER_SIZE - strlen(output_buffer) - 1);
                strncat(output_buffer, " ", MAX_BUFFER_SIZE - strlen(output_buffer) - 1);
            }
        }
    }

    closedir(dp);
}

void daemonize() {
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    umask(0);

    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_BUFFER_SIZE];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    } 
    else {
        printf("Server started, waiting for connections...\n");
    }

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        char result[1060];
        char nama[1024];
        char jenis[1024];
        char pass[1040];
        char Lokasi[1024] = "";

        terima(new_socket, buffer, jenis);

        if (strcmp("REGISTER", jenis) == 0) {
            int last_id = get_next_user_id();

            terima(new_socket, buffer, nama);
            int ada = ganda(nama);
            if (ada == 1) {
                snprintf(result, sizeof(result), "%s sudah terdaftar", nama);
                send(new_socket, result, strlen(result), 0);
                close(new_socket);
                continue;
            } 
            else if (ada == 0) {
                terima(new_socket, buffer, pass);
                if(last_id == 1) {
                    penuliscsv(last_id, nama, pass, "ROOT", USERS_CSV_PATH);
                    snprintf(result, sizeof(result), "%s berhasil register", nama);
                } 
                else {
                    penuliscsv(last_id, nama, pass, "ADMIN", USERS_CSV_PATH);
                    snprintf(result, sizeof(result), "%s berhasil register", nama);
                }
                send(new_socket, result, strlen(result), 0);
                close(new_socket);
                continue;
            }
        } 
        else if (strcmp("LOGIN", jenis) == 0) {
            terima(new_socket, buffer, nama);
            int ada = ganda(nama);
            terima(new_socket, buffer, pass);
            int passada = ganda(pass);
            if (ada == 0) {
                snprintf(result, sizeof(result), "Akun tidak ditemukan");
            } 
            else if (ada == 1 && passada == 0) {
                snprintf(result, sizeof(result), "Password salah");
            } 
            else if (ada == 1 && passada == 1) {
                snprintf(result, sizeof(result), "%s berhasil login", nama);
            }
            send(new_socket, result, strlen(result), 0);

            if (ada == 1 && passada == 1) {
                while (1) {
                int channels = 0;
                    terima(new_socket, buffer, jenis);
                    printf("Received command: %s\n", jenis);

                    if (strcmp(jenis, "LIST CHANNEL") == 0) {
                        list_directory("/home/mken/SISOPraktikum/DiscordIT/DiscorIT", result);
                        send(new_socket, result, strlen(result), 0);
                    } 
                    else if (strcmp(jenis, "LIST ROOM") == 0) {
                        if (strlen(Lokasi) > 0) {
                            char room_path[1070];
                            snprintf(room_path, sizeof(room_path), "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/%s", Lokasi);
                            list_directory(room_path, result);
                            send(new_socket, result, strlen(result), 0);
                        } 
                        else {
                            snprintf(result, sizeof(result), "You must join a channel first");
                            send(new_socket, result, strlen(result), 0);
                        }
                    } 
                    else if (strncmp(jenis, "JOIN ", 5) == 0) {
                        sscanf(jenis, "JOIN %s", Lokasi);
                        char channel_path[1070];
                        snprintf(channel_path, sizeof(channel_path), "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/%s", Lokasi);

                        if (directory_exists(channel_path)) {
                            channels = 1;
                            snprintf(result, sizeof(result), "JOINED %s", Lokasi);
                        } 
                        else {
                            snprintf(result, sizeof(result), "Channel %s does not exist", Lokasi);
                        }

                        send(new_socket, result, strlen(result), 0);
                    } 
                    else if (strcmp(jenis, "EXIT") == 0) {
                        if (strlen(Lokasi) > 0) {
                            bzero(Lokasi, sizeof(Lokasi));
                            snprintf(result, sizeof(result), "Exited channel, back to user state");
                        } 
                        else {
                            snprintf(result, sizeof(result), "Exited user state");
                            send(new_socket, result, strlen(result), 0);
                            break;
                        }
                        send(new_socket, result, strlen(result), 0);
                    } 
                    else {
                        snprintf(result, sizeof(result), "Unknown command: %s\n", jenis);
                        send(new_socket, result, strlen(result), 0);
                    }
                }
            }
        } 
        else {
            snprintf(result, sizeof(result), "Perintah tidak dikenal");
            send(new_socket, result, strlen(result), 0);
        }

        close(new_socket);
    }

    close(server_fd);
    return 0;
}
