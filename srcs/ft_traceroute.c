#include "../includes/ft_traceroute.h"

void check_args_count(int ac, char **av) {

	if (ac < 2) {
		fprintf(stderr, "not enough arguments\n");
		exit(EXIT_FAILURE);
	}

	for (uint8_t i = 1; i < ac; i++) {
		if (!strcmp(av[i], "-?") || !strcmp(av[i], "--help")) {
			print_man();
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
	struct timeval tv = {data->t_to_wait, 0};

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

static void print_route_infos(struct iphdr *ip_hdr, struct icmphdr *icmp_hdr, t_data *data, uint8_t sequence, long double rtt_msec) {


	if (!sequence || (icmp_hdr->type == ICMP_ECHOREPLY && !sequence)) {
	
		data->host_name = ip_to_hostname(inet_ntoa(*(struct in_addr *)&ip_hdr->saddr));
		if (!data->host_name)
			error_exit_program(data, "malloc fail");
		//if(!optionaddr && !strcmp(data->host_name, "no hostname found"))
			printf("   %s", inet_ntoa(*(struct in_addr *)&ip_hdr->saddr));
		// else
			// printf("   %s (%s)", inet_ntoa(*(struct in_addr *)&ip_hdr->saddr), data->host_name);
		free(data->host_name);
	}

	
	if (icmp_hdr->type == ICMP_TIME_EXCEEDED || icmp_hdr->type == ICMP_ECHOREPLY)
		printf("   %.3Lfms", rtt_msec);
	else 
		printf("   *");
}

void trace_pckt_route(t_data *data, struct sockaddr_in *addr_con) {

	for (; data->ttl <= data->max_hop; data->ttl++) {

		for (uint8_t sequence = 0; sequence < data->pckt_count; sequence++) {

			t_icmp_pckt pckt;
			char buffer[sizeof(struct iphdr) + sizeof(t_icmp_pckt)];
			struct timespec time_start, time_end;

			init_icmp_pckt(&pckt, data);
			memset(buffer, 0, sizeof(buffer));
			clock_gettime(CLOCK_MONOTONIC, &time_start);
			if (sendto(data->sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr *)addr_con, sizeof(*addr_con)) <= 0)
				error_exit_program(data, "sendto error");

			struct sockaddr_in rcv_addr_con;
			socklen_t addr_len = sizeof(rcv_addr_con);

			if (!sequence)
					printf("   %d", data->ttl);

			if (wait_response(data)) {
				if (recvfrom(data->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&rcv_addr_con, &addr_len) <= 0)
					error_exit_program(data, "recvfrom error");
					
				long double rtt_msec = get_ping_duration(&time_start, &time_end);

				t_icmp_pckt rcvd_pckt;
				struct iphdr *ip_hdr = (struct iphdr *)buffer;
				struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + (ip_hdr->ihl * 4));

				memcpy(&rcvd_pckt.hdr, icmp_hdr, sizeof(struct icmphdr));
				memcpy(rcvd_pckt.payload, buffer + (ip_hdr->ihl * 4) + sizeof(struct icmphdr), PAYLOAD_SIZE);
			
				print_route_infos(ip_hdr, icmp_hdr, data, sequence, rtt_msec);	
				if (sequence == (data->pckt_count - 1) && icmp_hdr->type == ICMP_ECHOREPLY) {
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

	check_args_count(ac, av);
	options_init(&data, ac, av);
	init_data(&data, ac, av);
	init_sock_addr(&data, &addr_con, data.ip_addr);
	print_traceroute_first_output(&data);
	trace_pckt_route(&data, &addr_con);
	close(data.sockfd);
}