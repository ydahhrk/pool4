#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/slab.h>
#include "pool4/pool4.h"

int callb(struct pool4_entry *entry, void *arg)
{
	int *arg_int = arg;
//	char addr[16];

	if (arg) {
		pr_info("%pI4 %u-%u %d\n", &entry->addr, entry->range.min,
				entry->range.max, *arg_int);
		(*arg_int)++;
	} else {
		pr_info("%pI4 %u-%u\n", &entry->addr, entry->range.min,
				entry->range.max);
	}

	return 0;
}

int cback(struct pool4_mask *mask, void *arg)
{
//	char addr[16];
	int *arg_int = arg;

	if (arg) {
		pr_info("%pI4 %u %d\n", &mask->addr, mask->port, *arg_int);
		(*arg_int)++;
	} else {
		pr_info("%pI4 %u\n", &mask->addr, mask->port);
	}

	if (mask->port == 64)
		return 1; /* interrumpir temprano. */

	return 0;
}

int cb(struct ipv6_prefix *prefix, void *arg)
{
	int *arg_int = arg;
	if (arg) {
		pr_info("Address: %x:%x:%x:%x\nLength:%u  %d\n",
				prefix->address.s6_addr32[0],
				prefix->address.s6_addr32[1],
				prefix->address.s6_addr32[2],
				prefix->address.s6_addr32[3],
				prefix->len, *arg_int);
		(*arg_int)++;
	} else {
		pr_info("Address: %x:%x:%x:%x\nLength:%u\n",
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
		pr_info("%x:%x:%x:%x %d\n",
				addr->s6_addr32[0],
				addr->s6_addr32[1],
				addr->s6_addr32[2],
				addr->s6_addr32[3],
				*arg_int);
		(*arg_int)++;
	} else {
		pr_info("%x:%x:%x:%x\n",
				addr->s6_addr32[0],
				addr->s6_addr32[1],
				addr->s6_addr32[2],
				addr->s6_addr32[3]);
	}

	if (addr->s6_addr[15] == 14)
		return 1; /* Interrumpir temprano. */

	return 0;
}


static int nat64_init(void)
{

	int error = 0;
	int a = 12;

	struct pool4 cpool;
	struct pool4 spool;

	struct client client;

	struct client_mask_domain domain;
	struct ipv4_transport_addr result;

	struct packet packet;

	struct ipv6_prefix *prefix0 = kmalloc(sizeof(*prefix0), GFP_KERNEL);
	struct ipv6_prefix *prefix1 = kmalloc(sizeof(*prefix1), GFP_KERNEL);
	struct ipv6_prefix *prefix2 = kmalloc(sizeof(*prefix2), GFP_KERNEL);

	struct pool4_entry *one = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *two = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *three = kmalloc(sizeof(*three), GFP_KERNEL);
	struct pool4_entry *four = kmalloc(sizeof(*four), GFP_KERNEL);

	pr_info("\n\n/* Testing */\n\n");

	client_init(&client);
	pool4_init(&cpool);
	pool4_init(&spool);

	prefix0->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix0->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix0->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix0->address.s6_addr32[3] = cpu_to_be32(0x0000);
	prefix0->len = 128;

	prefix1->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix1->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix1->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix1->address.s6_addr32[3] = cpu_to_be32(0x0001);
	prefix1->len = 128;

	prefix2->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix2->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix2->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix2->address.s6_addr32[3] = cpu_to_be32(0x0002);
	prefix2->len = 128;

	client_add(&client, prefix0);
	client_add(&client, prefix1);
	client_add(&client, prefix2);

	pr_info("Three elements added to client: \n");
	client_print_all(&client);
	pr_info("\n\n");

	pr_info("Deleting one element in client and printing: \n");
	client_delete(&client, prefix0);
	client_print_all(&client);
	pr_info("\n\n");

	pr_info("Flushing client and printing: \n");
	client_flush(&client);
	client_print_all(&client);
	pr_info("\n\n");

	client_add(&client, prefix0);
	client_add(&client, prefix1);
	client_add(&client, prefix2);

	pr_info("Added three elements again and counting clients \n");
	pr_info("Number of clients: %d\n", client_count(&client));
	pr_info("\n\n");

	pr_info("Testing client_addr_exist \n");
	client_addr_exist(&client, &prefix0->address);
	pr_info("Function return 0 if element exists and -1 if not");
	pr_info("\n\n");

	pr_info("Testing client_for_eachsample\n");
	pr_info("%d\n", a);
	error = client_for_eachsample(&client, cb, &a, NULL);
	if (error)
		return error;
	pr_info("%d\n", a);
	pr_info("\n");

	error = client_for_eachsample(&client, cb, NULL, prefix1);
	if (error)
		return error;
	pr_info("\n");

	pr_info("Testing client_for_each\n");
	pr_info("%d\n", a);
	error = client_for_each(&client, callback, &a, 0);
	if (error)
		return error;
	pr_info("%d\n", a);
	pr_info("\n");

	error = client_for_each(&client, callback, NULL, 2);
	if (error)
		return error;
	pr_info("\n");


	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000210);
	two->range.min = 6;
	two->range.max = 8;

	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000250);
	three->range.min = 100;
	three->range.max = 100;

	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000212);
	four->range.min = 30;
	four->range.max = 256;

	pool4_add(&cpool, one->mark, one->proto, &one->addr, &one->range);
	pool4_add(&cpool, two->mark, two->proto, &two->addr, &two->range);
	pool4_add(&cpool, three->mark, three->proto, &three->addr,
			&three->range);
	pool4_add(&cpool, four->mark, four->proto, &four->addr, &four->range);

	//Four elements printed
	pr_info("Four elements added\n");
	pool4_print_all(&cpool);
	pr_info("\n\n");

	//Removing one element and printing
	pr_info("Removing one element and printing\n");
	pool4_rm(&cpool, one->mark, one->proto, &one->addr, &one->range);
	pool4_print_all(&cpool);

	//Flushing the pool4 and printing
	pr_info("\nFlushing the pool4 and printing\n");
	pool4_flush(&cpool);
	pr_info("\n\n");
	pool4_print_all(&cpool);

	//Adding again four elements and printing
	pr_info("Adding again four elements and printing\n");
	pool4_add(&cpool, one->mark, one->proto, &one->addr, &one->range);
	pool4_add(&cpool, two->mark, two->proto, &two->addr, &two->range);
	pool4_add(&cpool, three->mark, three->proto, &three->addr,
			&three->range);
	pool4_add(&cpool, four->mark, four->proto, &four->addr, &four->range);
	pool4_print_all(&cpool);

	//Testing pool4_contains
	pr_info("\nTesting contains\n");
	pool4_contains(&cpool, one->mark, one->proto, &one->addr, &one->range);

	//Testing pool4_empty and printing
	pr_info("Testing empty\n");
	pool4_is_empty(&cpool);

	//Testing count
	pr_info("Testing count\n");
	pr_info("Number of entries in pool4: %d \n\n", pool4_count(&cpool));

	pr_info("%d\n", a);
	error = pool4_foreach_sample(&cpool, callb, &a, NULL);
	if (error)
		return error;
	pr_info("%d\n", a);
	pr_info("\n");

	error = pool4_foreach_sample(&cpool, callb, NULL, two);
	if (error)
		return error;
	pr_info("\n");

	pr_info("Flushing pool4 and adding other 3 elements to test "
			"pool4_foreach_taddr4\n\n");
	pool4_flush(&cpool);

	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000250);
	two->range.min = 100;
	two->range.max = 100;

	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000280);
	three->range.min = 253;
	three->range.max = 256;

	pool4_add(&cpool, one->mark, one->proto, &one->addr, &one->range);
	pool4_add(&cpool, two->mark, two->proto, &two->addr, &two->range);
	pool4_add(&cpool, three->mark, three->proto, &three->addr,
			&three->range);

	a = 0;

	pr_info("%d\n", a);
	error = pool4_foreach_taddr4(&cpool, cback, &a, 0);
	if (error)
		return error;
	pr_info("%d\n", a);
	pr_info("\n");

	error = pool4_foreach_taddr4(&cpool, cback, NULL, 6);
	if (error)
		return error;

