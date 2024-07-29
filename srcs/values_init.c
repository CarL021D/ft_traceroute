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
	data->dns_name = av[ac - 1];
}

void init_sock_addr(struct sockaddr_in *addr_con, char *ip_addr) {    
	
	memset(addr_con, 0, sizeof(struct sockaddr_in));
	addr_con->sin_family = AF_INET;
	addr_con->sin_port = htons(0);
	addr_con->sin_addr.s_addr = inet_addr(ip_addr);
}

void	init_icmp_pckt(t_icmp_pckt *pckt, t_data *data) {

	memset(pckt, 0, sizeof(t_icmp_pckt));
	pckt->hdr.type = ICMP_ECHO;
	pckt->hdr.code = 0;
	pckt->hdr.un.echo.id = getpid();
	pckt->hdr.checksum = 0;
	pckt->hdr.un.echo.sequence = data->sequence;

	for (uint16_t i = 0; i < data->payload_size - 1; i++)
    	pckt->payload[i] = (rand() % 95) + 32;
    pckt->payload[data->payload_size - 1] = '\0';
	pckt->hdr.checksum = checksum(pckt, sizeof(t_icmp_pckt));
}

