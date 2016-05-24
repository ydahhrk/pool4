#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <stdbool.h>
#include <linux/list.h>
#include "pool4/pool4.h"
#include "unit_test.h"

// Global variables

struct pool4 cpool;
struct pool4 spool;
struct client client;

struct pool4_entry one;
struct pool4_entry two;
struct pool4_entry three;
struct pool4_entry four;
struct pool4_entry five;
struct pool4_entry six;
struct pool4_entry seven;

struct ipv6_prefix prefix0;
struct ipv6_prefix prefix1;
struct ipv6_prefix prefix2;
struct ipv6_prefix prefix3;
struct ipv6_prefix prefix4;
struct ipv6_prefix prefix5;
struct ipv6_prefix prefix6;

struct pool4_entry node4_exp[7];
struct pool4_entry node4_seen[7];
struct ipv6_prefix node6_exp[7];
struct ipv6_prefix node6_seen[7];
struct pool4_mask mask_exp[75];
struct pool4_mask mask_seen[75];
struct in6_addr in6addr_exp[7];
struct in6_addr in6addr_seen[7];


int callb(struct pool4_entry *entry, void *arg)
{
	int *arg_int = arg;
	int i;

	if (arg) {
		pr_info("%pI4 %u-%u %d\n", &entry->addr, entry->range.min,
				entry->range.max, *arg_int);
		(*arg_int)++;
	} else {
		pr_info("%pI4 %u-%u\n", &entry->addr, entry->range.min,
				entry->range.max);
	}

	for (i = 0; i <= 6; i++) {
		if (entry->addr.s_addr == node4_exp[i].addr.s_addr &&
				entry->range.min == node4_exp[i].range.min &&
				entry->range.max == node4_exp[i].range.max) {
			node4_seen[i].addr.s_addr = entry->addr.s_addr;
			node4_seen[i].range.min = entry->range.min;
			node4_seen[i].range.max = entry->range.max;
		}
	}

	return 0;
}

int cback(struct pool4_mask *mask, void *arg)
{
	int *arg_int = arg;
	int i;

	if (arg) {
		pr_info("%pI4 %u %d\n", &mask->addr, mask->port, *arg_int);
		(*arg_int)++;
	} else {
		pr_info("%pI4 %u\n", &mask->addr, mask->port);
	}

	for (i = 0; i <= 74; i++) {
		if (mask->addr.s_addr == mask_exp[i].addr.s_addr &&
				mask->port == mask_exp[i].port) {
			mask_seen[i].addr.s_addr = mask->addr.s_addr;
			mask_seen[i].port = mask->port;
			if (mask->port == 64)
				return 1; /* interrumpir temprano. */
		}
	}

	return 0;
}

int cb(struct ipv6_prefix *prefix, void *arg)
{
	int *arg_int = arg;
	int i;

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

	for(i = 0; i <= 6; i++) {
		if (prefix->address.s6_addr32[0] ==
				node6_exp[i].address.s6_addr32[0] &&
				prefix->address.s6_addr32[1] ==
				node6_exp[i].address.s6_addr32[1] &&
				prefix->address.s6_addr32[2] ==
				node6_exp[i].address.s6_addr32[2] &&
				prefix->address.s6_addr32[3] ==
				node6_exp[i].address.s6_addr32[3] &&
				prefix->len == node6_exp[i].len) {
			node6_seen[i].address.s6_addr32[0] =
					prefix->address.s6_addr32[0];
			node6_seen[i].address.s6_addr32[1] =
					prefix->address.s6_addr32[1];
			node6_seen[i].address.s6_addr32[2] =
					prefix->address.s6_addr32[2];
			node6_seen[i].address.s6_addr32[3] =
					prefix->address.s6_addr32[3];
			node6_seen[i].len = prefix->len;
		}
	}

	return 0;
}

