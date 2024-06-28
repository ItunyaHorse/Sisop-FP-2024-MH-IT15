#  Sisop-FP-2024-MH-IT15 

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

Kode ini merupakan implementasi dari sebuah aplikasi klien untuk berkomunikasi dengan server melalui protokol TCP. Aplikasi ini dibuat menggunakan bahasa pemrograman C dan di dalamnya terdapat beberapa langkah penting yang dilakukan, seperti pembuatan socket, koneksi ke server, pengiriman dan penerimaan data, serta penanganan beberapa perintah khusus seperti login, bergabung dengan channel, dan membuat channel.

Pertama-tama, program ini mendefinisikan beberapa konstanta dan variabel yang diperlukan, seperti PORT yang digunakan untuk menghubungkan ke server pada port 8080, IP yang merupakan alamat IP server (dalam hal ini adalah 127.0.0.1 yang merupakan localhost), dan BUFFER_SIZE untuk ukuran buffer yang digunakan dalam pengiriman dan penerimaan data.

Program kemudian membuat socket dengan menggunakan fungsi socket(). Jika pembuatan socket gagal, program akan mencetak pesan kesalahan dan keluar. Selanjutnya, alamat server dan port diatur dalam struktur sockaddr_in. Fungsi inet_pton() digunakan untuk mengonversi alamat IP dari string ke format yang diperlukan oleh struktur sockaddr_in. Jika konversi gagal, program akan mencetak pesan kesalahan dan keluar.

Setelah itu, program mencoba menghubungkan socket yang telah dibuat ke server menggunakan fungsi connect(). Jika koneksi gagal, program akan mencetak pesan kesalahan dan keluar.

Setelah berhasil terhubung ke server, program memeriksa keberadaan file users.csv dan channels.csv di direktori /home/mken/SISOPraktikum/DiscordIT/DiscorIT/. Jika file tersebut tidak ada, program akan membuatnya.

Program kemudian mengirimkan beberapa data awal ke server, yaitu jenis perintah (Jenis), input pertama (Insert), dan input kedua (Insert2). Data ini dikirim menggunakan fungsi send(), dan balasan dari server diterima menggunakan fungsi recv().

Jika perintah yang dikirim adalah LOGIN, program akan masuk ke dalam loop utama untuk menerima dan mengirim perintah dari dan ke server. Dalam loop ini, program akan mencetak prompt yang sesuai dengan status saat ini (misalnya, apakah pengguna berada di dalam room atau channel). Pengguna dapat memasukkan perintah seperti JOIN untuk bergabung dengan channel atau room, CREATE untuk membuat channel atau room, atau EXIT untuk keluar dari room atau channel. Setiap perintah akan dikirim ke server dan balasan dari server akan dicetak ke layar.

Program akan terus berjalan sampai pengguna memasukkan perintah EXIT ketika berada di luar room dan channel. Akhirnya, socket akan ditutup dengan menggunakan fungsi close() dan program selesai.
