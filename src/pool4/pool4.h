#ifndef POOL4_H_
#define POOL4_H_

#include <stdio.h>
#include <stdlib.h>


void pool4_init();

int pool4_add(__u32 mark, __u8 proto,  struct in_addr addr,
		struct port_range *range);

int pool4_rm(__u32 mark, __u8 proto, struct in_addr addr,
		struct port_range *range);

int pool4_flush(void);

bool pool4_contains(__u32 mark, __u8 proto, struct in_addr addr,
		struct port_range *range);

bool pool4_is_empty(void);

int pool4_count(void);

void pool4_print_all(void);

int pool4_get_nth_taddr(struct client_mask_domain *domain,
			unsigned int n,
			struct ipv4_transport_addr *result);

#endif /* POOL4_H_ */