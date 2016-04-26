#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <stdbool.h>
#include <linux/list.h>
#include "pool4/pool4.h"
#include "unit_test.h"

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
				prefix->address.s6_addr32[3], prefix->len,
				*arg_int);
		(*arg_int)++;
	} else {
		pr_info("Address: %x:%x:%x:%x\nLength:%u\n",
				prefix->address.s6_addr32[0],
				prefix->address.s6_addr32[1],
				prefix->address.s6_addr32[2],
				prefix->address.s6_addr32[3], prefix->len);
	}

	return 0;
}

int callback(struct in6_addr *addr, void *arg)
{
	int *arg_int = arg;

	if (arg) {
		pr_info("%x:%x:%x:%x %d\n", addr->s6_addr32[0],
				addr->s6_addr32[1], addr->s6_addr32[2],
				addr->s6_addr32[3], *arg_int);
		(*arg_int)++;
	} else {
		pr_info("%x:%x:%x:%x\n", addr->s6_addr32[0], addr->s6_addr32[1],
				addr->s6_addr32[2], addr->s6_addr32[3]);
	}

	if (addr->s6_addr[15] == 14)
		return 1; /* Interrumpir temprano. */

	return 0;
}

// Test functions...

static bool get_valid_nth_taddr(struct pool4 *pool4,
		struct client_mask_domain *domain, unsigned int n,
		struct ipv4_transport_addr *result)
{
	bool valid_taddr = true;
	struct pool4_entry *listed;
	struct list_head *iter;
	struct list_head *tmp;

	pool4_get_nth_taddr(pool4, domain, n, result);

	list_for_each_safe(iter, tmp, &pool4->list)
	{
		listed = list_entry(iter, struct pool4_entry, list_hook);
		if (listed->addr.s_addr == result->l3.s_addr)
			return valid_taddr;
	}

	return false;
}

static bool init(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;

	success &= pool4_init(cpool);
	success &= pool4_init(spool);
	success &= client_init(client);
	if (success)
		return false;

	return true;
}

static void end(struct pool4 *cpool, struct pool4 *spool, struct client *client)
{
	pool4_flush(cpool);
	pool4_flush(spool);
	client_flush(client);
}

static int add_test(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;

	/* Creating elements for pool4 and client... */

	struct pool4_entry *one = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *two = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *three = kmalloc(sizeof(*three), GFP_KERNEL);
	struct pool4_entry *four = kmalloc(sizeof(*four), GFP_KERNEL);
	struct pool4_entry *five = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *six = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *seven = kmalloc(sizeof(*three), GFP_KERNEL);

	struct ipv6_prefix *prefix0 = kmalloc(sizeof(*prefix0), GFP_KERNEL);
	struct ipv6_prefix *prefix1 = kmalloc(sizeof(*prefix1), GFP_KERNEL);
	struct ipv6_prefix *prefix2 = kmalloc(sizeof(*prefix2), GFP_KERNEL);
	struct ipv6_prefix *prefix3 = kmalloc(sizeof(*prefix3), GFP_KERNEL);
	struct ipv6_prefix *prefix4 = kmalloc(sizeof(*prefix4), GFP_KERNEL);
	struct ipv6_prefix *prefix5 = kmalloc(sizeof(*prefix5), GFP_KERNEL);
	struct ipv6_prefix *prefix6 = kmalloc(sizeof(*prefix6), GFP_KERNEL);

	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000202);
	two->range.min = 6;
	two->range.max = 8;

	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000203);
	three->range.min = 100;
	three->range.max = 100;

	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000204);
	four->range.min = 30;
	four->range.max = 256;

	five->mark = 5;
	five->proto = 5;
	five->addr.s_addr = cpu_to_be32(0xc0000205);
	five->range.min = 4;
	five->range.max = 7;

	six->mark = 6;
	six->proto = 6;
	six->addr.s_addr = cpu_to_be32(0xc0000206);
	six->range.min = 100;
	six->range.max = 100;

	seven->mark = 7;
	seven->proto = 7;
	seven->addr.s_addr = cpu_to_be32(0xc0000207);
	seven->range.min = 253;
	seven->range.max = 256;

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

	prefix3->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3->address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3->len = 128;

	prefix4->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix4->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix4->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix4->address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix4->len = 128;

	prefix5->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix5->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix5->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix5->address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix5->len = 128;

	prefix6->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix6->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix6->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix6->address.s6_addr32[3] = cpu_to_be32(0x0006);
	prefix6->len = 128;

	// Adding 7 elements to cpool database...

	success &= ASSERT_INT(0,
			pool4_add(cpool, one->mark, one->proto, &one->addr,
					&one->range), "add one test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, two->mark, two->proto, &two->addr,
					&two->range), "add two test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, three->mark, three->proto,
					&three->addr, &three->range),
			"add three test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, four->mark, four->proto, &four->addr,
					&four->range), "add four test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, five->mark, five->proto, &five->addr,
					&five->range), "add five test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, six->mark, six->proto, &six->addr,
					&six->range), "add six test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, seven->mark, seven->proto,
					&seven->addr, &seven->range),
			"add seven test");

	// Adding 7 elements to client database...

	success &= ASSERT_INT(0, client_add(client, prefix0), "add 0 test");
	success &= ASSERT_INT(0, client_add(client, prefix1), "add 1 test");
	success &= ASSERT_INT(0, client_add(client, prefix2), "add 2 test");
	success &= ASSERT_INT(0, client_add(client, prefix3), "add 3 test");
	success &= ASSERT_INT(0, client_add(client, prefix4), "add 4 test");
	success &= ASSERT_INT(0, client_add(client, prefix5), "add 5 test");
	success &= ASSERT_INT(0, client_add(client, prefix6), "add 6 test");

	return success;
}

