/*
 * main.c
 *
 *  Created on: Dec 10, 2015
 *      Author: cfrutos
 */

#include "client.h"

#define cpu_to_be32(x) x

int  cb(struct ipv6_prefix *prefix, void *arg)
{
	int *arg_int = arg;
	if (arg) {
		printf("Address: %x:%x:%x:%x\nLength:%u  %d\n",
				prefix->address.s6_addr32[0],
				prefix->address.s6_addr32[1],
				prefix->address.s6_addr32[2],
				prefix->address.s6_addr32[3],
				prefix->len, *arg_int);
		(*arg_int)++;
	} else {
		printf("Address: %x:%x:%x:%x\nLength:%u\n",
				prefix->address.s6_addr32[0],
				prefix->address.s6_addr32[1],
				prefix->address.s6_addr32[2],
				prefix->address.s6_addr32[3],
				prefix->len);
	}

	return 0;
}

int callback(struct in6_addr *addr, void *arg)
{
	int *arg_int = arg;

	if (arg) {
		printf("%x:%x:%x:%x %d\n",
				addr->s6_addr32[0],
				addr->s6_addr32[1],
				addr->s6_addr32[2],
				addr->s6_addr32[3],
				*arg_int);
		(*arg_int)++;
	} else {
		printf("%x:%x:%x:%x\n",
				addr->s6_addr32[0],
				addr->s6_addr32[1],
				addr->s6_addr32[2],
				addr->s6_addr32[3]);
	}

	if (addr->s6_addr[15] == 14)
		return 1; /* Interrumpir temprano. */

	return 0;
}


int main()
{
	client_init();

	struct ipv6_prefix prefix;
	int i = 1;
	int error;


	/* 2001:db8::1 */

	//client_for_eachsample testing
	/*
	prefix.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix.address.s6_addr32[1] = cpu_to_be32(0xdb8);
	prefix.address.s6_addr32[2] = cpu_to_be32(0x1);
	prefix.address.s6_addr32[3] = cpu_to_be32(0);
	prefix.len = 96;
	client_add(&prefix);

	prefix.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix.address.s6_addr32[1] = cpu_to_be32(0xdb8);
	prefix.address.s6_addr32[2] = cpu_to_be32(0x2);
	prefix.address.s6_addr32[3] = cpu_to_be32(0);

	client_add(&prefix);

	prefix.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix.address.s6_addr32[1] = cpu_to_be32(0xdb8);
	prefix.address.s6_addr32[2] = cpu_to_be32(0x3);
	prefix.address.s6_addr32[3] = cpu_to_be32(0);

	client_add(&prefix);

	prefix.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix.address.s6_addr32[1] = cpu_to_be32(0xdb8);
	prefix.address.s6_addr32[2] = cpu_to_be32(0x4);
	prefix.address.s6_addr32[3] = cpu_to_be32(0);

	client_add(&prefix);

	printf("%d\n", i);
	error =client_for_eachsample(cb, &i, NULL);
	if (error)
		return error;
	printf("%d\n", i);

	prefix.address.s6_addr32[2] = cpu_to_be32(0x2);

	error = client_for_eachsample(cb, NULL, &prefix);
	if(error)
		return error;
	*/

	//client_for_each testing

	prefix.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix.address.s6_addr32[2] = cpu_to_be32(0x0001);
	prefix.address.s6_addr32[3] = cpu_to_be32(0x0000);
	prefix.len = 126;
	client_add(&prefix);

	prefix.address.s6_addr32[2] = cpu_to_be32(0x0002);
	prefix.address.s6_addr32[3] = cpu_to_be32(0x0004);
	client_add(&prefix);

	prefix.address.s6_addr32[2] = cpu_to_be32(0x0003);
	prefix.address.s6_addr32[3] = cpu_to_be32(0x0008);
	client_add(&prefix);

	printf("%d\n", i);

	error = client_for_each(callback, &i, 0);

	if(error)
		return error;
	printf("%d\n", i);


	return 0;
}
