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
#define CHANNEL_CSV_PATH "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/channels.csv"
#define BCRYPT_HASHSIZE 128

void write_channel_csv(int id_channel, const char *channel, const char *key) {
    FILE *file_ptr = fopen(CHANNEL_CSV_PATH, "a");
    if (file_ptr == NULL) {
        perror("Failed to open channels.csv");
        return;
    }
    fprintf(file_ptr, "%d,%s,%s\n", id_channel, channel, key);
    fclose(file_ptr);
}

int get_id(const char *path) {
    FILE *file_ptr = fopen(path, "r");
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


void create_channel(const char *channel_name, const char *channel_key, const char *creator) {
    char channel_path[1024];
    char auth[1060];
    snprintf(channel_path, sizeof(channel_path), "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/%s", channel_name);
    if (mkdir(channel_path, 0777) == -1) {
        perror("Failed to create channel directory");
        return;
    }

    char admin_folder[1050];
    snprintf(admin_folder, sizeof(admin_folder), "%s/admin", channel_path);
    if (mkdir(admin_folder, 0777) == -1) {
        perror("Failed to create channel directory");
        return;
    }

    int next_id = get_id(CHANNEL_CSV_PATH);
    write_channel_csv(next_id, channel_name, channel_key);

    sprintf(auth, "%s/auth.csv", admin_folder);
    FILE* file_ptr = fopen(auth, "r");
    if (file_ptr == NULL) {
        file_ptr = fopen(auth, "w");

        if (file_ptr == NULL) {
            perror("Failed to create auth.csv");
            return;
        }
        int auth_id = get_id(auth);
        fprintf(file_ptr, "%d,%s,%s\n", auth_id, creator, "ROOT");

    } 
    else {
        fclose(file_ptr);
        file_ptr = fopen(auth, "a");

        if (file_ptr == NULL) {
            perror("Failed to open auth.csv for appending");
            return;
        }

        int auth_id = get_id(auth);
        fprintf(file_ptr, "%d,%s,%s\n", auth_id, creator, "ADMIN");

    }
    
    fclose(file_ptr);

    printf("Channel %s created with ID %d\n", channel_name, next_id);
}

void create_room(const char *channel_name, const char *room_name) {
    char room_path[2048];
    char chat[2060];
    snprintf(room_path, sizeof(room_path), "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/%s/%s", channel_name, room_name);
    if (mkdir(room_path, 0777) == -1) {
        perror("Failed to create room directory");
        return;
    }

    sprintf(chat, "%s/chat.csv", room_path);
    FILE* file_ptr = fopen(chat, "r");
    if (file_ptr == NULL) {
        file_ptr = fopen(chat, "w");

        if (file_ptr == NULL) {
            perror("Failed to create auth.csv");
            return;
        }
    } 
    else {
        fclose(file_ptr);
        file_ptr = fopen(chat, "a");
        if (file_ptr == NULL) {
            perror("Failed to open chat.csv for appending");
            return;
        }
    }
}

void penuliscsv(int lastid, const char *nama, const char *pass, const char *role, const char *path) {
    FILE* file_ptr = fopen(path, "a");
    if (file_ptr == NULL) {
        perror("Failed to open csv file");
        return;
    }

    fprintf(file_ptr, "%d,%s,%s,%s\n", lastid, nama, pass, role);
    fclose(file_ptr);
}

void add_chat_message(const char *channel, const char *room, const char *username, const char *message) {
    char chat_path[3000];
    snprintf(chat_path, sizeof(chat_path), "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/%s/%s/chat.csv", channel, room);

    FILE *file_ptr = fopen(chat_path, "a");
    if (file_ptr == NULL) {
        perror("Failed to open chat.csv");
        return;
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date_time[20];
    strftime(date_time, sizeof(date_time), "%d/%m/%Y %H:%M:%S", tm_info);

    int last_id = get_id(chat_path);
    fprintf(file_ptr, "%s,%d,%s,%s\n", date_time, last_id, username, message);
    fclose(file_ptr);

    printf("Message added to chat: %s\n", message);
}

void see_chat_messages(const char *channel, const char *room) {
    char chat_path[3000];
    snprintf(chat_path, sizeof(chat_path), "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/%s/%s/chat.csv", channel, room);

    FILE *file_ptr = fopen(chat_path, "r");
    if (file_ptr == NULL) {
        perror("Failed to open chat.csv");
        return;
    }

    char line[3000];
    while (fgets(line, sizeof(line), file_ptr)) {
        char date_time[20], username[256], message[256];
        int id;
        sscanf(line, "%19[^,],%d,%255[^,],%[^\n]", date_time, &id, username, message);
        printf("[%s] [%d] [%s] %s\n", date_time, id, username, message);
    }
    fclose(file_ptr);
}

void edit_chat_message(const char *channel, const char *room, int message_id, const char *new_message) {
    char chat_path[3000], temp_path[3000];
    snprintf(chat_path, sizeof(chat_path), "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/%s/%s/chat.csv", channel, room);
    snprintf(temp_path, sizeof(temp_path), "%s", chat_path);

    FILE *file_ptr = fopen(chat_path, "r");
    FILE *temp_file_ptr = fopen(temp_path, "w");

    if (file_ptr == NULL || temp_file_ptr == NULL) {
        perror("Failed to open chat files");
        return;
    }

    char line[2048];
    while (fgets(line, sizeof(line), file_ptr)) {
        char date_time[20], username[256], message[256];
        int id;
        sscanf(line, "%19[^,],%d,%255[^,],%[^\n]", date_time, &id, username, message);

        if (id == message_id) {
            fprintf(temp_file_ptr, "%s,%d,%s,%s\n", date_time, id, username, new_message);
        } else {
            fprintf(temp_file_ptr, "%s,%d,%s,%s\n", date_time, id, username, message);
        }
    }

    fclose(file_ptr);
    fclose(temp_file_ptr);

    remove(chat_path);
    rename(temp_path, chat_path);
    printf("Message %d edited to: %s\n", message_id, new_message);
}

void delete_chat_message(const char *channel, const char *room, int message_id) {
    char chat_path[3000], temp_path[3000];
    snprintf(chat_path, sizeof(chat_path), "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/%s/%s/chat.csv", channel, room);
    snprintf(temp_path, sizeof(temp_path), "%s", chat_path);

    FILE *file_ptr = fopen(chat_path, "r");
    FILE *temp_file_ptr = fopen(temp_path, "w");

    if (file_ptr == NULL || temp_file_ptr == NULL) {
        perror("Failed to open chat files");
        return;
    }

    char line[2048];
    while (fgets(line, sizeof(line), file_ptr)) {
        char date_time[20], username[256], message[256];
        int id;
        sscanf(line, "%19[^,],%d,%255[^,],%[^\n]", date_time, &id, username, message);

        if (id != message_id) {
            fprintf(temp_file_ptr, "%s,%d,%s,%s\n", date_time, id, username, message);
        }
    }

    fclose(file_ptr);
    fclose(temp_file_ptr);

    remove(chat_path);
    rename(temp_path, chat_path);
    printf("Message %d deleted\n", message_id);
}

void handle_chat(int new_socket, const char *channel, const char *room, const char *username, const char *command, const char *argument) {
    char result[MAX_BUFFER_SIZE];

    if (strncmp(command, "CHAT ", 5) == 0) {
        add_chat_message(channel, room, username, argument);
        snprintf(result, sizeof(result), "Message sent: %s", argument);
    } else if (strcmp(command, "SEE CHAT") == 0) {
        see_chat_messages(channel, room);
        snprintf(result, sizeof(result), "Chat messages displayed");
    } else if (strncmp(command, "EDIT CHAT ", 10) == 0) {
        int message_id;
        char new_message[256];
        sscanf(argument, "%d %255[^\n]", &message_id, new_message);
        edit_chat_message(channel, room, message_id, new_message);
        snprintf(result, sizeof(result), "Message %d edited: %s", message_id, new_message);
    } else if (strncmp(command, "DEL CHAT ", 9) == 0) {
        int message_id;
        sscanf(argument, "%d", &message_id);
        delete_chat_message(channel, room, message_id);
        snprintf(result, sizeof(result), "Message %d deleted", message_id);
    } else {
        snprintf(result, sizeof(result), "Unknown chat command: %s", command);
    }

    send(new_socket, result, strlen(result), 0);
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

void checker(const char *path) {
    FILE *file_ptr = fopen(path, "r");
    if (file_ptr == NULL) {
        file_ptr = fopen(path, "w");
        if (file_ptr == NULL) {
            perror("Failed to create file");
            return;
        }
    }
    fclose(file_ptr);
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

    daemonize();

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        char result[3000];
        char nama[1024];
        char jenis[1024];
        char pass[1040];
        char Lokasi[1024] = "";
        char room[2048] = "";

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
                        if (strlen(room) > 0) {
                            char channel_path[2098];
                            snprintf(channel_path, sizeof(channel_path), "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/%s", room);
                            list_directory(channel_path, result);
                            send(new_socket, result, strlen(result), 0);
                        } else {
                            snprintf(result, sizeof(result), "You must join a channel first");
                            send(new_socket, result, strlen(result), 0);
                        }
                    }

                    else if (strncmp(jenis, "CREATE CHANNEL ", 15) == 0) {
                        char channel_name[256];
                    char channel_key[256];

                    char *name_start = strchr(jenis, ' ') + 1;
                    char *name_end = strstr(name_start, " -k ");

                    if (name_end == NULL) {
                        snprintf(result, sizeof(result), "Salah Format");
                        send(new_socket, result, strlen(result), 0);
                        continue;
                    }

                    int name_length = name_end - name_start;
                    strncpy(channel_name, name_start, name_length);
                    channel_name[name_length] = '\0';

                    char *password_start = name_end + 4;
                    strncpy(channel_key, password_start, sizeof(channel_key) - 1);
                    channel_key[sizeof(channel_key) - 1] = '\0';

                    create_channel(channel_name, channel_key, nama);
                    snprintf(result, sizeof(result), "Channel %s created successfully", channel_name);
                    send(new_socket, result, strlen(result), 0);
                    } 
                    
                    else if (strncmp(jenis, "CREATE ROOM ", 12) == 0) {
                    if (strlen(room) == 0) {
                        snprintf(result, sizeof(result), "You must join a channel first");
                        send(new_socket, result, strlen(result), 0);
                    } 
                    
                    else {
                        char room_name[1060];
                        sscanf(jenis + 12, "%s", room_name);
                        create_room(room, room_name);
                        snprintf(result, sizeof(result), "Room %s created successfully", room_name);
                        send(new_socket, result, strlen(result), 0);
                    }
                    }

                    else if (strncmp(jenis, "JOIN ", 5) == 0) {
                        char join_target[256];
                        sscanf(jenis + 5, "%s", join_target);

                        if (strlen(room) == 0) {
                            char channel_path[1068];
                            snprintf(channel_path, sizeof(channel_path), "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/%s", join_target);

                            if (directory_exists(channel_path)) {
                                snprintf(room, sizeof(room), "%s", join_target);
                                bzero(Lokasi, sizeof(Lokasi));
                                snprintf(result, sizeof(result), "JOINED CHANNEL %s", join_target);
                            } 
                            
                            else {
                                snprintf(result, sizeof(result), "Channel %s does not exist", join_target);
                            }
                        } 
                       
                        else if (strlen(Lokasi) == 0) {
                            char room_path[3000];
                            snprintf(room_path, sizeof(room_path), "/home/mken/SISOPraktikum/DiscordIT/DiscorIT/%s/%s", room, join_target);

                            if (directory_exists(room_path)) {
                                snprintf(Lokasi, sizeof(Lokasi), "%s", join_target);
                                snprintf(result, sizeof(result), "JOINED ROOM %s", join_target);
                            } 
                            
                            else {
                                snprintf(result, sizeof(result), "Nothing's Called %s here", room);
                            }
                        } 

                        else {
                            snprintf(result, sizeof(result), "You are already in a room. Type EXIT to leave the current room first.");
                        }

                        send(new_socket, result, strlen(result), 0);
                    }

                    else if (strcmp(jenis, "CHAT ") == 0 || strcmp(jenis, "SEE CHAT") == 0 || strncmp(jenis, "EDIT CHAT", 9) == 0 || strncmp(jenis, "DEL CHAT", 8) == 0) {
                            terima(new_socket, buffer, nama);
                            char *command = strtok(buffer, " ");
                            char *argument = strtok(NULL, "");

                            handle_chat(new_socket, room, Lokasi, nama, command, argument);
                        }
                    else if (strcmp(jenis, "EXIT") == 0) {
                        if (strlen(Lokasi) > 0) {
                            bzero(Lokasi, sizeof(Lokasi));
                            snprintf(result, sizeof(result), "Exited room, back to channel state");
                        } 
                        else if (strlen(room) > 0) {
                            bzero(room, sizeof(room));
                            snprintf(result, sizeof(result), "Exited channel, back to user state");
                        } 
                        else {
                            snprintf(result, sizeof(result), "Exited user state");
                            send(new_socket, result, strlen(result), 0);
                            continue;
                        }
                        send(new_socket, result, strlen(result), 0);
                    } 
                    else {
                        snprintf(result, sizeof(result), "Unknown command: %s", jenis);
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
