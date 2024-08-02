#include "../includes/ft_traceroute.h"

unsigned short checksum(void *b, int len) {
	unsigned short *buf = b; 
	unsigned int sum = 0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char *)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

char *resolve_hostname_to_ip(const char *hostname) {    
	struct addrinfo hints, *res;
	static char ip_addr[INET_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
		fprintf(stderr, "ping: unknown host\n");
		return NULL;
	}

	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
	inet_ntop(AF_INET, &addr->sin_addr, ip_addr, sizeof(ip_addr));
	freeaddrinfo(res);
	return ip_addr;
}

bool is_private_ip(const char *ip_str) {
    struct in_addr addr;
    inet_aton(ip_str, &addr);
    uint32_t ip = ntohl(addr.s_addr);

    if ((ip >= 0x0A000000 && ip <= 0x0AFFFFFF) ||
        (ip >= 0xAC100000 && ip <= 0xAC1FFFFF) ||
        (ip >= 0xC0A80000 && ip <= 0xC0A8FFFF)) {
        return true;
    }
    return false;
}

long double get_ping_duration(struct timespec *time_start, struct timespec *time_end) {

	long double rtt_msec;

	clock_gettime(CLOCK_MONOTONIC, time_end);
	rtt_msec = (time_end->tv_sec - time_start->tv_sec) * 1000.0;
	rtt_msec += (time_end->tv_nsec - time_start->tv_nsec) / 1000000.0;
	
	return rtt_msec;
}