static bool remove_test(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;

	struct pool4_entry *one = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *two = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *three = kmalloc(sizeof(*three), GFP_KERNEL);
	struct pool4_entry *four = kmalloc(sizeof(*four), GFP_KERNEL);
	struct pool4_entry *five = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *six = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *seven = kmalloc(sizeof(*three), GFP_KERNEL);

	struct ipv6_prefix *prefix0 = kmalloc(sizeof(*prefix0), GFP_KERNEL);
	struct ipv6_prefix *prefix1 = kmalloc(sizeof(*prefix1), GFP_KERNEL);
	struct ipv6_prefix *prefix2 = kmalloc(sizeof(*prefix2), GFP_KERNEL);
	struct ipv6_prefix *prefix3 = kmalloc(sizeof(*prefix3), GFP_KERNEL);
	struct ipv6_prefix *prefix4 = kmalloc(sizeof(*prefix4), GFP_KERNEL);
	struct ipv6_prefix *prefix5 = kmalloc(sizeof(*prefix5), GFP_KERNEL);
	struct ipv6_prefix *prefix6 = kmalloc(sizeof(*prefix6), GFP_KERNEL);

	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000202);
	two->range.min = 6;
	two->range.max = 8;

	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000203);
	three->range.min = 100;
	three->range.max = 100;

	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000204);
	four->range.min = 30;
	four->range.max = 256;

	five->mark = 5;
	five->proto = 5;
	five->addr.s_addr = cpu_to_be32(0xc0000205);
	five->range.min = 4;
	five->range.max = 7;

	six->mark = 6;
	six->proto = 6;
	six->addr.s_addr = cpu_to_be32(0xc0000206);
	six->range.min = 100;
	six->range.max = 100;

	seven->mark = 7;
	seven->proto = 7;
	seven->addr.s_addr = cpu_to_be32(0xc0000207);
	seven->range.min = 253;
	seven->range.max = 256;

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

	prefix3->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3->address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3->len = 128;

	prefix4->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix4->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix4->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix4->address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix4->len = 128;

	prefix5->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix5->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix5->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix5->address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix5->len = 128;

	prefix6->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix6->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix6->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix6->address.s6_addr32[3] = cpu_to_be32(0x0006);
	prefix6->len = 128;

	// Adding 7 elements to cpool database...

	success &= ASSERT_INT(0,
			pool4_add(cpool, one->mark, one->proto, &one->addr,
					&one->range), "add one test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, two->mark, two->proto, &two->addr,
					&two->range), "add two test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, three->mark, three->proto,
					&three->addr, &three->range),
			"add three test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, four->mark, four->proto, &four->addr,
					&four->range), "add four test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, five->mark, five->proto, &five->addr,
					&five->range), "add five test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, six->mark, six->proto, &six->addr,
					&six->range), "add six test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, seven->mark, seven->proto,
					&seven->addr, &seven->range),
			"add seven test");

	// Adding 7 elements to client database...

	success &= ASSERT_INT(0, client_add(client, prefix0), "add 0 test");
	success &= ASSERT_INT(0, client_add(client, prefix1), "add 1 test");
	success &= ASSERT_INT(0, client_add(client, prefix2), "add 2 test");
	success &= ASSERT_INT(0, client_add(client, prefix3), "add 3 test");
	success &= ASSERT_INT(0, client_add(client, prefix4), "add 4 test");
	success &= ASSERT_INT(0, client_add(client, prefix5), "add 5 test");
	success &= ASSERT_INT(0, client_add(client, prefix6), "add 6 test");

	/* Removing 5 elements from cpool... */

	success &= ASSERT_INT(0,
			pool4_rm(cpool, one->mark, one->proto, &one->addr,
					&one->range), "pool4_rm test");
	success &= ASSERT_INT(0,
			pool4_rm(cpool, two->mark, two->proto, &two->addr,
					&two->range), "pool4_rm test");
	success &= ASSERT_INT(0,
			pool4_rm(cpool, three->mark, three->proto, &three->addr,
					&three->range), "pool_rm test");
	success &= ASSERT_INT(0,
			pool4_rm(cpool, four->mark, four->proto, &four->addr,
					&four->range), "pool4_rm test");
	success &= ASSERT_INT(0,
			pool4_rm(cpool, five->mark, five->proto, &five->addr,
					&five->range), "pool4_rm test");

	/* Removing 5 elements from client... */

	success &= ASSERT_INT(0, client_delete(client, prefix0),
			"client_delete test");
	success &= ASSERT_INT(0, client_delete(client, prefix1),
			"client_delete test");
	success &= ASSERT_INT(0, client_delete(client, prefix2),
			"client_delete test");
	success &= ASSERT_INT(0, client_delete(client, prefix3),
			"client_delete test");
	success &= ASSERT_INT(0, client_delete(client, prefix4),
			"client_delete test");

	return success;
}