//	pr_info("\nAdding one more element and testing same function: \n");
//	four->mark = 4;
//	four->proto = 4;
//	four->addr.s_addr = cpu_to_be32(0xc0000201);
//	four->range.min = 63;
//	four->range.max = 68;
//	pool4_add(&cpool, four->mark, four->proto, &four->addr, &four->range);

//	pr_info("\n");
//	pr_info("Iteration finished early because it reached port 64\n");
//	error = pool4_foreach_taddr4(&cpool, cback, NULL, 1027);
//	if (error)
//		return error;

	pr_info("\n");
	pr_info("Testing client_get_mask_domain\n");
	pool4_flush(&cpool);
	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 10;
	one->range.max = 20;
	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000202);
	two->range.min = 10;
	two->range.max = 20;
	pool4_add(&cpool, one->mark, one->proto, &one->addr, &one->range);
	pool4_add(&cpool, two->mark, two->proto, &two->addr, &two->range);

	error = client_get_mask_domain(&client, &cpool, &prefix0->address,
			&domain, 7);
	if (error)
		return error;
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	error = client_get_mask_domain(&client, &cpool, &prefix1->address,
			&domain, 7);
	if (error)
		return error;
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	error = client_get_mask_domain(&client, &cpool, &prefix2->address,
			&domain, 7);
	if (error)
		return error;
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);
	pr_info("\n\n");

	pr_info("Testing pool4_get_nth_taddr...\n");
	error = pool4_get_nth_taddr(&cpool, &domain, 2, &result);
	if (error)
		return error;
	pr_info("%pI4: %u", &result.l3, result.l4);
	pr_info("\n\n");

	pr_info("Testing get_mask...\n");
	error = get_mask(&packet, &cpool, &spool, &client, &result, 2);
	if (error)
		return error;
	pr_info("%pI4: %u", &result.l3, result.l4);
	pr_info("\n\n");

	return error;

}

static void nat64_exit(void)
{

}

module_init(nat64_init);
module_exit(nat64_exit);
