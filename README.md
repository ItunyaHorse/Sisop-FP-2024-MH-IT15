```
```

Kode ini merupakan implementasi dari sebuah aplikasi klien untuk berkomunikasi dengan server melalui protokol TCP. Aplikasi ini dibuat menggunakan bahasa pemrograman C dan di dalamnya terdapat beberapa langkah penting yang dilakukan, seperti pembuatan socket, koneksi ke server, pengiriman dan penerimaan data, serta penanganan beberapa perintah khusus seperti login, bergabung dengan channel, dan membuat channel.

Pertama-tama, program ini mendefinisikan beberapa konstanta dan variabel yang diperlukan, seperti PORT yang digunakan untuk menghubungkan ke server pada port 8080, IP yang merupakan alamat IP server (dalam hal ini adalah 127.0.0.1 yang merupakan localhost), dan BUFFER_SIZE untuk ukuran buffer yang digunakan dalam pengiriman dan penerimaan data.

Program kemudian membuat socket dengan menggunakan fungsi socket(). Jika pembuatan socket gagal, program akan mencetak pesan kesalahan dan keluar. Selanjutnya, alamat server dan port diatur dalam struktur sockaddr_in. Fungsi inet_pton() digunakan untuk mengonversi alamat IP dari string ke format yang diperlukan oleh struktur sockaddr_in. Jika konversi gagal, program akan mencetak pesan kesalahan dan keluar.

Setelah itu, program mencoba menghubungkan socket yang telah dibuat ke server menggunakan fungsi connect(). Jika koneksi gagal, program akan mencetak pesan kesalahan dan keluar.

Setelah berhasil terhubung ke server, program memeriksa keberadaan file users.csv dan channels.csv di direktori /home/mken/SISOPraktikum/DiscordIT/DiscorIT/. Jika file tersebut tidak ada, program akan membuatnya.

Program kemudian mengirimkan beberapa data awal ke server, yaitu jenis perintah (Jenis), input pertama (Insert), dan input kedua (Insert2). Data ini dikirim menggunakan fungsi send(), dan balasan dari server diterima menggunakan fungsi recv().

Jika perintah yang dikirim adalah LOGIN, program akan masuk ke dalam loop utama untuk menerima dan mengirim perintah dari dan ke server. Dalam loop ini, program akan mencetak prompt yang sesuai dengan status saat ini (misalnya, apakah pengguna berada di dalam room atau channel). Pengguna dapat memasukkan perintah seperti JOIN untuk bergabung dengan channel atau room, CREATE untuk membuat channel atau room, atau EXIT untuk keluar dari room atau channel. Setiap perintah akan dikirim ke server dan balasan dari server akan dicetak ke layar.

Program akan terus berjalan sampai pengguna memasukkan perintah EXIT ketika berada di luar room dan channel. Akhirnya, socket akan ditutup dengan menggunakan fungsi close() dan program selesai.