int callback(struct in6_addr *addr, void *arg)
{
	int *arg_int = arg;
	int i;

	if (arg) {
		pr_info("%x:%x:%x:%x %d\n", addr->s6_addr32[0],
				addr->s6_addr32[1], addr->s6_addr32[2],
				addr->s6_addr32[3], *arg_int);
		(*arg_int)++;
	} else {
		pr_info("%x:%x:%x:%x\n", addr->s6_addr32[0], addr->s6_addr32[1],
				addr->s6_addr32[2], addr->s6_addr32[3]);
	}

	for (i = 0; i <= 6; i++) {
		if (addr->s6_addr32[0] == in6addr_exp[i].s6_addr32[0] &&
				addr->s6_addr32[1] == in6addr_exp[i].s6_addr32[1] &&
				addr->s6_addr32[2] == in6addr_exp[i].s6_addr32[2] &&
				addr->s6_addr32[3] == in6addr_exp[i].s6_addr32[3]) {
			in6addr_seen[i].s6_addr32[0] = addr->s6_addr32[0];
			in6addr_seen[i].s6_addr32[1] = addr->s6_addr32[1];
			in6addr_seen[i].s6_addr32[2] = addr->s6_addr32[2];
			in6addr_seen[i].s6_addr32[3] = addr->s6_addr32[3];
			if (addr->s6_addr[15] == 14)
				return 1; /* Interrumpir temprano. */
		}
	}

	return 0;
}

// Test functions...

static int clear_nodes_arrays (void)
{
	int i;

	for (i = 0; i <= 6; i++) {
		node4_exp[i].mark = 0;
		node4_exp[i].proto = 0;
		node4_exp[i].addr.s_addr = 0;
		node4_exp[i].range.min = 0;
		node4_exp[i].range.max = 0;

		node4_seen[i].mark = 0;
		node4_seen[i].proto = 0;
		node4_seen[i].addr.s_addr = 0;
		node4_seen[i].range.min = 0;
		node4_seen[i].range.max = 0;

		node6_exp[i].address.s6_addr32[0] = 0;
		node6_exp[i].address.s6_addr32[1] = 0;
		node6_exp[i].address.s6_addr32[2] = 0;
		node6_exp[i].address.s6_addr32[3] = 0;
		node6_exp[i].len = 0;

		node6_seen[i].address.s6_addr32[0] = 0;
		node6_seen[i].address.s6_addr32[1] = 0;
		node6_seen[i].address.s6_addr32[2] = 0;
		node6_seen[i].address.s6_addr32[3] = 0;
		node6_seen[i].len = 0;
	}

	return 0;
}

static bool match_pool4_nodes(void)
{
	int i;
	bool match;

	for (i = 0; i <= 6; i++) {
//		pr_info("%pI4 %u-%u = %pI4 %u-%u\n", &node4_exp[i].addr,
//				node4_exp[i].range.min,	node4_exp[i].range.max,
//				&node4_seen[i].addr, node4_seen[i].range.min,
//				node4_seen[i].range.max);
		if (node4_exp[i].addr.s_addr == node4_seen[i].addr.s_addr &&
				node4_exp[i].range.min ==
				node4_seen[i].range.min &&
				node4_exp[i].range.max ==
				node4_seen[i].range.max)
			match = true;
		else
			match = false;
		if (!match)
			return match;
	}

	return match;
}

static bool match_client_nodes(void)
{
	int i;
	bool match;

	for (i = 0; i <= 6; i++) {
		if (node6_exp[i].address.s6_addr32[0] ==
				node6_seen[i].address.s6_addr32[0] &&
				node6_exp[i].address.s6_addr32[1] ==
				node6_seen[i].address.s6_addr32[1] &&
				node6_exp[i].address.s6_addr32[2] ==
				node6_seen[i].address.s6_addr32[2] &&
				node6_exp[i].address.s6_addr32[3] ==
				node6_seen[i].address.s6_addr32[3] &&
				node6_exp[i].len == node6_seen[i].len)
			match = true;
		else
			match = false;
		if (!match)
			return match;
	}

	return match;
}

