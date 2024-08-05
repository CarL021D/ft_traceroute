#include "../includes/ft_traceroute.h"

void options_init(t_data *data, int ac, char **av) {

	for (uint8_t i = 1; i < (ac - 1); i += 2) {
		
		if(!av[i + 1])
			arg_error_exit_program(data);
		
		if (!strcmp(av[i], "--resolve-hostnames")) {
			data->option.res_host_name = 1;
			i--;
			continue;
		}

		for (uint8_t j = 0; av[i + 1][j]; j++)
			if (!isdigit(av[i + 1][j]))
				arg_error_exit_program(data);

		if (!strcmp(av[i], "-m"))
			data->option.m = atoi(av[i + 1]);

		if (!strcmp(av[i], "-q"))
			data->option.q = atoi(av[i + 1]);

		if (!strcmp(av[i], "-f"))
			data->option.f = atoi(av[i + 1]);

		if (!strcmp(av[i], "-w"))
			data->option.w = atoi(av[i + 1]);
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
	   "              use ICMP ECHO as probe\n\n"
	   "       -m, --max-hop=NUM\n"
	   "              set maximal hop count (default: 64)\n\n"
	   "       -q, --tries=NUM\n"
	   "              send NUM probe packets per hop (default: 3)\n\n"
	   "       --resolve-hostnames\n"
	   "              resolve hostnames\n\n"
	   "       -w, --wait=NUM\n"
	   "              wait NUM seconds for response (default: 3)\n\n"
	   "       -?, --help\n"
	   "              give this help list\n"
   );
}