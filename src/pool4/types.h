#include <stdio.h>
#include <stdint.h>
#include "list.h"

typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

typedef __u32 __be32;
typedef __u16 __be16;
typedef __u64 __be64;

struct in_addr {
	__be32 s_addr;
};

struct port_range {
		__u16 min;
		__u16 max;
};

struct pool4_entry {
	__u32 mark;
	__u8 proto;
	struct in_addr addr;
	struct port_range range;
	struct list_head list_hook;
};

struct pool4_mask {
	__u32 mark;
	__u8 proto;
	struct in_addr addr;
	__u16 port;
};

static char *ip_to_str (unsigned int ip, char *buffer) {
    sprintf (buffer, "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xff,
        (ip >> 8) & 0xff, ip & 0xff);
    return buffer;
}

#define cpu_to_be32(x) x

#define ENOMEM 12    /* Out of memory */
#define	ESRCH 3	    /* No such process */

typedef enum { false, true } bool;