static int fill_exp_masks(void)
{
	struct list_head *iter;
	struct pool4_entry *entry;
	int i = 0;
	int p;

	while (i <= 74) {
		list_for_each(iter, &cpool.list) {
			entry = list_entry(iter, struct pool4_entry, list_hook);
			for (p = entry->range.min; p <= entry->range.max; p++) {
				mask_exp[i].addr.s_addr = entry->addr.s_addr;
				mask_exp[i].port = p;
				i++;
			}
		}
	}

	return 0;
}

static bool match_masks(void)
{
	int i;
	bool match;

	for (i = 0; i <= 74; i++) {
		if (mask_exp[i].addr.s_addr == mask_seen[i].addr.s_addr &&
				mask_exp[i].port == mask_seen[i].port)
			match = true;
		else
		{
			match = false;
		}
		if (!match)
			return match;
	}

	return match;
}

static int print_exp_masks(void)
{
	int i;

	for (i = 0; i <= 74; i++) {
		pr_info("%pI4 %u\n", &mask_exp[i].addr, mask_exp[i].port);
	}

	return 0;
}

static int fill_exp_in6_addr(void)
{
	struct list_head *iter;
	struct ipv6_client *entry;
	int i = 0;

	while (i <= 6) {
		list_for_each(iter, &client.list_hook) {
			entry = list_entry(iter, struct ipv6_client, list_hook);
			in6addr_exp[i].s6_addr32[0] = entry->ipx.address.s6_addr32[0];
			in6addr_exp[i].s6_addr32[1] = entry->ipx.address.s6_addr32[1];
			in6addr_exp[i].s6_addr32[2] = entry->ipx.address.s6_addr32[2];
			in6addr_exp[i].s6_addr32[3] = entry->ipx.address.s6_addr32[3];
			i++;
		}
	}

	return 0;
}

static bool match_in6_addrs(void)
{
	int i;
	bool match;

	for (i = 0; i <= 6; i++) {
		if (in6addr_exp[i].s6_addr32[0] == in6addr_seen[0].s6_addr32[0] &&
			in6addr_exp[i].s6_addr32[1] == in6addr_seen[i].s6_addr32[1] &&
			in6addr_exp[i].s6_addr32[2] == in6addr_seen[i].s6_addr32[2] &&
			in6addr_exp[i].s6_addr32[3] == in6addr_seen[i].s6_addr32[3])
			match = true;
		else
			match = false;
		if (!match)
			return match;
	}

	return match;
}

static int print_exp_in6_addrs(void)
{
	int i;

	for (i = 0; i <= 6; i++) {
		pr_info("%x:%x:%x:%x\n", in6addr_exp[i].s6_addr32[0],
				in6addr_exp[i].s6_addr32[1],
				in6addr_exp[i].s6_addr32[2],
				in6addr_exp[i].s6_addr32[3]);
	}

	return 0;
}

