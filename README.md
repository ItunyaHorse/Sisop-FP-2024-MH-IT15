#  Sisop-FP-2024-MH-IT15 

Anggota:
    1. Michael Kenneth Salim (5027231008)
    2. Nicholas Emanuel Fade (5027231070)
    3. Veri Rahman (5027231088)

#### Dikerjakan oleh Michael Kenneth Salim 5027231008

### discorit.c

```
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
    char room[1024] = "";

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

    FILE* channel_ptr = fopen("/home/mken/SISOPraktikum/DiscordIT/DiscorIT/channels.csv", "r");
    if (channel_ptr == NULL) {
        channel_ptr = fopen("/home/mken/SISOPraktikum/DiscordIT/DiscorIT/channels.csv", "w");
    }
    fclose(channel_ptr);

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
            printf("[%s/%s/%s]: ", Insert, room, Lokasi);
        } 
        else if (strlen(room) > 0) {
            printf("[%s/%s]: ", Insert, room);
        } 
        else {
            printf("[%s]: ", Insert);
        }
        
        fgets(Input, sizeof(Input), stdin);
        Input[strcspn(Input, "\n")] = 0;
        send(sock, Input, strlen(Input), 0);
        bzero(buffer, sizeof(buffer));
        recv(sock, buffer, sizeof(buffer), 0);

        if (strncmp(Input, "JOIN ", 5) == 0) {
            if (strlen(room) == 0) {
                sscanf(buffer, "JOINED CHANNEL %s", room);
            } 
            else {
                sscanf(buffer, "JOINED ROOM %s", Lokasi);
            }
        } 

        else if (strncmp(Input, "CREATE ", 7) == 0) {
            send(sock, Input, strlen(Input), 0);
            bzero(buffer, sizeof(buffer));
            recv(sock, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
        }

        else if (strcmp(Input, "EXIT") == 0) {
            if (strlen(Lokasi) > 0) {
                Lokasi[0] = '\0'; 
            } 
            else if (strlen(room) > 0) {
                room[0] = '\0';
            } 
            else {
                break;
            }
        } 
        
        else {
            send(sock, Input, strlen(Input), 0);
            bzero(buffer, sizeof(buffer));
            recv(sock, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
        }
    }
}

    close(sock);
    return 0;
}
```

Jadi, pertama-tama, program ini mendeklarasi beberapa konstanta dan variabel yang diperlukan, seperti PORT yang digunakan untuk menghubungkan ke server pada port 8080, IP yang merupakan alamat IP server (dalam hal ini adalah 127.0.0.1 yang merupakan localhost), dan BUFFER_SIZE untuk ukuran buffer yang digunakan dalam pengiriman dan penerimaan data.

Program kemudian membuat socket dengan menggunakan fungsi socket(). Jika pembuatan socket gagal, program akan mencetak pesan kesalahan dan keluar. Selanjutnya, alamat server dan port diatur dalam struktur sockaddr_in. Fungsi inet_pton() digunakan untuk mengonversi alamat IP dari string ke format yang diperlukan oleh struktur sockaddr_in. Jika konversi gagal, program akan mencetak pesan kesalahan dan keluar.

Setelah itu, program mencoba menghubungkan socket yang telah dibuat ke server menggunakan fungsi connect(). Jika koneksi gagal, program akan mencetak pesan kesalahan dan keluar.

Setelah berhasil terhubung ke server, program memeriksa keberadaan file users.csv dan channels.csv di direktori DiscorIT. Jika file tersebut tidak ada, maka program akan membuatnya.

Program kemudian mengirimkan beberapa data awal ke server, yaitu jenis perintah (Jenis), input pertama (Insert), dan input kedua (Insert2). Data ini dikirim menggunakan fungsi send(), dan balasan dari server diterima menggunakan fungsi recv().

Jika perintah yang dikirim adalah LOGIN, program akan masuk ke dalam loop utama untuk menerima dan mengirim perintah dari dan ke server. Dalam loop ini, program akan mencetak prompt yang sesuai dengan status saat ini (misalnya, apakah pengguna berada di dalam room atau channel). Pengguna dapat memasukkan perintah seperti JOIN untuk bergabung dengan channel atau room, CREATE untuk membuat channel atau room, atau EXIT untuk keluar dari room atau channel. Setiap perintah akan dikirim ke server dan balasan dari server akan dicetak ke layar.

