/*
 * main.c
 *
 *  Created on: Dec 10, 2015
 *      Author: cfrutos
 */

#include "client.h"

#define cpu_to_be32(x) x
#define be16_to_cpu(x) x

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
static void print_address(char *prefix, struct in6_addr *addr)
{
	printf("%s", prefix);
	printf("%08x:%08x:%08x:%08x\n",
			be16_to_cpu(addr->s6_addr32[0]),
			be16_to_cpu(addr->s6_addr32[1]),
			be16_to_cpu(addr->s6_addr32[2]),
			be16_to_cpu(addr->s6_addr32[3]));
}

static int assert_addr6(struct in6_addr *addr, __u32 quadrant_4)
{
	struct in6_addr expected;

	if (addr->s6_addr32[0] != cpu_to_be32(0x20010db8)) {
		printf("Error: Incorrect value for quadrant 1!\n");
		goto fail;
	}
	if (addr->s6_addr32[1] != cpu_to_be32(0)) {
		printf("Error: Incorrect value for quadrant 2!\n");
		goto fail;
	}
	if (addr->s6_addr32[2] != cpu_to_be32(0)) {
		printf("Error: Incorrect value for quadrant 3!\n");
		goto fail;
	}
	if (addr->s6_addr32[3] != cpu_to_be32(quadrant_4)) {
		printf("Error: Incorrect value for quadrant 4!\n");
		goto fail;
	}

	return 0;

fail:
	expected.s6_addr32[0] = cpu_to_be32(0x20010db8);
	expected.s6_addr32[1] = cpu_to_be32(0x0);
	expected.s6_addr32[2] = cpu_to_be32(0x0);
	expected.s6_addr32[3] = cpu_to_be32(quadrant_4);
	print_address("	Expected: ", &expected);
	print_address("	Actual:   ", addr);
	return -1;
}

static int cb2(struct in6_addr *addr, void *arg)
{
	int *index = arg;
	int error;

	printf("Iteration %d. ", *index);
	print_address("Address: ", addr);

	switch (*index) {
	case 0:
		/* En la 1a iteración, la dirección debe ser 2001:db8::b01. */
		error = assert_addr6(addr, 0xb01);
		break;
	case 1:
		/* En 2a 1a iteración, la dirección debe ser 2001:db8::c00. */
		error = assert_addr6(addr, 0xc00);
		break;
	case 2:
		/* Etc. */
		error = assert_addr6(addr, 0xa00);
		break;
	case 3:
		error = assert_addr6(addr, 0xa01);
		break;
	case 4:
		error = assert_addr6(addr, 0xa02);
		break;
	case 5:
		error = assert_addr6(addr, 0xa03);
		break;
	case 6:
		error = assert_addr6(addr, 0xb00);
		break;
	default:
		printf("Error: Too many iterations!\n");
		error = -1;
	}

	(*index)++;
	return error;
}
int main()
{
	client_init();

	struct ipv6_prefix prefix;
	int i = 1;
	int error;
	int count = 0;

	/* 2001:db8::1 */

	/* client_for_eachsample testing */
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

	/* client_for_each testing */

	prefix.address.s6_addr32[0] = cpu_to_be32(0x20010db8);
	prefix.address.s6_addr32[1] = cpu_to_be32(0x0);
	prefix.address.s6_addr32[2] = cpu_to_be32(0x0);
	prefix.address.s6_addr32[3] = cpu_to_be32(0xa00);
	prefix.len = 126;
	error = client_add(&prefix);
	if (error)
		return error;
;
	prefix.address.s6_addr32[3] = cpu_to_be32(0xb00);
	prefix.len = 127;
	error = client_add(&prefix);
	if (error)
		return error;

	prefix.address.s6_addr32[3] = cpu_to_be32(0xc00);
	prefix.len = 128;
	error = client_add(&prefix);
	if (error)
		return error;

	error = client_for_each(cb2, &count, 12);
	if (error) {
		printf("client_for_each() returned %d\n", error);
		return error;
	}

	if (count != 7){
		printf("Error: I was supposed to iterate over 7 entries\n");
		return -1;
	}

	return 0;
}
