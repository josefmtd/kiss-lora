# client_new.c

Program ini dibuat berdasarkan program daemon beacon.c yang tersedia dari library ax25-tools:

```
# apt-get install ax25-tools
```

Alur program ini adalah sebagai berikut:

1. Identifikasi port AX.25 yang tersedia, ambil data callsign dari port AX.25 yang terdapat pada Interface Linux

2. Konversi callsign berupa string sesuai dengan standar addressing pada AX.25

3. Menjalankan fungsi daemon

4. Looping program socket, pengiriman data, dan close socket setiap interval tertentu.

## penjelasan program

Program ini memerlukan library:
```
#include <sys/types.h>
#include <sys/socket.h>
#include <netax25/ax25.h>
#include <netax25/axlib.h>
#include <netax25/axconfig.h>
#include <netax25/daemon.h>

#include <config.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>
```

Fungsi-fungsi dasar yang menjadi dasar pada program ini adalah:
1. getopt(argc, argv, optstring), untuk mendapatkan argument optional yang ada pada sebuah fungsi UNIX.
2. ax25_config_load_ports(void), untuk menguji ketersediaan port AX.25 pada sistem
3. ax25_config_get_addr(port), untuk mendapatkan port callsign dari sebuah port ax25
4. ax25_aton(port_call, &client_address), untuk mengubah callsign dalam format ASCII ke spesifikasi network AX.25
5. ax25_aton(server_call, &server_address), untuk mengubah callsign dalam format ASCII destination dengan digipeater ke spesifikasi network AX.25
6. socket(AF_AX25, SOCK_SEQPACKET, 0), untuk membuat sebuah socket dengan connection mode
7. bind(socket_file_descriptor, (struct sockaddr*)&client_address, client_address_length), untuk memberi address socket sesuai dengan address client
8. connect(socket_file_descriptor, (struct sockaddr*)&server_address, server_address_length), untuk menghubungkan socket client dengan socket server
9. write(socket_file_descriptor, message, strlen(message)), untuk mengirimkan pesan melalui protokol ax.25 connection mode;