static bool count_test(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;

	int cpool_entries = 0;
	int client_entries = 0;

	struct pool4_entry *one = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *two = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *three = kmalloc(sizeof(*three), GFP_KERNEL);
	struct pool4_entry *four = kmalloc(sizeof(*four), GFP_KERNEL);
	struct pool4_entry *five = kmalloc(sizeof(*one), GFP_KERNEL);

	struct ipv6_prefix *prefix0 = kmalloc(sizeof(*prefix0), GFP_KERNEL);
	struct ipv6_prefix *prefix1 = kmalloc(sizeof(*prefix1), GFP_KERNEL);
	struct ipv6_prefix *prefix2 = kmalloc(sizeof(*prefix2), GFP_KERNEL);
	struct ipv6_prefix *prefix3 = kmalloc(sizeof(*prefix3), GFP_KERNEL);
	struct ipv6_prefix *prefix4 = kmalloc(sizeof(*prefix4), GFP_KERNEL);

	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000202);
	two->range.min = 6;
	two->range.max = 8;

	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000203);
	three->range.min = 100;
	three->range.max = 100;

	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000204);
	four->range.min = 30;
	four->range.max = 256;

	five->mark = 5;
	five->proto = 5;
	five->addr.s_addr = cpu_to_be32(0xc0000205);
	five->range.min = 4;
	five->range.max = 7;

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

	prefix3->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3->address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3->len = 128;

	prefix4->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix4->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix4->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix4->address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix4->len = 128;

	success &= ASSERT_INT(0,
			pool4_add(cpool, one->mark, one->proto, &one->addr,
					&one->range), "add one test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, two->mark, two->proto, &two->addr,
					&two->range), "add two test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, three->mark, three->proto,
					&three->addr, &three->range),
			"add three test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, four->mark, four->proto, &four->addr,
					&four->range), "add four test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, five->mark, five->proto, &five->addr,
					&five->range), "add five test");

	success &= ASSERT_INT(0, client_add(client, prefix0), "add 0 test");
	success &= ASSERT_INT(0, client_add(client, prefix1), "add 1 test");
	success &= ASSERT_INT(0, client_add(client, prefix2), "add 2 test");
	success &= ASSERT_INT(0, client_add(client, prefix3), "add 3 test");
	success &= ASSERT_INT(0, client_add(client, prefix4), "add 4 test");

	pool4_is_empty(cpool);

	cpool_entries = pool4_count(cpool);

	client_entries = client_count(client);

	return success;
}

