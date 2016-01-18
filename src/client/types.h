#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

#include <stdint.h>

typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

typedef __u32 __be32;
typedef __u16 __be16;
typedef __u64 __be64;

/*
typedef int bool;

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif
*/

#define cpu_to_be32(x)	x
#define kmalloc(x,y)		malloc(x)
#define kfree(x)		free(x)
#define GFP_KERNEL		(__GFP_WAIT | __GFP_IO | __GFP_FS)
#define INET_ADDRSTRLEN		(16)
#define INET6_ADDRSTRLEN	(48)
#define AF_INET6		(10)


struct in6_addr
{
	union
	{
		__u8 u6_addr8[16];
		__u16 u6_addr16[8];
		__u32 u6_addr32[4];
	} in6_u;

#define s6_addr in6_u.u6_addr8
#define s6_addr16 in6_u.u6_addr16
#define s6_addr32 in6_u.u6_addr32
};

struct ipv6_prefix {
	/** IPv6 prefix. */
	struct in6_addr address;
	/** Number of bits from "address" which represent the network. */
	__u8 len;
};

/*
struct in_addr {
	__be32 s_addr;
};

struct ipv4_transport_addr {
	** The layer-3 identifier. *
	struct in_addr l3;
	** The layer-4 identifier (Either the port (TCP or UDP) or the ICMP id). *
	__u16 l4;
};

struct client_mask_domain {
	struct ipv4_transport_addr first;
	unsigned int step;
	unsigned int count;
};
*/
#endif