static bool init(void)
{
	bool success = true;

	success &= ASSERT_INT(0, pool4_init(&cpool), "Initializing cpool");
	success &= ASSERT_INT(0, pool4_init(&spool), "Initializing spool");
	success &= ASSERT_INT(0, client_init(&client), "Initializing client");

	one.mark = 1;
	one.proto = 1;
	one.addr.s_addr = cpu_to_be32(0xc0000201);
	one.range.min = 4;
	one.range.max = 7;

	two.mark = 2;
	two.proto = 2;
	two.addr.s_addr = cpu_to_be32(0xc0000202);
	two.range.min = 10;
	two.range.max = 20;

	three.mark = 3;
	three.proto = 3;
	three.addr.s_addr = cpu_to_be32(0xc0000203);
	three.range.min = 25;
	three.range.max = 39;

	four.mark = 4;
	four.proto = 4;
	four.addr.s_addr = cpu_to_be32(0xc0000204);
	four.range.min = 40;
	four.range.max = 63;

	five.mark = 5;
	five.proto = 5;
	five.addr.s_addr = cpu_to_be32(0xc0000205);
	five.range.min = 65;
	five.range.max = 80;

	six.mark = 6;
	six.proto = 6;
	six.addr.s_addr = cpu_to_be32(0xc0000206);
	six.range.min = 100;
	six.range.max = 100;

	seven.mark = 7;
	seven.proto = 7;
	seven.addr.s_addr = cpu_to_be32(0xc0000207);
	seven.range.min = 253;
	seven.range.max = 256;

	prefix0.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix0.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix0.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix0.address.s6_addr32[3] = cpu_to_be32(0x0000);
	prefix0.len = 128;

	prefix1.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix1.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix1.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix1.address.s6_addr32[3] = cpu_to_be32(0x0001);
	prefix1.len = 128;

	prefix2.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix2.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix2.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix2.address.s6_addr32[3] = cpu_to_be32(0x0002);
	prefix2.len = 128;

	prefix3.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3.address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3.len = 128;

	prefix4.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix4.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix4.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix4.address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix4.len = 128;

	prefix5.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix5.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix5.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix5.address.s6_addr32[3] = cpu_to_be32(0x0005);
	prefix5.len = 128;

	prefix6.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix6.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix6.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix6.address.s6_addr32[3] = cpu_to_be32(0x0006);
	prefix6.len = 128;

	// Filling cpool database...

	success &= ASSERT_INT(0,
			pool4_add(&cpool, one.mark, one.proto, &one.addr,
					&one.range), "adding one...");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, one.mark, one.proto,
			&one.addr, &one.range), "Confirm add");

	success &= ASSERT_INT(0,
			pool4_add(&cpool, two.mark, two.proto, &two.addr,
					&two.range), "add two test");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, two.mark, two.proto,
			&two.addr, &two.range), "Confirm add...");

	success &= ASSERT_INT(0,
			pool4_add(&cpool, three.mark, three.proto,
					&three.addr, &three.range),
			"add three test");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, three.mark,
			three.proto, &three.addr, &three.range),
			"Confirm add...");

	success &= ASSERT_INT(0,
			pool4_add(&cpool, four.mark, four.proto, &four.addr,
					&four.range), "add four test");
	success &= 	ASSERT_BOOL(true, pool4_contains(&cpool, four.mark,
			four.proto, &four.addr, &four.range), "Confirm add...");

	success &= ASSERT_INT(0,
			pool4_add(&cpool, five.mark, five.proto, &five.addr,
					&five.range), "add five test");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, five.mark,
			five.proto, &five.addr, &five.range), "Confirm add...");

	success &= ASSERT_INT(0,
			pool4_add(&cpool, six.mark, six.proto, &six.addr,
					&six.range), "add six test");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, six.mark, six.proto,
			&six.addr, &six.range), "Confirm add...");

	success &= ASSERT_INT(0,
			pool4_add(&cpool, seven.mark, seven.proto,
					&seven.addr, &seven.range),
			"add seven test");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, seven.mark,
			seven.proto, &seven.addr, &seven.range),
			"Confirm add...");

	// Filling client database...

	success &= ASSERT_INT(0, client_add(&client, &prefix0), "add 0 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix0.address),
			"exist prefix0 test");

	success &= ASSERT_INT(0, client_add(&client, &prefix1), "add 1 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix1.address),
			"exist prefix1 test");

	success &= ASSERT_INT(0, client_add(&client, &prefix2), "add 2 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix2.address),
			"exist prefix2 test");

	success &= ASSERT_INT(0, client_add(&client, &prefix3), "add 3 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix3.address),
			"exist prefix3 test");

	success &= ASSERT_INT(0, client_add(&client, &prefix4), "add 4 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix4.address),
			"exist prefix4 test");

	success &= ASSERT_INT(0, client_add(&client, &prefix5), "add 5 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix5.address),
			"exist prefix5 test");

	success &= ASSERT_INT(0, client_add(&client, &prefix6), "add 6 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix6.address),
			"exist prefix6 test");

	return success;
}

