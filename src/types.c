#include "types.h"

struct ipv6hdr *pkt_ip6_hdr(struct packet *pkt) {
	return pkt->hdr;
};
