#include "../types.h"
#include "pool4.h"

int callb(struct pool4_entry *entry, void *arg)
{
	int *arg_int = arg;
	char addr[16];

	if (arg) {
		printf("%s %u-%u %d\n", ip4_to_str(entry->addr.s_addr, addr),
				entry->range.min, entry->range.max, *arg_int);
		(*arg_int)++;
	} else {
		printf("%s %u-%u\n", ip4_to_str(entry->addr.s_addr, addr),
				entry->range.min, entry->range.max);
	}

	return 0;
}

int cback(struct pool4_mask *mask, void *arg)
{
	int *arg_int = arg;
	char addr[16];

	if (arg) {
		printf("%s %u %d\n", ip4_to_str(mask->addr.s_addr, addr),
				mask->port, *arg_int);
		(*arg_int)++;
	} else {
		printf("%s %u\n", ip4_to_str(mask->addr.s_addr, addr),
				mask->port);
	}

	if (mask->port == 64)
		return 1; /* Interrumpir temprano. */

	return 0;
}

int main()
{
	int error = 0;
	struct pool4 cpool;
	struct pool4 spool;

	pool4_init(&cpool);
	pool4_init(&spool);

	struct pool4_entry *one = malloc(sizeof(*one));
	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 10;
	one->range.max = 20;

	struct pool4_entry *two = malloc(sizeof(*two));
	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000202);
	one->range.min = 30;
	one->range.max = 40;

	struct pool4_entry *three = malloc(sizeof(*three));
	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 50;
	one->range.max = 60;

	struct pool4_entry *four = malloc(sizeof(*four));
	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000203);
	one->range.min = 70;
	one->range.max = 80;

	pool4_add(&cpool, one->mark, one->proto, &one->addr, &one->range);


//	struct client_mask_domain domain;
//
//	struct ipv4_transport_addr mask;
//
//
//	domain.first.l3.s_addr = cpu_to_be32(0xc0000201);
//	domain.first.l4 = 55;
//	domain.step = 3;
//	domain.count = 4;
//	pool4_get_nth_taddr(&domain, 10, &mask);
//	printf("%s #%u\n\n\n", ip4_to_str(mask.l3.s_addr,addr), mask.l4);

	return error;
}
