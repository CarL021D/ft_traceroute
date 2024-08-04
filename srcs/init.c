#include "../includes/ft_traceroute.h"

static int32_t init_icmp_socket() {

	int32_t sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		fprintf(stderr, "socket init error\n");
		return -1;
	}
	return sockfd;
}

void	init_data(t_data *data, int ac, char **av) {

	memset(data, 0, sizeof(t_data));
	
	options_init(data, ac, av);

	data->ip_addr = resolve_hostname_to_ip(av[ac - 1]);
	if (!data->ip_addr)
		exit(EXIT_FAILURE);

	if ((data->sockfd = init_icmp_socket()) < 0) {
		fprintf(stderr, "Failed to initialize socket\n");
		exit(EXIT_FAILURE);
	}
	data->max_hop = !data->option.m ? 64 : data->option.m;
	data->pckt_count = !data->option.q ? 3 : data->option.q;
	data->ttl = !data->option.f ? 1 : data->option.f;
	data->dns_name = av[ac - 1];
}

void init_sock_addr(t_data *data, struct sockaddr_in *addr_con, char *ip_addr) {    
	
	memset(addr_con, 0, sizeof(struct sockaddr_in));
	addr_con->sin_family = AF_INET;
	if (inet_pton(AF_INET, data->ip_addr, &addr_con->sin_addr) <= 0) {
		fprintf(stderr, "inet_pton error for %s\n", data->ip_addr);
		exit(EXIT_FAILURE);
	}
	addr_con->sin_port = htons(0);
	addr_con->sin_addr.s_addr = inet_addr(ip_addr);
}

void	init_icmp_pckt(t_icmp_pckt *pckt, t_data *data) {

	setsockopt(data->sockfd, IPPROTO_IP, IP_TTL, &data->ttl, sizeof(data->ttl));

	memset(pckt, 0, sizeof(t_icmp_pckt));
	pckt->hdr.type = ICMP_ECHO;
	pckt->hdr.code = 0;
	pckt->hdr.un.echo.id = getpid();
	pckt->hdr.un.echo.sequence = data->ttl;
	pckt->hdr.checksum = checksum(pckt, sizeof(t_icmp_pckt));
}