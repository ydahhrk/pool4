/*
 * main.c
 *
 *  Created on: Dec 10, 2015
 *      Author: cfrutos
 */

#include "client.h"

#define cpu_to_be32(x) x



int main()
{
	client_init();

	struct ipv6_prefix prefix;

	/* 2001:db8::1 */
	prefix.address.s6_addr32[0] = cpu_to_be32(0x20010db8);
	prefix.address.s6_addr32[1] = cpu_to_be32(0);
	prefix.address.s6_addr32[2] = cpu_to_be32(0);
	prefix.address.s6_addr32[3] = cpu_to_be32(0x00000001);
	prefix.len = 32;

	client_add(&prefix);

	prefix.address.s6_addr32[0] = cpu_to_be32(0x20010db7);

	client_add(&prefix);

	prefix.address.s6_addr32[0] = cpu_to_be32(0x20010db6);

	client_add(&prefix);
	client_print_all();

	printf("\n");

	client_delete(&prefix);
	client_print_all();

	printf("\n# of clients: %i\n",client_count());

	client_exist(&prefix);
	prefix.address.s6_addr32[0] = cpu_to_be32(0x20010db7);
	client_exist(&prefix);

	printf("Debugging");
	return 0;
}