static bool print_all_test(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;

	struct pool4_entry *one = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *two = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *three = kmalloc(sizeof(*three), GFP_KERNEL);
	struct pool4_entry *four = kmalloc(sizeof(*four), GFP_KERNEL);
	struct pool4_entry *five = kmalloc(sizeof(*one), GFP_KERNEL);

	struct ipv6_prefix *prefix0 = kmalloc(sizeof(*prefix0), GFP_KERNEL);
	struct ipv6_prefix *prefix1 = kmalloc(sizeof(*prefix1), GFP_KERNEL);
	struct ipv6_prefix *prefix2 = kmalloc(sizeof(*prefix2), GFP_KERNEL);
	struct ipv6_prefix *prefix3 = kmalloc(sizeof(*prefix3), GFP_KERNEL);
	struct ipv6_prefix *prefix4 = kmalloc(sizeof(*prefix4), GFP_KERNEL);

	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000202);
	two->range.min = 6;
	two->range.max = 8;

	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000203);
	three->range.min = 100;
	three->range.max = 100;

	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000204);
	four->range.min = 30;
	four->range.max = 256;

	five->mark = 5;
	five->proto = 5;
	five->addr.s_addr = cpu_to_be32(0xc0000205);
	five->range.min = 4;
	five->range.max = 7;

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

	prefix3->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3->address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3->len = 128;

	prefix4->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix4->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix4->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix4->address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix4->len = 128;

	success &= ASSERT_INT(0,
			pool4_add(cpool, one->mark, one->proto, &one->addr,
					&one->range), "add one test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, two->mark, two->proto, &two->addr,
					&two->range), "add two test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, three->mark, three->proto,
					&three->addr, &three->range),
			"add three test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, four->mark, four->proto, &four->addr,
					&four->range), "add four test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, five->mark, five->proto, &five->addr,
					&five->range), "add five test");

	success &= ASSERT_INT(0, client_add(client, prefix0), "add 0 test");
	success &= ASSERT_INT(0, client_add(client, prefix1), "add 1 test");
	success &= ASSERT_INT(0, client_add(client, prefix2), "add 2 test");
	success &= ASSERT_INT(0, client_add(client, prefix3), "add 3 test");
	success &= ASSERT_INT(0, client_add(client, prefix4), "add 4 test");

	success &= ASSERT_INT(0, pool4_print_all(cpool), "print pool4 test");

	success &= ASSERT_INT(0, client_print_all(client), "print client test");

	return success;
}

static bool entry_exist_test(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;

	struct pool4_entry *one = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *two = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *three = kmalloc(sizeof(*three), GFP_KERNEL);
	struct pool4_entry *four = kmalloc(sizeof(*four), GFP_KERNEL);
	struct pool4_entry *five = kmalloc(sizeof(*one), GFP_KERNEL);

	struct ipv6_prefix *prefix0 = kmalloc(sizeof(*prefix0), GFP_KERNEL);
	struct ipv6_prefix *prefix1 = kmalloc(sizeof(*prefix1), GFP_KERNEL);
	struct ipv6_prefix *prefix2 = kmalloc(sizeof(*prefix2), GFP_KERNEL);
	struct ipv6_prefix *prefix3 = kmalloc(sizeof(*prefix3), GFP_KERNEL);
	struct ipv6_prefix *prefix4 = kmalloc(sizeof(*prefix4), GFP_KERNEL);

	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000202);
	two->range.min = 6;
	two->range.max = 8;

	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000203);
	three->range.min = 100;
	three->range.max = 100;

	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000204);
	four->range.min = 30;
	four->range.max = 256;

	five->mark = 5;
	five->proto = 5;
	five->addr.s_addr = cpu_to_be32(0xc0000205);
	five->range.min = 4;
	five->range.max = 7;

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

	prefix3->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3->address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3->len = 128;

	prefix4->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix4->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix4->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix4->address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix4->len = 128;

	success &= ASSERT_INT(0,
			pool4_add(cpool, one->mark, one->proto, &one->addr,
					&one->range), "add one test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, two->mark, two->proto, &two->addr,
					&two->range), "add two test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, three->mark, three->proto,
					&three->addr, &three->range),
			"add three test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, four->mark, four->proto, &four->addr,
					&four->range), "add four test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, five->mark, five->proto, &five->addr,
					&five->range), "add five test");

	success &= ASSERT_INT(0, client_add(client, prefix0), "add 0 test");
	success &= ASSERT_INT(0, client_add(client, prefix1), "add 1 test");
	success &= ASSERT_INT(0, client_add(client, prefix2), "add 2 test");
	success &= ASSERT_INT(0, client_add(client, prefix3), "add 3 test");
	success &= ASSERT_INT(0, client_add(client, prefix4), "add 4 test");

	success &= pool4_contains(cpool, one->mark, one->proto, &one->addr,
			&one->range);
	success &= pool4_contains(cpool, two->mark, two->proto, &two->addr,
			&two->range);
	success &= pool4_contains(cpool, three->mark, three->proto,
			&three->addr, &three->range);

	success &= ASSERT_INT(0, client_addr_exist(client, &prefix0->address),
			"exist prefix0 test");
	success &= ASSERT_INT(0, client_addr_exist(client, &prefix1->address),
			"exist prefix1 test");
	success &= ASSERT_INT(0, client_addr_exist(client, &prefix2->address),
			"exist prefix2 test");

	return success;
}