Program akan terus berjalan sampai pengguna memasukkan perintah EXIT ketika berada di luar room dan channel. Akhirnya, socket akan ditutup dengan menggunakan fungsi close() dan program selesai.

### server.c

```
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

void penuliscsv(int lastid, const char *nama, const char *pass, const char *role, const char *path) {
    FILE* file_ptr = fopen(path, "a");
    if (file_ptr == NULL) {
        perror("Failed to open csv file");
        return;
    }

    fprintf(file_ptr, "%d,%s,%s,%s\n", lastid, nama, pass, role);
    fclose(file_ptr);
}
```

1. Fungsi Penulisan dan Pembacaan CSV:
Terdapat beberapa fungsi yang berguna untuk menulis dan membaca file CSV, seperti write_channel_csv yang menulis informasi channel baru ke file CSV channel, dan get_id yang membaca file CSV untuk mendapatkan ID terakhir yang digunakan dan kemudian menambahkannya untuk mendapatkan ID baru. Fungsi penuliscsv digunakan untuk menambahkan data pengguna baru ke file CSV pengguna.

```
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
```

2. Pembuatan Channel dan Room:
Fungsi create_channel digunakan untuk membuat direktori baru untuk channel yang baru dibuat serta menulis data channel baru ke file CSV. Fungsi ini juga membuat direktori admin di dalam channel dan file auth.csv yang berisi informasi pengguna yang membuat channel tersebut. Fungsi create_room digunakan untuk membuat direktori room di dalam channel yang sudah ada serta membuat file chat.csv di dalam room tersebut untuk menyimpan pesan-pesan chat.


```
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
```
3. Manajemen Pesan Chat:
Fungsi add_chat_message menambahkan pesan baru ke dalam file chat.csv di room yang sesuai, sementara see_chat_messages membaca dan menampilkan semua pesan dari file tersebut. Fungsi edit_chat_message mengedit pesan yang sudah ada berdasarkan ID pesan, dan delete_chat_message menghapus pesan dari file chat.csv.


```
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
```

4. Fungsi Utama dan Server Socket:
Fungsi main mengatur socket server, menerima koneksi dari klien, dan menangani berbagai perintah yang diterima. Setelah socket server dibuat dan di-bind ke alamat yang sesuai, server mulai mendengarkan koneksi masuk. Server kemudian berjalan sebagai daemon, memastikan proses berjalan di latar belakang tanpa terikat ke terminal.

```
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

```

5. Proses Registrasi dan Login Pengguna:
Ketika server menerima perintah registrasi (REGISTER), ia akan memeriksa apakah username sudah ada dalam file CSV pengguna. Jika tidak ada, pengguna baru ditambahkan. Untuk login (LOGIN), server memeriksa apakah username dan password sama dengan data dalam file CSV.

```
else if (strcmp(jenis, "CHAT ") == 0 || strcmp(jenis, "SEE CHAT") == 0 || strncmp(jenis, "EDIT CHAT", 9) == 0 || strncmp(jenis, "DEL CHAT", 8) == 0) {
                            terima(new_socket, buffer, nama);
                            char *command = strtok(buffer, " ");
                            char *argument = strtok(NULL, "");

                            handle_chat(new_socket, room, Lokasi, nama, command, argument);
                        }
```

6. Penanganan Perintah Chat:
Ketika server menerima perintah terkait chat (misalnya, CHAT, SEE CHAT, EDIT CHAT, DEL CHAT), ia akan memanggil fungsi yang sesuai untuk menangani perintah tersebut, seperti menambahkan pesan, melihat semua pesan, mengedit pesan, atau menghapus pesan. Server juga dapat menerima perintah untuk membuat channel atau room baru, serta untuk bergabung ke channel atau room yang sudah ada.

```
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
```

7. Pengelolaan Direktori:
Fungsi directory_exists memeriksa apakah direktori tertentu ada, dan list_directory mengembalikan daftar semua direktori (channel atau room) yang ada di dalam jalur yang diberikan.

### monitor.c
Belum diselesaikan

### Kendala
1. Input yang tidak diterima di server.c
2. Fitur Chat yang kurang responsif

### Revisi
1. Penyelesaian fungsi chat
