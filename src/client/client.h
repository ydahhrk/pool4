#ifndef CLIENT_H_
#define CLIENT_H_

#include "types.h"

struct ipv6_client{
	struct ipv6_prefix ipx;
	struct list_head list_hook;
};

struct client{
	struct list_head list_hook;
};

struct pool4;

int client_init(struct client *client);

int client_add(struct client *client, struct ipv6_prefix *prefix);

int client_delete(struct client *client, struct ipv6_prefix *prefix);

int client_flush(struct client *client);

int client_exist(struct client *client, struct ipv6_prefix *prefix);

unsigned int client_count(struct client *client);

int client_print_all(struct client *client);

int client_for_eachsample(struct client *client, int (*func)(struct ipv6_prefix
		*, void *), void *arg, struct ipv6_prefix *offset);

int client_for_each(struct client *client, int (*cb)(struct in6_addr *, void *),
		void *arg, unsigned int offset);

bool client_addr_exist(struct client *client, struct in6_addr *addr);

int client_get_mask_domain(struct client *client, struct pool4 *pool4,
		struct in6_addr *addr, struct client_mask_domain *result,
		unsigned int masks_per_client);

#endif /* CLIENT_H_ */