static void end(void)
{
	pool4_flush(&cpool);
	pool4_flush(&spool);
	client_flush(&client);
}

static bool remove_entries(void)
{
	bool success = true;

	/* Removing elements from cpool... */

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, one.mark, one.proto, &one.addr,
					&one.range), "pool4_rm test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, one.mark,
			one.proto, &one.addr, &one.range),
			"Confirm rm...");

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, two.mark, two.proto, &two.addr,
					&two.range), "pool4_rm test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, two.mark,
			two.proto, &two.addr, &two.range),
			"Confirm rm...");

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, three.mark, three.proto, &three.addr,
					&three.range), "pool_rm test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, three.mark,
			three.proto, &three.addr, &three.range),
			"Confirm rm...");

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, four.mark, four.proto, &four.addr,
					&four.range), "pool4_rm test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, four.mark,
			four.proto, &four.addr, &four.range),
			"Confirm rm...");

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, five.mark, five.proto, &five.addr,
					&five.range), "pool4_rm test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, five.mark,
			five.proto, &five.addr, &five.range),
			"Confirm rm...");

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, six.mark, six.proto, &six.addr,
					&six.range), "pool4_rm test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, six.mark,
			six.proto, &six.addr, &six.range),
			"Confirm rm...");

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, seven.mark, seven.proto, &seven.addr,
					&seven.range), "pool4_rm test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, seven.mark,
			seven.proto, &seven.addr, &seven.range),
			"Confirm rm...");

	success &= ASSERT_INT(0, pool4_print_all(&cpool), "print test");
	pr_info("\n");

	/* Removing elements from client... */

	success &= ASSERT_INT(0, client_delete(&client, &prefix0),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client, &prefix0.address),
			"exist prefix0 test");

	success &= ASSERT_INT(0, client_delete(&client, &prefix1),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client, &prefix1.address),
			"exist prefix1 test");

	success &= ASSERT_INT(0, client_delete(&client, &prefix2),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client, &prefix2.address),
			"exist prefix2 test");

	success &= ASSERT_INT(0, client_delete(&client, &prefix3),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client, &prefix3.address),
			"exist prefix3 test");

	success &= ASSERT_INT(0, client_delete(&client, &prefix4),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client, &prefix4.address),
			"exist prefix4 test");

	success &= ASSERT_INT(0, client_delete(&client, &prefix5),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client, &prefix5.address),
			"exist prefix5 test");

	success &= ASSERT_INT(0, client_delete(&client, &prefix6),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client, &prefix6.address),
			"exist prefix6 test");


	success &= ASSERT_INT(0, client_print_all(&client), "print test");
	pr_info("\n");

	return success;
}

static bool count_addr(void)
{
	bool success = true;

	int cpool_addrs = 0;
	int client_addrs = 0;

	success &= ASSERT_BOOL(false, pool4_is_empty(&cpool), "is empty test");

	cpool_addrs = pool4_count(&cpool);
	client_addrs = client_count(&client);

	success &= ASSERT_INT(75, pool4_count(&cpool), "Count test");
	success &= ASSERT_INT(7, client_count(&client), "Count test");

	pr_info("\nAddresses in cpool: %d\n", cpool_addrs);
	pr_info("Addresses in client: %d\n\n", client_addrs);

	return success;
}

static bool print_all_addr(void)
{
	bool success = true;

	pr_info("\n");
	success &= ASSERT_INT(0, pool4_print_all(&cpool), "print pool4 test");

	success &= ASSERT_INT(0, client_print_all(&client), "print client test");
	pr_info("\n");

	return success;
}

