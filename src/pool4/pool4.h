#ifndef POOL4_H_
#define POOL4_H_

#include "../types.h"

struct pool4_entry {
	__u32 mark;
	__u8 proto;
	struct in_addr addr;
	struct port_range range;
	struct list_head list_hook;
};

struct pool4 {
	struct list_head list;
};

void pool4_init(struct pool4 *pool4);

int pool4_add(struct pool4 *pool4, __u32 mark, __u8 proto,
		struct in_addr *addr, struct port_range *range);

int pool4_rm(struct pool4 *pool4, __u32 mark, __u8 proto,
		struct in_addr *addr, struct port_range *range);

int pool4_flush(struct pool4 *pool4);

int pool4_contains(struct pool4 *pool4, __u32 mark, __u8 proto,
		struct in_addr *addr, struct port_range *range);

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
struct ipv4_transport_addr get_mask(struct packet *packet, struct pool4 *cpool, struct pool4 *spool);

#endif /* POOL4_H_ */
