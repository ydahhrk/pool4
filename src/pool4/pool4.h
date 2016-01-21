#ifndef POOL4_H_
#define POOL4_H_

#include <stdio.h>
#include <stdlib.h>
#include "../types.h"
#include "../list.h"

struct pool4_entry {
	__u32 mark;
	__u8 proto;
	struct in_addr addr;
	struct port_range range;
	struct list_head list_hook;
};


void pool4_init();

int pool4_add(__u32 mark, __u8 proto,  struct in_addr *addr,
		struct port_range *range);

int pool4_rm(__u32 mark, __u8 proto, struct in_addr *addr,
		struct port_range *range);

int pool4_flush(void);

bool pool4_contains(__u32 mark, __u8 proto, struct in_addr *addr,
		struct port_range *range);

bool pool4_is_empty(void);

int pool4_count(void);

void pool4_print_all(void);

int pool4_foreach_sample(int (*cb)(struct pool4_entry *, void *), void *arg,
		struct pool4_entry *offset);

int taddr4_count();

int taddr4_find_pos(int ipv6_pos, struct client_mask_domain *result);

int pool4_foreach_taddr4(int (*cback)(struct pool4_mask *, void *), void *arg,
		unsigned int offset);

int pool4_get_nth_taddr(struct client_mask_domain *domain,
			unsigned int n,
			struct ipv4_transport_addr *result);

#endif /* POOL4_H_ */