static bool foreach_sample_test(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;
	int a = 12;

	struct pool4_entry *one = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *two = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *three = kmalloc(sizeof(*three), GFP_KERNEL);
	struct pool4_entry *four = kmalloc(sizeof(*four), GFP_KERNEL);
	struct pool4_entry *five = kmalloc(sizeof(*one), GFP_KERNEL);

	struct ipv6_prefix *prefix0 = kmalloc(sizeof(*prefix0), GFP_KERNEL);
	struct ipv6_prefix *prefix1 = kmalloc(sizeof(*prefix1), GFP_KERNEL);
	struct ipv6_prefix *prefix2 = kmalloc(sizeof(*prefix2), GFP_KERNEL);
	struct ipv6_prefix *prefix3 = kmalloc(sizeof(*prefix3), GFP_KERNEL);
	struct ipv6_prefix *prefix4 = kmalloc(sizeof(*prefix4), GFP_KERNEL);

	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000202);
	two->range.min = 6;
	two->range.max = 8;

	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000203);
	three->range.min = 100;
	three->range.max = 100;

	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000204);
	four->range.min = 30;
	four->range.max = 256;

	five->mark = 5;
	five->proto = 5;
	five->addr.s_addr = cpu_to_be32(0xc0000205);
	five->range.min = 4;
	five->range.max = 7;

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

	prefix3->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3->address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3->len = 128;

	prefix4->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix4->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix4->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix4->address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix4->len = 128;

	success &= ASSERT_INT(0,
			pool4_add(cpool, one->mark, one->proto, &one->addr,
					&one->range), "add one test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, two->mark, two->proto, &two->addr,
					&two->range), "add two test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, three->mark, three->proto,
					&three->addr, &three->range),
			"add three test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, four->mark, four->proto, &four->addr,
					&four->range), "add four test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, five->mark, five->proto, &five->addr,
					&five->range), "add five test");

	success &= ASSERT_INT(0, client_add(client, prefix0), "add 0 test");
	success &= ASSERT_INT(0, client_add(client, prefix1), "add 1 test");
	success &= ASSERT_INT(0, client_add(client, prefix2), "add 2 test");
	success &= ASSERT_INT(0, client_add(client, prefix3), "add 3 test");
	success &= ASSERT_INT(0, client_add(client, prefix4), "add 4 test");

	/* pool4_foreach_sample test */

	pr_info("%d\n", a);
	success &= ASSERT_INT(0, pool4_foreach_sample(cpool, callb, &a, NULL),
			"pool4 for each sample, offset = NULL");
	pr_info("%d\n", a);
	pr_info("\n");

	success &= ASSERT_INT(0, pool4_foreach_sample(cpool, callb, NULL, two),
			"pool4 for each sample, offset with value");
	pr_info("\n");

	/* client_foreach_sample test */

	pr_info("%d\n", a);
	success &= ASSERT_INT(0, client_for_eachsample(client, cb, &a, NULL),
			"client for each sample, offset = NULL");
	pr_info("%d\n", a);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_for_eachsample(client, cb, NULL, prefix1),
			"client for each sample, offset with value");
	pr_info("\n");

	return success;
}

