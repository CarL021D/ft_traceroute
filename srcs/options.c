#include "../includes/ft_traceroute.h"



void options_init(t_data *data, int ac, char **av) {

	for (uint8_t i = 1; i < (ac - 1); i += 2) {
		
		if(!av[i + 1])
			arg_error_exit_program(data);
		
		// if (!strcmp(av[i], "-v")) {
		// 	data->option.v = 1;
		// 	i--;
		// 	continue;
		// }

		// if (!strcmp(av[i], "-q")) {
		// 	data->option.q = 1;
		// 	i--;
		// 	continue;
		// }

		// if (!strcmp(av[i], "-f")) {
		// 	data->option.f = 1;
		// 	i--;
		// 	return;
		// }


		for (uint8_t j = 0; av[i + 1][j]; j++)
			if (!isdigit(av[i + 1][j]))
				arg_error_exit_program(data);

		if (!strcmp(av[i], "-m")) {
			data->option.m = atoi(av[i + 1]);
			// return;
		}

		if (!strcmp(av[i], "-q")) {
			data->option.q = atoi(av[i + 1]);
			// return;
		}

		if (!strcmp(av[i], "-f")) {
			data->option.f = atoi(av[i + 1]);
			// return;
		}


	// 	if (!strcmp(av[i], "-c")) {
	// 		data->option.c = atoi(av[i + 1]);
	// 		return;
	// 	}
	}
}

void print_man() {

    printf("TRACEROUTE                                                              User Commands\n\n"
       "NAME\n"
       "       traceroute - Trace the route to a host\n\n"
       "SYNOPSIS\n"
       "       traceroute [OPTION...] HOST\n\n"
       "DESCRIPTION\n"
       "       Print the route packets trace to network host.\n\n"
       "       -f, --first-hop=NUM\n"
       "              set initial hop distance, i.e., time-to-live\n\n"
       "       -g, --gateways=GATES\n"
       "              list of gateways for loose source routing\n\n"
       "       -I, --icmp\n"
       "              use ICMP ECHO as probe\n\n"
       "       -m, --max-hop=NUM\n"
       "              set maximal hop count (default: 64)\n\n"
       "       -M, --type=METHOD\n"
       "              use METHOD (`icmp' or `udp') for traceroute operations, defaulting to `udp'\n\n"
       "       -p, --port=PORT\n"
       "              use destination PORT port (default: 33434)\n\n"
       "       -q, --tries=NUM\n"
       "              send NUM probe packets per hop (default: 3)\n\n"
       "       --resolve-hostnames\n"
       "              resolve hostnames\n\n"
       "       -t, --tos=NUM\n"
       "              set type of service (TOS) to NUM\n\n"
       "       -w, --wait=NUM\n"
       "              wait NUM seconds for response (default: 3)\n\n"
       "       -?, --help\n"
       "              give this help list\n"
   );
}