/*#include "pool4.h"

struct in_addr {
    unsigned long s_addr;  // load with inet_aton()
};

int pool4_init(void)
{
	return 0;
}

void pool4_destroy(void)
{
}

int pool4_allocate(struct ipv4_transport_addr *result)
{
	result->l3.s_addr = htonl(0xc0000201);
	result->l4 = 1234;
	return 0;
}
*/
