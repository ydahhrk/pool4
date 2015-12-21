#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

#include <stdint.h>

typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

typedef int bool;

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#define kmalloc(x,y)		malloc(x)
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

#endif