static bool for_each_test(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;
	int a = 0;

	struct pool4_entry *one = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *two = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *three = kmalloc(sizeof(*three), GFP_KERNEL);
	struct pool4_entry *four = kmalloc(sizeof(*four), GFP_KERNEL);
	struct pool4_entry *five = kmalloc(sizeof(*one), GFP_KERNEL);

	struct ipv6_prefix *prefix0 = kmalloc(sizeof(*prefix0), GFP_KERNEL);
	struct ipv6_prefix *prefix1 = kmalloc(sizeof(*prefix1), GFP_KERNEL);
	struct ipv6_prefix *prefix2 = kmalloc(sizeof(*prefix2), GFP_KERNEL);
	struct ipv6_prefix *prefix3 = kmalloc(sizeof(*prefix3), GFP_KERNEL);
	struct ipv6_prefix *prefix4 = kmalloc(sizeof(*prefix4), GFP_KERNEL);

	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000202);
	two->range.min = 6;
	two->range.max = 8;

	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000203);
	three->range.min = 100;
	three->range.max = 100;

	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000204);
	four->range.min = 65;
	four->range.max = 256;

	five->mark = 5;
	five->proto = 5;
	five->addr.s_addr = cpu_to_be32(0xc0000205);
	five->range.min = 4;
	five->range.max = 7;

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

	prefix3->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3->address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3->len = 128;

	prefix4->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix4->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix4->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix4->address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix4->len = 128;

	success &= ASSERT_INT(0,
			pool4_add(cpool, one->mark, one->proto, &one->addr,
					&one->range), "add one test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, two->mark, two->proto, &two->addr,
					&two->range), "add two test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, three->mark, three->proto,
					&three->addr, &three->range),
			"add three test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, four->mark, four->proto, &four->addr,
					&four->range), "add four test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, five->mark, five->proto, &five->addr,
					&five->range), "add five test");

	success &= ASSERT_INT(0, client_add(client, prefix0), "add 0 test");
	success &= ASSERT_INT(0, client_add(client, prefix1), "add 1 test");
	success &= ASSERT_INT(0, client_add(client, prefix2), "add 2 test");
	success &= ASSERT_INT(0, client_add(client, prefix3), "add 3 test");
	success &= ASSERT_INT(0, client_add(client, prefix4), "add 4 test");

	/* pool4_foreach_taddr4 test */

	pr_info("%d\n", a);
	success = ASSERT_INT(0, pool4_foreach_taddr4(cpool, cback, &a, 0),
			"pool4 for each taddr4, offset = 0");
	pr_info("%d\n", a);
	pr_info("\n");

	success = ASSERT_INT(0, pool4_foreach_taddr4(cpool, cback, NULL, 6),
			"pool4 for each taddr4, offset = 6");

	/* client_for_each test */

	a = 12;

	pr_info("%d\n", a);
	success = ASSERT_INT(0, client_for_each(client, callback, &a, 0),
			"client for each, offset = 0");
	pr_info("%d\n", a);
	pr_info("\n");

	success = ASSERT_INT(0, client_for_each(client, callback, NULL, 2),
			"client for each, offset = 2");
	pr_info("\n");

	return success;
}

static bool client_get_mask_domain_test(struct pool4 *cpool,
		struct pool4 *spool, struct client *client)
{
	bool success = true;
	struct client_mask_domain domain;

	struct pool4_entry *one = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *two = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *three = kmalloc(sizeof(*three), GFP_KERNEL);
	struct pool4_entry *four = kmalloc(sizeof(*four), GFP_KERNEL);
	struct pool4_entry *five = kmalloc(sizeof(*one), GFP_KERNEL);

	struct ipv6_prefix *prefix0 = kmalloc(sizeof(*prefix0), GFP_KERNEL);
	struct ipv6_prefix *prefix1 = kmalloc(sizeof(*prefix1), GFP_KERNEL);
	struct ipv6_prefix *prefix2 = kmalloc(sizeof(*prefix2), GFP_KERNEL);
	struct ipv6_prefix *prefix3 = kmalloc(sizeof(*prefix3), GFP_KERNEL);
	struct ipv6_prefix *prefix4 = kmalloc(sizeof(*prefix4), GFP_KERNEL);

	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000202);
	two->range.min = 6;
	two->range.max = 8;

	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000203);
	three->range.min = 100;
	three->range.max = 100;

	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000204);
	four->range.min = 65;
	four->range.max = 256;

	five->mark = 5;
	five->proto = 5;
	five->addr.s_addr = cpu_to_be32(0xc0000205);
	five->range.min = 4;
	five->range.max = 7;

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

	prefix3->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3->address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3->len = 128;

	prefix4->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix4->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix4->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix4->address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix4->len = 128;

	success &= ASSERT_INT(0,
			pool4_add(cpool, one->mark, one->proto, &one->addr,
					&one->range), "add one test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, two->mark, two->proto, &two->addr,
					&two->range), "add two test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, three->mark, three->proto,
					&three->addr, &three->range),
			"add three test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, four->mark, four->proto, &four->addr,
					&four->range), "add four test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, five->mark, five->proto, &five->addr,
					&five->range), "add five test");

	success &= ASSERT_INT(0, client_add(client, prefix0), "add 0 test");
	success &= ASSERT_INT(0, client_add(client, prefix1), "add 1 test");
	success &= ASSERT_INT(0, client_add(client, prefix2), "add 2 test");
	success &= ASSERT_INT(0, client_add(client, prefix3), "add 3 test");
	success &= ASSERT_INT(0, client_add(client, prefix4), "add 4 test");

	/* Testing client_get_mask_domain */

	success &= ASSERT_INT(0, pool4_print_all(cpool), "print all test");
	pr_info("\n\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix0->address,
					&domain, 7), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix1->address,
					&domain, 7), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix2->address,
					&domain, 7), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);
	pr_info("\n\n");

	return success;
}

