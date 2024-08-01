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
	
	data->ip_addr = resolve_hostname_to_ip(av[ac - 1]);
	if (!data->ip_addr)
		exit(EXIT_FAILURE);

	if ((data->sockfd = init_icmp_socket()) < 0) {
		fprintf(stderr, "Failed to initialize socket\n");
		exit(EXIT_FAILURE);
	}
	data->payload_size = 56;
	data->max_ttl = 64;
	data->dns_name = av[ac - 1];
	data->sequence = 0;
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

void	init_icmp_pckt(t_icmp_pckt *pckt, t_data *data, uint16_t ttl) {

	setsockopt(data->sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
		// if (setsockopt(data->sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
		// 	error_exit_program(data, "setsockopt error");

	memset(pckt, 0, sizeof(t_icmp_pckt));
	pckt->hdr.type = ICMP_ECHO;
	pckt->hdr.code = 0;
	pckt->hdr.un.echo.id = getpid();
	pckt->hdr.un.echo.sequence = ttl;

	// for (uint16_t i = 0; i < data->payload_size; i++)
	//     pckt->payload[i] = '\0';
	pckt->hdr.checksum = checksum(pckt, sizeof(t_icmp_pckt));
	// (void)ttl;
}