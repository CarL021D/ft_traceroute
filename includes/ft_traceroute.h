#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define PAYLOAD_SIZE 56

typedef struct	s_data {

    int32_t			sockfd;
    uint8_t    		payload_size;
    double			sleep_time;
    char			*dns_name;
    char 			*ip_addr;
    // uint16_t   		icmp_pckt_size;
    // uint16_t		sent_pckt_count;
    // uint16_t		rcvd_pckt_count;
    uint16_t    	sequence;
    long double		*rtt_arr;
    char			*opt_p_payload;
    // t_option		option;
}			t_data;

typedef struct	s_icmp_pckt {

    struct icmphdr	hdr;
    char payload[PAYLOAD_SIZE];
}			t_icmp_pckt;

void	init_sock_addr(struct sockaddr_in *addr_con, char *ip_addr);
void	init_data(t_data *data, int ac, char **av);
void	init_icmp_pckt(t_icmp_pckt *pckt, t_data *data, uint16_t ttl);

unsigned short 	checksum(void *b, int len);
char			*resolve_hostname_to_ip(const char *hostname);
long double		get_ping_duration(struct timespec *time_start, struct timespec *time_end);


void    error_exit_program(t_data *data, char *error_message);
#endif