static bool addr_exist(void)
{
	bool success = true;

	pr_info("\n");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, one.mark,
			one.proto, &one.addr, &one.range),
			"one exist test");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, two.mark,
			two.proto, &two.addr, &two.range),
			"two exist test");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, three.mark,
			three.proto, &three.addr, &three.range),
			"three exist test");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, four.mark,
			four.proto, &four.addr, &four.range),
			"four exist test");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, five.mark,
			five.proto, &five.addr, &five.range),
			"five exist test");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, six.mark,
			six.proto, &six.addr, &six.range),
			"six exist test");
	success &= ASSERT_BOOL(true, pool4_contains(&cpool, seven.mark,
			seven.proto, &seven.addr, &seven.range),
			"seven exist test");

	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix0.address),
			"exist prefix0 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix1.address),
			"exist prefix1 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix2.address),
			"exist prefix2 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix3.address),
			"exist prefix3 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix4.address),
			"exist prefix4 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix5.address),
			"exist prefix5 test");
	success &= ASSERT_BOOL(true, client_addr_exist(&client, &prefix6.address),
			"exist prefix6 test");

	return success;
}

static bool foreach_sample(void)
{
	bool success = true;
	int a = 12;

	node4_exp[0] = one;
	node4_exp[1] = two;
	node4_exp[2] = three;
	node4_exp[3] = four;
	node4_exp[4] = five;
	node4_exp[5] = six;
	node4_exp[6] = seven;

	/* pool4_foreach_sample test */

	pr_info("\n%d\n", a);
	success &= ASSERT_INT(0, pool4_foreach_sample(&cpool, callb, &a, NULL),
			"pool4 for each sample, offset = NULL");
	pr_info("%d\n", a);
	success &= ASSERT_BOOL(true, match_pool4_nodes(), "Checking nodes");
	pr_info("\n");

	/* Here it needs to start from the 3rd node */
	success &= ASSERT_INT(0, clear_nodes_arrays(), "Clearing arrays...");

	node4_exp[2] = three;
	node4_exp[3] = four;
	node4_exp[4] = five;
	node4_exp[5] = six;
	node4_exp[6] = seven;

	success &= ASSERT_INT(0, pool4_foreach_sample(&cpool, callb, NULL, &two),
			"pool4 for each sample, offset = 2nd node");
	success &= ASSERT_BOOL(true, match_pool4_nodes(), "Checking nodes");
	pr_info("\n");

	/* client_foreach_sample test */

	node6_exp[0] = prefix0;
	node6_exp[1] = prefix1;
	node6_exp[2] = prefix2;
	node6_exp[3] = prefix3;
	node6_exp[4] = prefix4;
	node6_exp[5] = prefix5;
	node6_exp[6] = prefix6;

	pr_info("%d\n", a);
	success &= ASSERT_INT(0, client_for_eachsample(&client, cb, &a, NULL),
			"client for each sample, offset = NULL");
	pr_info("%d\n", a);

	success &= ASSERT_BOOL(true, match_client_nodes(), "Checking nodes");
	pr_info("\n");

	/* It needs to start from the 3rd node*/
	success &= ASSERT_INT(0, clear_nodes_arrays(), "Clearing arrays...");

	node6_exp[2] = prefix2;
	node6_exp[3] = prefix3;
	node6_exp[4] = prefix4;
	node6_exp[5] = prefix5;
	node6_exp[6] = prefix6;

	success &= ASSERT_INT(0,
			client_for_eachsample(&client, cb, NULL, &prefix1),
			"client for each sample, offset = 2nd node");

	success &= ASSERT_BOOL(true, match_client_nodes(), "Checking nodes");
	pr_info("\n");

	return success;
}

