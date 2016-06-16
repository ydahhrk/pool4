#ifndef _JOOL_TYPES_H
#define _JOOL_TYPES_H

#include <linux/types.h>
#include <linux/in6.h>
#include <linux/in.h>
#include <linux/ipv6.h>
#include <stdbool.h>

struct ipv6_prefix {
	/** IPv6 prefix. */
	struct in6_addr address;
	/** Number of bits from "address" which represent the network. */
	__u8 len;
};

struct ipv4_transport_addr {
	struct in_addr l3;
	__u16 l4;
};

char *ip4_to_str (unsigned int ip, char *buffer);


struct client_mask_domain {
	struct ipv4_transport_addr first;
	unsigned int step;
	unsigned int count;
};

struct port_range {
		__u16 min;
		__u16 max;
};

struct pool4_mask {
	__u32 mark;
	__u8 proto;
	struct in_addr addr;
	__u16 port;
};

struct packet {
	struct ipv6hdr *hdr;
};

struct ipv6hdr *pkt_ip6_hdr(struct packet *pkt);

#endif