static bool pool4_get_nth_taddr_test(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;
	struct client_mask_domain domain;
	struct ipv4_transport_addr result;

	struct pool4_entry *one = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *two = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *three = kmalloc(sizeof(*three), GFP_KERNEL);
	struct pool4_entry *four = kmalloc(sizeof(*four), GFP_KERNEL);
	struct pool4_entry *five = kmalloc(sizeof(*one), GFP_KERNEL);

	struct ipv6_prefix *prefix0 = kmalloc(sizeof(*prefix0), GFP_KERNEL);
	struct ipv6_prefix *prefix1 = kmalloc(sizeof(*prefix1), GFP_KERNEL);
	struct ipv6_prefix *prefix2 = kmalloc(sizeof(*prefix2), GFP_KERNEL);
	struct ipv6_prefix *prefix3 = kmalloc(sizeof(*prefix3), GFP_KERNEL);
	struct ipv6_prefix *prefix4 = kmalloc(sizeof(*prefix4), GFP_KERNEL);

	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000202);
	two->range.min = 6;
	two->range.max = 8;

	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000203);
	three->range.min = 100;
	three->range.max = 100;

	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000204);
	four->range.min = 65;
	four->range.max = 256;

	five->mark = 5;
	five->proto = 5;
	five->addr.s_addr = cpu_to_be32(0xc0000205);
	five->range.min = 4;
	five->range.max = 7;

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

	prefix3->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3->address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3->len = 128;

	prefix4->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix4->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix4->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix4->address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix4->len = 128;

	success &= ASSERT_INT(0,
			pool4_add(cpool, one->mark, one->proto, &one->addr,
					&one->range), "add one test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, two->mark, two->proto, &two->addr,
					&two->range), "add two test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, three->mark, three->proto,
					&three->addr, &three->range),
			"add three test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, four->mark, four->proto, &four->addr,
					&four->range), "add four test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, five->mark, five->proto, &five->addr,
					&five->range), "add five test");

	success &= ASSERT_INT(0, client_add(client, prefix0), "add 0 test");
	success &= ASSERT_INT(0, client_add(client, prefix1), "add 1 test");
	success &= ASSERT_INT(0, client_add(client, prefix2), "add 2 test");
	success &= ASSERT_INT(0, client_add(client, prefix3), "add 3 test");
	success &= ASSERT_INT(0, client_add(client, prefix4), "add 4 test");

	success &= ASSERT_INT(0, pool4_print_all(cpool), "print all test");
	pr_info("\n\n");

	/* pool4_get_nth_taddr test */

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix0->address,
					&domain, 7), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(cpool, &domain, 1, &result),
			"Asking for the 1st taddr...");
//	success &= ASSERT_BOOL(true, get_valid_nth_taddr(cpool, &domain, 3,
//			&result), "Asking for the 3rd taddr...");
	pr_info("%pI4: %u", &result.l3, result.l4);
	pr_info("\n\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix1->address,
					&domain, 7), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(cpool, &domain, 5, &result),
			"Asking for the 5th taddr...");
//	success &= ASSERT_BOOL(true, get_valid_nth_taddr(cpool, &domain, 3,
//			&result), "Asking for the 3rd taddr...");
	pr_info("%pI4: %u", &result.l3, result.l4);
	pr_info("\n\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix2->address,
					&domain, 7), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(cpool, &domain, 3, &result),
			"Asking for the 3rd taddr...");
//	success &= ASSERT_BOOL(true, get_valid_nth_taddr(cpool, &domain, 3,
//			&result), "Asking for the 3rd taddr...");
	pr_info("%pI4: %u", &result.l3, result.l4);
	pr_info("\n\n");

	/* With given values in example */

	pool4_flush(spool);
	pool4_flush(cpool);
	client_flush(client);

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

	prefix0->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix0->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix0->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix0->address.s6_addr32[3] = cpu_to_be32(0x0000);
	prefix0->len = 128;

	success &= ASSERT_INT(0,
			pool4_add(cpool, one->mark, one->proto, &one->addr,
					&one->range), "add one test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, two->mark, two->proto, &two->addr,
					&two->range), "add two test");

	success &= ASSERT_INT(0, client_add(client, prefix0), "add 0 test");

	success &= ASSERT_INT(0, pool4_print_all(cpool), "print all test");
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix0->address,
					&domain, 7), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(cpool, &domain, 1, &result),
			"Asking for the 1st taddr...");
//	success &= ASSERT_BOOL(true, get_valid_nth_taddr(cpool, &domain, 2,
//			&result), "Asking for the 2th taddr...");
	pr_info("%pI4: %u", &result.l3, result.l4);
	pr_info("\n\n");

	return success;
}