static bool for_each_addr(void)
{
	bool success = true;
	int a = 0;

	/* pool4_foreach_taddr4 test */
	pr_info("\n");
	success &= ASSERT_INT(0, fill_exp_masks(), "Fill all masks to array");
//	success &= ASSERT_INT(0, print_exp_masks(), "Print expected masks");

	pr_info("\n%d\n", a);
	success = ASSERT_INT(0, pool4_foreach_taddr4(&cpool, cback, &a, 0),
			"pool4 for each taddr4, offset = 0");
	pr_info("%d\n", a);
	success = ASSERT_BOOL(true, match_masks(), "Checking masks visited");
	pr_info("\n");

	success = ASSERT_INT(0, pool4_foreach_taddr4(&cpool, cback, NULL, 6),
			"pool4 for each taddr4, offset = 6");
	success &= ASSERT_BOOL(true, match_masks(), "Checking masks visited");


	/* client_for_each test */
	a = 12;

	success &= ASSERT_INT(0, fill_exp_in6_addr(), "Fill all in6 addrs");
//	success &= ASSERT_INT(0, print_exp_in6_addrs(), "Print exp in6 addrs");

	pr_info("\n%d\n", a);
	success = ASSERT_INT(0, client_for_each(&client, callback, &a, 0),
			"client for each, offset = 0");
	pr_info("%d\n", a);

	success &= ASSERT_BOOL(true, match_in6_addrs(), "Checking nodes");
	pr_info("\n");

	success = ASSERT_INT(0, client_for_each(&client, callback, NULL, 2),
			"client for each, offset = 2");

	success &= ASSERT_BOOL(true, match_in6_addrs(), "Checking nodes");
	pr_info("\n");

	return success;
}

static bool mask_domain_test(void)
{
	bool success = true;
	struct client_mask_domain domain;

	/* Testing client_get_mask_domain */

	pr_info("\n");
	success &= ASSERT_INT(0, pool4_print_all(&cpool), "print all test");
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix0.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix1.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix2.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix3.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix4.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix5.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix6.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);
	pr_info("\n");

	return success;
}

static bool get_nth_taddr_test(void)
{
	bool success = true;
	struct client_mask_domain domain;
	struct ipv4_transport_addr result;

	pr_info("\n");
	success &= ASSERT_INT(0, pool4_print_all(&cpool), "print all test");
	pr_info("\n");

	/* pool4_get_nth_taddr test */

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix0.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(&cpool, &domain, 9, &result),
			"Asking for the 9th taddr...");
	pr_info("9th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix1.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(&cpool, &domain, 5, &result),
			"Asking for the 5th taddr...");
	pr_info("5th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix2.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(&cpool, &domain, 3, &result),
			"Asking for the 3rd taddr...");
	pr_info("3rd taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix3.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(&cpool, &domain, 8, &result),
			"Asking for the 8th taddr...");
	pr_info("8th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix4.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(&cpool, &domain, 7, &result),
			"Asking for the 7th taddr...");
	pr_info("7th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix5.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(&cpool, &domain, 6, &result),
			"Asking for the 6th taddr...");
	pr_info("6th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(&client, &cpool, &prefix6.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(&cpool, &domain, 2, &result),
			"Asking for the 2nd taddr...");
	pr_info("2nd taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");


	return success;
}

