#include "bib.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct bib_entry {
	struct ipv4_transport_addr addr;
	enum l4_protocol proto;
};

#define TABLE_SIZE 100
struct bib_entry table[TABLE_SIZE];

int bibdb_init(void)
{
	unsigned int i;

	srand(time(NULL));

	for (i = 0; i < TABLE_SIZE; i++) {
		table[i].addr.l3.s_addr = htonl(0xc0000200 + i % 2);
		table[i].addr.l4 = rand();
		table[i].proto = L4PROTO_TCP;
	}

	return 0;
}

void bibdb_destroy(void)
{
	/* No code. */
}

bool bibdb_contains4(const struct ipv4_transport_addr *addr,
		const l4_protocol proto)
{
	struct bib_entry *bib;
	unsigned int i;

	for (i = 0; i < TABLE_SIZE; i++) {
		bib = &table[i];
		if (taddr4_equals(&bib->addr, addr) && bib->proto == proto)
			return true;
	}

	return false;
}

void bibdb_print(void)
{
	struct bib_entry *bib;
	unsigned int i;

	printf("------------------------------------------------\n");
	printf("Using BIB:\n");

	for (i = 0; i < TABLE_SIZE; i++) {
		bib = &table[i];
		printf("\t%u:\t%s\t%u\t%u\n", i, inet_ntoa(bib->addr.l3),
				bib->addr.l4, bib->proto);
	}

	printf("------------------------------------------------\n");
}