static bool get_mask_test(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;
	struct ipv4_transport_addr result;

	struct packet *packet = kmalloc(sizeof(*packet), GFP_KERNEL);

	struct pool4_entry *one = kmalloc(sizeof(*one), GFP_KERNEL);
	struct pool4_entry *two = kmalloc(sizeof(*two), GFP_KERNEL);
	struct pool4_entry *three = kmalloc(sizeof(*three), GFP_KERNEL);
	struct pool4_entry *four = kmalloc(sizeof(*four), GFP_KERNEL);
	struct pool4_entry *five = kmalloc(sizeof(*one), GFP_KERNEL);

	struct ipv6_prefix *prefix0 = kmalloc(sizeof(*prefix0), GFP_KERNEL);
	struct ipv6_prefix *prefix1 = kmalloc(sizeof(*prefix1), GFP_KERNEL);
	struct ipv6_prefix *prefix2 = kmalloc(sizeof(*prefix2), GFP_KERNEL);
	struct ipv6_prefix *prefix3 = kmalloc(sizeof(*prefix3), GFP_KERNEL);
	struct ipv6_prefix *prefix4 = kmalloc(sizeof(*prefix4), GFP_KERNEL);

	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000202);
	two->range.min = 6;
	two->range.max = 8;

	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000203);
	three->range.min = 100;
	three->range.max = 100;

	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000204);
	four->range.min = 65;
	four->range.max = 256;

	five->mark = 5;
	five->proto = 5;
	five->addr.s_addr = cpu_to_be32(0xc0000205);
	five->range.min = 4;
	five->range.max = 7;

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

	prefix3->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3->address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3->len = 128;

	prefix4->address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix4->address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix4->address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix4->address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix4->len = 128;

	success &= ASSERT_INT(0,
			pool4_add(cpool, one->mark, one->proto, &one->addr,
					&one->range), "add one test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, two->mark, two->proto, &two->addr,
					&two->range), "add two test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, three->mark, three->proto,
					&three->addr, &three->range),
			"add three test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, four->mark, four->proto, &four->addr,
					&four->range), "add four test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, five->mark, five->proto, &five->addr,
					&five->range), "add five test");

	success &= ASSERT_INT(0, client_add(client, prefix0), "add 0 test");
	success &= ASSERT_INT(0, client_add(client, prefix1), "add 1 test");
	success &= ASSERT_INT(0, client_add(client, prefix2), "add 2 test");
	success &= ASSERT_INT(0, client_add(client, prefix3), "add 3 test");
	success &= ASSERT_INT(0, client_add(client, prefix4), "add 4 test");

	success &= ASSERT_INT(0, pool4_print_all(cpool), "print all test");
	pr_info("\n");

	success &= ASSERT_INT(0, client_print_all(client), "print all test");
	pr_info("\n");

	success &= ASSERT_INT(0,
			get_mask(packet, cpool, spool, client, &result, 7),
			"get mask test");
	pr_info("%pI4: %u\n", &result.l3, result.l4);

	success &= ASSERT_INT(0,
			get_mask(packet, cpool, spool, client, &result, 8),
			"get mask test");
	pr_info("%pI4: %u\n", &result.l3, result.l4);

	success &= ASSERT_INT(0,
			get_mask(packet, cpool, spool, client, &result, 5),
			"get mask test");
	pr_info("%pI4: %u\n\n", &result.l3, result.l4);

	return success;
}

static int nat64_init(void)
{

	struct pool4 cpool;
	struct pool4 spool;
	struct client client;

	START_TESTS("pool4 test");

	INIT_CALL_END(init(&cpool, &spool, &client),
			add_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "add functions");
	INIT_CALL_END(init(&cpool, &spool, &client),
			remove_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "remove functions");
	INIT_CALL_END(init(&cpool, &spool, &client),
			count_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "count functions");
	INIT_CALL_END(init(&cpool, &spool, &client),
			print_all_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "print functions");
	INIT_CALL_END(init(&cpool, &spool, &client),
			entry_exist_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "entry exist functions");
	INIT_CALL_END(init(&cpool, &spool, &client),
			foreach_sample_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "for each sample test");
	INIT_CALL_END(init(&cpool, &spool, &client),
			for_each_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "for each test");
	INIT_CALL_END(init(&cpool, &spool, &client),
			client_get_mask_domain_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "get mask domain test");
	INIT_CALL_END(init(&cpool, &spool, &client),
			pool4_get_nth_taddr_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "get nth taddr test");
	INIT_CALL_END(init(&cpool, &spool, &client),
			get_mask_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "get mask test");

	END_TESTS
	;

	return 0;
}

static void nat64_exit(void)
{

}

module_init( nat64_init);
module_exit( nat64_exit);
