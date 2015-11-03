#include "pool4.h"

int pool4_init(void)
{
	/* Init code here. */
	return 0;
}

void pool4_destroy(void)
{
	/* Cleanup code here. */
}

int pool4_allocate(struct ipv4_transport_addr *result)
{
	/* Port allocation code here. */
	result->l3.s_addr = htonl(0xc0000201);
	result->l4 = 1234;
	return 0;
}
