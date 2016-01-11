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
	struct list_hook list;
};

<<<<<<< HEAD
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
=======
struct client_mask_domain {

		struct ipv4_transport_addr *first;
		unsigned int step;
		unsigned int count;
};

struct ipv4_transport_addr {
	/** The layer-3 identifier. */
	struct in_addr l3;
	/** The layer-4 identifier (Either the port (TCP or UDP) or the ICMP id). */
	__u16 l4;
};

static char *ip4_to_str (unsigned int ip, char *buffer) {

    sprintf (buffer, "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xff,

        (ip >> 8) & 0xff, ip & 0xff);

    return buffer;

>>>>>>> refs/remotes/origin/Frutos
}

#define cpu_to_be32(x) x

#define ENOMEM 12    /* Out of memory */
#define	ESRCH 3	    /* No such process */

typedef enum { false, true } bool;
