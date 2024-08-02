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
			exit(EXIT_SUCCESS);
		}
	}
}

void print_traceroute_first_output(t_data *data) {
	
		printf("traceroute to %s (%s), 64 hops max\n", data->dns_name,
				data->ip_addr);
}

static bool wait_response(t_data *data)
{
	struct timeval tv = {1, 0};

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(data->sockfd, &readfds);

	int32_t retval = select(data->sockfd + 1, &readfds, NULL, NULL, &tv);
	if (retval == -1)
		error_exit_program(data, "select error");
	else if (!retval)
		return false;
	return true;
}

static void print_route_infos(struct iphdr *ip_hdr, struct icmphdr *icmp_hdr, uint16_t ttl, uint8_t sequence, long double rtt_msec) {

	if (!sequence || (icmp_hdr->type == ICMP_ECHOREPLY && !sequence))
		printf("   %s", inet_ntoa(*(struct in_addr *)&ip_hdr->saddr));
	if (icmp_hdr->type == ICMP_TIME_EXCEEDED || icmp_hdr->type == ICMP_ECHOREPLY)
		printf("   %.3Lfms", rtt_msec);
	else 
		printf("   *");
	(void)ttl;
}

void trace_pckt_route(t_data *data, struct sockaddr_in *addr_con) {

    for (uint16_t ttl = 1; ttl <= 64; ttl++) {

		for (uint8_t sequence = 0; sequence < 3; sequence++) {

			t_icmp_pckt pckt;
			char buffer[sizeof(struct iphdr) + sizeof(t_icmp_pckt)];
			struct timespec time_start, time_end;

			init_icmp_pckt(&pckt, data, ttl);
			memset(buffer, 0, sizeof(buffer));
			clock_gettime(CLOCK_MONOTONIC, &time_start);
			if (sendto(data->sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0)
				error_exit_program(data, "sendto error");

			struct sockaddr_in rcv_addr_con;
			socklen_t addr_len = sizeof(rcv_addr_con);

			if (!sequence)
					printf("   %d", ttl);

			if (wait_response(data)) {
				if (recvfrom(data->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&rcv_addr_con, &addr_len) <= 0)
					error_exit_program(data, "recvfrom error");
					
				long double rtt_msec = get_ping_duration(&time_start, &time_end);

				t_icmp_pckt rcvd_pckt;
				struct iphdr *ip_hdr = (struct iphdr *)buffer;
				struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));

				memcpy(&rcvd_pckt.hdr, icmp_hdr, sizeof(struct icmphdr));
				memcpy(rcvd_pckt.payload, buffer + (ip_hdr->ihl * 4) + sizeof(struct icmphdr), PAYLOAD_SIZE);
			
				print_route_infos(ip_hdr, icmp_hdr, ttl, sequence, rtt_msec);	
				if (sequence == 2 && icmp_hdr->type == ICMP_ECHOREPLY) {
					printf("\n");
					return;
				}
			}
			else {
				printf("   *");
			}
		}
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