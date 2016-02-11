#ifndef CLIENT_H_
#define CLIENT_H_

#include "types.h"

void client_init(void);

int client_add(struct ipv6_prefix *prefix);

void client_delete(struct ipv6_prefix *prefix);

void client_flush(void);

bool client_exist(struct ipv6_prefix *prefix);

unsigned int client_count(void);

void client_print_all(void);

int client_for_eachsample(int (*func)(struct ipv6_prefix *, void *), void *arg,
		struct ipv6_prefix *offset);

int client_for_each(int (*cb)(struct in6_addr *, void *),
		void *arg, unsigned int offset);

int client_addr_exist(struct in6_addr *addr);

int client_get_mask_domain(struct in6_addr *client,
		struct client_mask_domain *result,
		unsigned int masks_per_client);

#endif /* CLIENT_H_ */
