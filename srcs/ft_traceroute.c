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

	// struct timespec time_start, time_end;
	socklen_t addr_len = sizeof(*addr_con);
	// long double rtt_msec;

    for (uint16_t ttl = 1; ttl <= 64; ttl++) {

        for (uint16_t i = 0; i < 3; i++) {
            
            t_icmp_pckt pckt;
            char buffer[sizeof(struct iphdr) + sizeof(t_icmp_pckt)];

            init_icmp_pckt(&pckt, data, ttl);

            struct timespec time_start, time_end;
            clock_gettime(CLOCK_MONOTONIC, &time_start);

            if (sendto(data->sockfd, &pckt, sizeof(t_icmp_pckt), 0, (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0)
                error_exit_program(data, "sendto error");

            if (recvfrom(data->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)addr_con, &addr_len) <= 0)
                    error_exit_program(data, "recvfrom error");
            
            long double rtt_msec = get_ping_duration(&time_start, &time_end);
            t_icmp_pckt rcvd_pckt;
            struct iphdr *ip_hdr = (struct iphdr *)buffer;
            struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));
            
            memcpy(&rcvd_pckt.hdr, icmp_hdr, sizeof(struct icmphdr));
            memcpy(rcvd_pckt.payload, buffer + (ip_hdr->ihl * 4) + sizeof(struct icmphdr), PAYLOAD_SIZE);

            if (i == 0)
                printf("  %d    %s", ttl, inet_ntoa(*(struct in_addr *)&ip_hdr->saddr));
            printf("  %.3Lfms", rtt_msec);
            if (i == 2)
                printf("\n");

            if (i == 2 && !strcmp(data->ip_addr, inet_ntoa(*(struct in_addr *)&ip_hdr->saddr)))
                return;
        }
        
    }
    
}

int main(int ac, char **av) {

	t_data				data;
	struct sockaddr_in	addr_con;

	signal(SIGINT, sig_handler);
	check_args_count(ac, av);
	init_data(&data, ac, av);
	init_sock_addr(&addr_con, data.ip_addr);
	print_traceroute_first_output(&data);

	// while (!c_sig) {
		trace_pckt_route(&data, &addr_con);
		// usleep(1 * 1000000);
	// }

	// traceroute_exit_output(&data);
	// free(data.rtt_arr);
	close(data.sockfd);
}