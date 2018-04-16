// Generic Libraries

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netax25/ax25.h>
#include <netax25/axlib.h>
#include <netax25/axconfig.h>

// Global Define

#define VERSION "0.1.0"

// Global Variables

int main(int argc, char **argv) {
	int option;
	
	while ((option = getopt(argc, argv, "b:p:vw:")) != -1) {
		switch(option) {
			case 'b':
				if (*optarg!= 'e' && *optarg != 'l') {
					fprintf(stderr,
						"call: invalid argument for option '-b'\n");
					return 1;
				}
				backoff = *optarg == 'e'; // exponential backoff = 1, linear backoff = 0
				break;
			case 'p':
				if ((paclen = atoi(optarg)) == 0) {
					fprintf(stderr,
						"call: option '-p' requires a numeric argument\n");
					return 1;
				}
				if (paclen < 1 || paclen > 500) {
					fprintf(stderr,
						"call: paclen must be between 1 and 500\n");
					return 1;
				}
				break;
			case 'v':
				printf("call: %s\n", VERSION);
				return 0;
			case 'w':
				if ((window = atoi(optarg)) == 0) {
					fprintf(stderr,
						"call: option "-w" requires a numeric argument\n");
					return 1;
				}
				if (window < 1 || window > 7) {
					fprintf(stderr,
						"call: window must be between 1 and 7 frames\n");
					return 1;
				}
				break;
			case '?':
			case ':':
				fprintf(stderr,
					"usage: call [-b l|e] [-p paclen] [-v] [-w window] port callsign [[via] digipeaters ...]\n");
				return 1;
		}
	}
	
	if (optind == argc || optind == argc - 1) {
		fprintf(stderr,
			"usage: call [-b l|e] [-p paclen] [-v] [-w window] port callsign [[via] digipeaters ...]\n");
		return 1;
	}
	port = argv[optind];
	
	if (ax25_config_load_ports() == 0) {
		fprintf(stderr, "call: no AX.25 port data configured\n");
		return 1;
	}
	
	if (ax25_config_get_addr(port) == NULL) {
		fprintf(stderr,
			"call: invalid port setting\n");
		return 1;
	}
	
	window = ax25_config_get_window(port);
	paclen = ax25_config_get_paclen(port);

}