static bool get_mask_test(void)
{
	bool success = true;
	struct ipv4_transport_addr mask;
	struct packet packet;

	pr_info("\n");
	success &= ASSERT_INT(0, pool4_print_all(&cpool), "print all test");
	pr_info("\n");

	success &= ASSERT_INT(0, client_print_all(&client), "print all test");
	pr_info("\n");

	packet.hdr = kmalloc(sizeof(struct ipv6hdr), GFP_KERNEL);
	if (!packet.hdr) {
			return -ENOMEM;
		}

	packet.hdr->saddr = prefix0.address;
	pr_info("%x.%x.%x.%x\n\n", packet.hdr->saddr.in6_u.u6_addr32[0],
			packet.hdr->saddr.in6_u.u6_addr32[1],
			packet.hdr->saddr.in6_u.u6_addr32[2],
			packet.hdr->saddr.in6_u.u6_addr32[3]);

	success &= ASSERT_INT(0,
			get_mask(&packet, &cpool, &spool, &client, &mask, 10),
			"get mask test");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	packet.hdr->saddr = prefix1.address;
	pr_info("%x.%x.%x.%x\n\n", packet.hdr->saddr.in6_u.u6_addr32[0],
			packet.hdr->saddr.in6_u.u6_addr32[1],
			packet.hdr->saddr.in6_u.u6_addr32[2],
			packet.hdr->saddr.in6_u.u6_addr32[3]);

	success &= ASSERT_INT(0,
			get_mask(&packet, &cpool, &spool, &client, &mask, 3),
			"get mask test");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	packet.hdr->saddr = prefix2.address;
	pr_info("%x.%x.%x.%x\n\n", packet.hdr->saddr.in6_u.u6_addr32[0],
			packet.hdr->saddr.in6_u.u6_addr32[1],
			packet.hdr->saddr.in6_u.u6_addr32[2],
			packet.hdr->saddr.in6_u.u6_addr32[3]);

	success &= ASSERT_INT(0,
			get_mask(&packet, &cpool, &spool, &client, &mask, 11),
			"get mask test");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	packet.hdr->saddr = prefix3.address;
	pr_info("%x.%x.%x.%x\n\n", packet.hdr->saddr.in6_u.u6_addr32[0],
			packet.hdr->saddr.in6_u.u6_addr32[1],
			packet.hdr->saddr.in6_u.u6_addr32[2],
			packet.hdr->saddr.in6_u.u6_addr32[3]);

	success &= ASSERT_INT(0,
			get_mask(&packet, &cpool, &spool, &client, &mask, 5),
			"get mask test");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	packet.hdr->saddr = prefix4.address;
	pr_info("%x.%x.%x.%x\n\n", packet.hdr->saddr.in6_u.u6_addr32[0],
			packet.hdr->saddr.in6_u.u6_addr32[1],
			packet.hdr->saddr.in6_u.u6_addr32[2],
			packet.hdr->saddr.in6_u.u6_addr32[3]);

	success &= ASSERT_INT(0,
			get_mask(&packet, &cpool, &spool, &client, &mask, 12),
			"get mask test");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	packet.hdr->saddr = prefix5.address;
	pr_info("%x.%x.%x.%x\n\n", packet.hdr->saddr.in6_u.u6_addr32[0],
			packet.hdr->saddr.in6_u.u6_addr32[1],
			packet.hdr->saddr.in6_u.u6_addr32[2],
			packet.hdr->saddr.in6_u.u6_addr32[3]);

	success &= ASSERT_INT(0,
			get_mask(&packet, &cpool, &spool, &client, &mask, 2),
			"get mask test");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	packet.hdr->saddr = prefix6.address;
	pr_info("%x.%x.%x.%x\n\n", packet.hdr->saddr.in6_u.u6_addr32[0],
			packet.hdr->saddr.in6_u.u6_addr32[1],
			packet.hdr->saddr.in6_u.u6_addr32[2],
			packet.hdr->saddr.in6_u.u6_addr32[3]);

	success &= ASSERT_INT(0,
			get_mask(&packet, &cpool, &spool, &client, &mask, 4),
			"get mask test");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);


	kfree(packet.hdr);
	return success;
}

static int nat64_init(void)
{

	START_TESTS("pool4 test");

	INIT_CALL_END(init(), remove_entries(), end(), "remove functions");
	INIT_CALL_END(init(), count_addr(), end(), "count functions");
	INIT_CALL_END(init(), print_all_addr(), end(), "print functions");
	INIT_CALL_END(init(), addr_exist(), end(), "entry exist functions");
	INIT_CALL_END(init(), foreach_sample(), end(), "for each sample");
	INIT_CALL_END(init(),  for_each_addr(), end(), "for each addr");
	INIT_CALL_END(init(), mask_domain_test(), end(), "get mask domain");
	INIT_CALL_END(init(), get_nth_taddr_test(), end(), "get nth taddr");
	INIT_CALL_END(init(), get_mask_test(), end(), "get mask");

	END_TESTS;

	return 0;
}

static void nat64_exit(void)
{

}

module_init( nat64_init);
module_exit( nat64_exit);
