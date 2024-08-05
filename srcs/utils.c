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

char	*ip_to_hostname(char* ip_addr) {
    struct sockaddr_in sa;
    struct hostent *he;
    char *host_name;

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;

    inet_pton(AF_INET, ip_addr, &sa.sin_addr);
    he = gethostbyaddr(&sa.sin_addr, sizeof(sa.sin_addr), AF_INET);
    if (!he)
        return strdup("no hostname found");
    host_name = strdup(he->h_name);
    if (!host_name)
        return (NULL);
    return host_name;
}



long double get_ping_duration(struct timespec *time_start, struct timespec *time_end) {

	long double rtt_msec;

	clock_gettime(CLOCK_MONOTONIC, time_end);
	rtt_msec = (time_end->tv_sec - time_start->tv_sec) * 1000.0;
	rtt_msec += (time_end->tv_nsec - time_start->tv_nsec) / 1000000.0;
	
	return rtt_msec;
}
