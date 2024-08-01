#include "../includes/ft_traceroute.h"

volatile sig_atomic_t c_sig = 0;

void sig_handler(int _)
{
	(void)_;
	c_sig = 1;
}

void check_args_count(int ac, char **av) {

	if (ac < 2) {
		fprintf(stderr, "not enough arguments\n");
		exit(EXIT_FAILURE);
	}

	for (uint8_t i = 1; i < ac; i++) {
		if (!strcmp(av[i], "-?") || !strcmp(av[i], "--help")) {
			// help_option_exec();
			exit(EXIT_SUCCESS);
		}
	}
}

void print_traceroute_first_output(t_data *data) {
	
		printf("traceroute to %s (%s), 64 hops max\n", data->dns_name,
				data->ip_addr);
    (void)data;
}

void trace_pckt_route(t_data *data, struct sockaddr_in *addr_con) {

	// socklen_t addr_len = sizeof(*addr_con);

    for (uint16_t ttl = 1; ttl <= 64; ttl++) {

			t_icmp_pckt pckt;
			char buffer[sizeof(struct iphdr) + sizeof(t_icmp_pckt)];
			struct timespec time_start, time_end;

			init_icmp_pckt(&pckt, data, ttl);
			memset(buffer, 0, sizeof(buffer));
            clock_gettime(CLOCK_MONOTONIC, &time_start);
            if (sendto(data->sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0)
                error_exit_program(data, "sendto error");

			struct sockaddr_in addr_;
	        socklen_t addr_len = sizeof(addr_);

            if (recvfrom(data->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr_, &addr_len) <= 0)
                    error_exit_program(data, "recvfrom error");
            
            long double rtt_msec = get_ping_duration(&time_start, &time_end);
            struct iphdr *ip_hdr = (struct iphdr *)buffer;
            
            printf("  %d    %s", ttl, inet_ntoa(*(struct in_addr *)&ip_hdr->saddr));
        	printf("  %.3Lfms", rtt_msec);
            printf("\n");	        
    }
    
}

int main(int ac, char **av) {

	t_data				data;
	struct sockaddr_in	addr_con;

	signal(SIGINT, sig_handler);
	check_args_count(ac, av);
	init_data(&data, ac, av);
	init_sock_addr(&data, &addr_con, data.ip_addr);
	print_traceroute_first_output(&data);

	// while (!c_sig) {
		trace_pckt_route(&data, &addr_con);
		// usleep(1 * 1000000);
	// }

	// traceroute_exit_output(&data);
	// free(data.rtt_arr);
	close(data.sockfd);
}