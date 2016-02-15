#ifndef POOL4_H_
#define POOL4_H_

#include <stdbool.h>
#include <linux/in.h>
#include <linux/list.h>
#include "types.h"


struct pool4_entry {
	__u32 mark;
	__u8 proto;
	struct in_addr addr;
	struct port_range range;
	struct list_head list_hook;
};

struct pool4;

int pool4_init(struct pool4 *pool4);

int pool4_add(struct pool4 *pool4, __u32 mark, __u8 proto,
		struct in_addr *addr, struct port_range *range);

int pool4_rm(struct pool4 *pool4, __u32 mark, __u8 proto,
		struct in_addr *addr, struct port_range *range);

int pool4_flush(struct pool4 *pool4);

bool pool4_contains(struct pool4 *pool4, __u32 mark, __u8 proto,
		struct in_addr *addr, struct port_range *range);

bool pool4_is_empty(struct pool4 *pool4);

int pool4_count(struct pool4 *pool4);

void pool4_print_all(struct pool4 *pool4);

int pool4_foreach_sample(struct pool4 *pool4,
		int (*cb)(struct pool4_entry *, void *), void *arg,
		struct pool4_entry *offset);

int pool4_taddr4_find_pos(struct pool4 *pool4, int ipv6_pos,
		struct client_mask_domain *result,
		unsigned int masks_per_client);

int pool4_foreach_taddr4(struct pool4 *pool4,
		int (*cback)(struct pool4_mask *, void *), void *arg,
		unsigned int offset);

int pool4_get_nth_taddr(struct pool4 *pool4, struct client_mask_domain *domain,
		unsigned int n, struct ipv4_transport_addr *result);

#endif /* POOL4_H_ */
