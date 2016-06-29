#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <stdbool.h>
#include <linux/list.h>
#include "pool4/pool4.h"
#include "client/client.h"
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
		}
	}
	if (mask->port == 64)
		return 1; /* interrumpir temprano. */

	return 0;
}

int cb(struct ipv6_prefix *prefix, void *arg)
{
	int *arg_int = arg;
	int i;

	if (arg) {
		pr_info("Address: %x:%x:%x:%x\nLength:%u  %d\n",
				be32_to_cpu(prefix->address.s6_addr32[0]),
				be32_to_cpu(prefix->address.s6_addr32[1]),
				be32_to_cpu(prefix->address.s6_addr32[2]),
				be32_to_cpu(prefix->address.s6_addr32[3]),
				prefix->len, *arg_int);
		(*arg_int)++;
	} else {
		pr_info("Address: %x:%x:%x:%x\nLength:%u\n",
				be32_to_cpu(prefix->address.s6_addr32[0]),
				be32_to_cpu(prefix->address.s6_addr32[1]),
				be32_to_cpu(prefix->address.s6_addr32[2]),
				be32_to_cpu(prefix->address.s6_addr32[3]),
				prefix->len);
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
		pr_info("%x:%x:%x:%x %d\n", be32_to_cpu(addr->s6_addr32[0]),
				be32_to_cpu(addr->s6_addr32[1]),
				be32_to_cpu(addr->s6_addr32[2]),
				be32_to_cpu(addr->s6_addr32[3]), *arg_int);
		(*arg_int)++;
	} else {
		pr_info("%x:%x:%x:%x\n", be32_to_cpu(addr->s6_addr32[0]),
				be32_to_cpu(addr->s6_addr32[1]),
				be32_to_cpu(addr->s6_addr32[2]),
				be32_to_cpu(addr->s6_addr32[3]));
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
		}
	}
	if (addr->s6_addr[15] == 14)
		return 1; /* Interrumpir temprano. */

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
				if (p == 64);
					return 0;
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
			match = false;
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
			if (entry->ipx.address.s6_addr[15] == 14)
				return 0;
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

static bool match_domain(struct client_mask_domain *expected,
		struct client *client, struct pool4 *pool4,
		struct in6_addr *addr, struct client_mask_domain *domain,
		unsigned int masks_per_client)
{
	bool success = true;
	int error;

	error = client_get_mask_domain(client, pool4, addr, domain,
			masks_per_client);
	if (error)
		return false;

	if (expected->first.l3.s_addr == domain->first.l3.s_addr &&
			expected->first.l4 == domain->first.l4 &&
			expected->step == domain->step &&
			expected->count == domain->count)
		success = true;
	else
		success = false;

	return success;
}

static bool match_nth_taddr(struct ipv4_transport_addr *taddr_exp,
		struct pool4 *pool4, struct client_mask_domain *domain,
		unsigned int n, struct ipv4_transport_addr *result)
{
	bool success = true;
	int error;

	error = pool4_get_nth_taddr(pool4, domain, n, result);
	if (error)
		return false;

	if (taddr_exp->l3.s_addr == result->l3.s_addr &&
			taddr_exp->l4 == result->l4)
		success = true;
	else
		success = false;

	return success;
}

static bool valid_mask(struct ipv4_transport_addr *exp_mask,
		struct packet *packet, struct pool4 *cpool, struct pool4 *spool,
		struct client *client, struct ipv4_transport_addr *result,
		unsigned int masks_per_client)
{
	bool success = true;
	int error;

	error = get_mask(packet, cpool, spool, client, result, masks_per_client);
	if (error)
		return false;

	if (exp_mask->l3.s_addr == result->l3.s_addr &&
			exp_mask->l4 == result->l4)
		success = true;
	else
		success = false;

	return success;
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
	prefix1.len = 122;

	prefix2.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix2.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix2.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix2.address.s6_addr32[3] = cpu_to_be32(0x0002);
	prefix2.len = 127;

	prefix3.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3.address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3.len = 126;

	prefix4.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix4.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix4.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix4.address.s6_addr32[3] = cpu_to_be32(0x0004);
	prefix4.len = 127;

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
					&one.range), "Remove element one");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, one.mark,
			one.proto, &one.addr, &one.range),
			"Confirm rm one...");

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, two.mark, two.proto, &two.addr,
					&two.range), "Remove element two");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, two.mark,
			two.proto, &two.addr, &two.range),
			"Confirm rm two...");

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, three.mark, three.proto, &three.addr,
					&three.range), "Remove element three");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, three.mark,
			three.proto, &three.addr, &three.range),
			"Confirm rm three...");

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, four.mark, four.proto, &four.addr,
					&four.range), "Remove element four");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, four.mark,
			four.proto, &four.addr, &four.range),
			"Confirm rm four...");

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, five.mark, five.proto, &five.addr,
					&five.range), "Remove element five");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, five.mark,
			five.proto, &five.addr, &five.range),
			"Confirm rm five...");

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, six.mark, six.proto, &six.addr,
					&six.range), "Remove element six");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, six.mark,
			six.proto, &six.addr, &six.range),
			"Confirm rm six...");

	success &= ASSERT_INT(0,
			pool4_rm(&cpool, seven.mark, seven.proto, &seven.addr,
					&seven.range), "Remove element seven");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, seven.mark,
			seven.proto, &seven.addr, &seven.range),
			"Confirm rm seven...");

	success &= ASSERT_INT(0, pool4_print_all(&cpool), "print test");

	/* Removing elements from client... */

	success &= ASSERT_INT(0, client_delete(&client, &prefix0),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix0.address), "exist prefix0 test");

	success &= ASSERT_INT(0, client_delete(&client, &prefix1),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix1.address), "exist prefix1 test");

	success &= ASSERT_INT(0, client_delete(&client, &prefix2),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix2.address), "exist prefix2 test");

	success &= ASSERT_INT(0, client_delete(&client, &prefix3),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix3.address), "exist prefix3 test");

	success &= ASSERT_INT(0, client_delete(&client, &prefix4),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix4.address), "exist prefix4 test");

	success &= ASSERT_INT(0, client_delete(&client, &prefix5),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix5.address), "exist prefix5 test");

	success &= ASSERT_INT(0, client_delete(&client, &prefix6),
			"client_delete test");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix6.address), "exist prefix6 test");

	success &= ASSERT_INT(0, client_print_all(&client), "print test");
	pr_info("\n");

	return success;
}

static bool flush_dbases(void)
{
	int error = 0;
	bool success = true;

	error = pool4_flush(&cpool);
	if (error)
		return false;

	success &= ASSERT_INT(0, pool4_count(&cpool), "cpool flushing");
	success &= ASSERT_BOOL(true, pool4_is_empty(&cpool), "cpool is empty");

	error = pool4_flush(&spool);
	if (error)
		return false;

	success &= ASSERT_INT(0, pool4_count(&spool), "spool flushing");
	success &= ASSERT_BOOL(true, pool4_is_empty(&spool), "spool is empty");

	error = client_flush(&client);
	if (error)
		return false;

	success &= ASSERT_INT(0, client_count(&client), "client flushing");

	return success;
}

static bool print_all_addr(void)
{
	int error;
	bool success = true;

	pr_info("\n");
	error = pool4_print_all(&cpool);
	if (error)
		return false;

	pr_info("\n");
	success &= ASSERT_INT(0, pool4_print_all(&cpool), "print cpool");

	pr_info("\n");
	error = client_print_all(&client);
	if (error)
		return false;
	pr_info("\n");

	success &= ASSERT_INT(0, client_print_all(&client), "print client");
	pr_info("\n");

	return success;
}

static bool count_addr(void)
{
	bool success = true;

	success &= ASSERT_BOOL(false, pool4_is_empty(&cpool), "cpool is empty");
	success &= ASSERT_BOOL(true, pool4_is_empty(&spool), "spool is empty");

	success &= ASSERT_INT(75, pool4_count(&cpool), "cpool count");
	success &= ASSERT_INT(0, pool4_count(&spool), "spool count");
	success &= ASSERT_INT(75, client_count(&client), "client count");

	//cpool, removing elements and count
	success &= ASSERT_INT(0,
			pool4_rm(&cpool, four.mark, four.proto, &four.addr,
					&four.range), "Remove element four");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, four.mark,
			four.proto, &four.addr, &four.range),
			"Confirm rm four...");

	success &= ASSERT_BOOL(false, pool4_is_empty(&cpool), "cpool is empty");

	success &= ASSERT_INT(51, pool4_count(&cpool), "cpool count");

	//client, removing elements and count
	success &= ASSERT_INT(0, client_delete(&client, &prefix3),
			"delete prefix3");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix3.address), "exist prefix3");

	success &= ASSERT_INT(71, client_count(&client), "client count");

	return success;
}

static bool elements_exist(void)
{
	int error;
	bool success = true;

	//cpool elements
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

	//cpool, erase elements and ask if exist them
	error = pool4_flush(&cpool);
	if (error)
		return false;

	success &= ASSERT_BOOL(false, pool4_contains(&cpool, one.mark,
			one.proto, &one.addr, &one.range),
			"one exist test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, two.mark,
			two.proto, &two.addr, &two.range),
			"two exist test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, three.mark,
			three.proto, &three.addr, &three.range),
			"three exist test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, four.mark,
			four.proto, &four.addr, &four.range),
			"four exist test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, five.mark,
			five.proto, &five.addr, &five.range),
			"five exist test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, six.mark,
			six.proto, &six.addr, &six.range),
			"six exist test");
	success &= ASSERT_BOOL(false, pool4_contains(&cpool, seven.mark,
			seven.proto, &seven.addr, &seven.range),
			"seven exist test");

	//client elements
	success &= ASSERT_BOOL(true, client_exist(&client, &prefix0),
			"exist prefix0");
	success &= ASSERT_BOOL(true, client_exist(&client, &prefix1),
			"exist prefix1");
	success &= ASSERT_BOOL(true, client_exist(&client, &prefix2),
			"exist prefix2");
	success &= ASSERT_BOOL(true, client_exist(&client, &prefix3),
			"exist prefix3");
	success &= ASSERT_BOOL(true, client_exist(&client, &prefix4),
			"exist prefix4");
	success &= ASSERT_BOOL(true, client_exist(&client, &prefix5),
			"exist prefix5");
	success &= ASSERT_BOOL(true, client_exist(&client, &prefix6),
			"exist prefix6");

	//client addresses
	success &= ASSERT_BOOL(true, client_addr_exist(&client,
			&prefix0.address), "exist prefix0 address");
	success &= ASSERT_BOOL(true, client_addr_exist(&client,
			&prefix1.address), "exist prefix1 address");
	success &= ASSERT_BOOL(true, client_addr_exist(&client,
			&prefix2.address), "exist prefix2 address");
	success &= ASSERT_BOOL(true, client_addr_exist(&client,
			&prefix3.address), "exist prefix3 address");
	success &= ASSERT_BOOL(true, client_addr_exist(&client,
			&prefix4.address), "exist prefix4 address");
	success &= ASSERT_BOOL(true, client_addr_exist(&client,
			&prefix5.address), "exist prefix5 address");
	success &= ASSERT_BOOL(true, client_addr_exist(&client,
			&prefix6.address), "exist prefix6 address");

	//client, erase elements and ask if exist them
	error = client_flush(&client);
	if (error)
		return false;

	//clients elements
	success &= ASSERT_BOOL(false, client_exist(&client, &prefix0),
			"exist prefix0");
	success &= ASSERT_BOOL(false, client_exist(&client, &prefix1),
			"exist prefix1");
	success &= ASSERT_BOOL(false, client_exist(&client, &prefix2),
			"exist prefix2");
	success &= ASSERT_BOOL(false, client_exist(&client, &prefix3),
			"exist prefix3");
	success &= ASSERT_BOOL(false, client_exist(&client, &prefix4),
			"exist prefix4");
	success &= ASSERT_BOOL(false, client_exist(&client, &prefix5),
			"exist prefix5");
	success &= ASSERT_BOOL(false, client_exist(&client, &prefix6),
			"exist prefix6");

	//clients addresses
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix0.address), "exist prefix0 address");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix1.address), "exist prefix1 address");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix2.address), "exist prefix2 address");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix3.address), "exist prefix3 address");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix4.address), "exist prefix4 address");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix5.address), "exist prefix5 address");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix6.address), "exist prefix6 address");

	return success;
}

static bool foreach_sample(void)
{
	bool success = true;
	int a = 1;

	/* pool4_foreach_sample test */

	// cpool; offset = NULL, a = 1
	success &= ASSERT_INT(0, clear_nodes_arrays(), "Clearing arrays...");

	node4_exp[0] = one;
	node4_exp[1] = two;
	node4_exp[2] = three;
	node4_exp[3] = four;
	node4_exp[4] = five;
	node4_exp[5] = six;
	node4_exp[6] = seven;

	pr_info("\n%d\n", a);
	success &= ASSERT_INT(0, pool4_foreach_sample(&cpool, callb, &a, NULL),
			"pool4 for each sample, offset = NULL");
	pr_info("%d\n", a);
	success &= ASSERT_BOOL(true, match_pool4_nodes(), "Match pool4 nodes");
	pr_info("\n");

	// cpool; offset = second node, arg = NULL, so it starts from 3rd node
	success &= ASSERT_INT(0, clear_nodes_arrays(), "Clearing arrays...");

	node4_exp[2] = three;
	node4_exp[3] = four;
	node4_exp[4] = five;
	node4_exp[5] = six;
	node4_exp[6] = seven;

	success &= ASSERT_INT(0, pool4_foreach_sample(&cpool, callb, NULL, &two),
			"pool4 for each sample, offset = 2nd node");
	success &= ASSERT_BOOL(true, match_pool4_nodes(), "Match pool4 nodes");
	pr_info("\n");

	//cpool; offset = 5th node, arg = 1, so it starts from 6th node
	a = 1;
	success &= ASSERT_INT(0, clear_nodes_arrays(), "Clearing arrays...");

	node4_exp[5] = six;
	node4_exp[6] = seven;

	pr_info("%d\n", a);
	success &= ASSERT_INT(0, pool4_foreach_sample(&cpool, callb, NULL, &five),
			"pool4 for each sample, offset = 5th node");
	pr_info("%d\n", a);
	success &= ASSERT_BOOL(true, match_pool4_nodes(), "Match pool4 nodes");

	// cpool; offset = last node, a = 1, so it does not visit any node
	a = 1;
	success &= ASSERT_INT(0, clear_nodes_arrays(), "Clearing arrays...");

	pr_info("\n%d\n", a);
	success &= ASSERT_INT(0, pool4_foreach_sample(&cpool, callb, NULL, &seven),
			"pool4 for each sample, offset = 7th node");
	pr_info("%d\n", a);
	success &= ASSERT_BOOL(true, match_pool4_nodes(), "Match pool4 nodes");
	pr_info("\n");

	/* client_foreach_sample test */

	// client; offset = NULL, arg = 1
	a = 1;
	success &= ASSERT_INT(0, clear_nodes_arrays(), "Clearing arrays...");

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
	success &= ASSERT_BOOL(true, match_client_nodes(), "Match client nodes");
	pr_info("\n");

	// client; offset = 2nd node, arg = NULL, so it starts from 3rd node
	success &= ASSERT_INT(0, clear_nodes_arrays(), "Clearing arrays...");

	node6_exp[2] = prefix2;
	node6_exp[3] = prefix3;
	node6_exp[4] = prefix4;
	node6_exp[5] = prefix5;
	node6_exp[6] = prefix6;

	success &= ASSERT_INT(0,
			client_for_eachsample(&client, cb, NULL, &prefix1),
			"client for each sample, offset = 2nd node");
	success &= ASSERT_BOOL(true, match_client_nodes(), "Match client nodes");
	pr_info("\n");

	// client; offset = 5th node, arg = 1, so it starts from 6th node
	a = 1;
	success &= ASSERT_INT(0, clear_nodes_arrays(), "Clearing arrays...");

	node6_exp[5] = prefix5;
	node6_exp[6] = prefix6;

	pr_info("%d\n", a);
	success &= ASSERT_INT(0, client_for_eachsample(&client, cb, &a, &prefix4),
			"client for each sample, offset = 5th node");
	pr_info("%d\n", a);
	success &= ASSERT_BOOL(true, match_client_nodes(), "Match client nodes");
	pr_info("\n");

	// client; offset = last node, a = 1, so it does not visit any node
	a= 1;
	success &= ASSERT_INT(0, clear_nodes_arrays(), "Clearing arrays...");

	pr_info("%d\n", a);
	success &= ASSERT_INT(0, client_for_eachsample(&client, cb, &a, &prefix6),
			"client for each sample, offset = 5th node");
	pr_info("%d\n", a);
	success &= ASSERT_BOOL(true, match_client_nodes(), "Match client nodes");
	pr_info("\n");

	return success;
}

static bool for_each_addr(void)
{
	bool success = true;
	int a = 0;

	/* pool4_foreach_taddr4 test */

	// cpool; offset = 0, arg = 0; so it starts from 1st node address
	success &= ASSERT_INT(0, fill_exp_masks(), "Fill exp masks to array");

	pr_info("\n%d\n", a);
	success &= ASSERT_INT(0, pool4_foreach_taddr4(&cpool, cback, &a, 0),
			"pool4 for each taddr4, offset = 0");
	pr_info("%d\n", a);
	success &= ASSERT_BOOL(true, match_masks(), "Checking masks visited");
	pr_info("\n");

	// cpool; offset = 6, arg = NULL; so it starts from 7th address
	success &= ASSERT_INT(0, pool4_foreach_taddr4(&cpool, cback, NULL, 6),
			"pool4 for each taddr4, offset = 6");
	success &= ASSERT_BOOL(true, match_masks(), "Checking masks visited");
	pr_info("\n");

	// cpool; offset = 78, so it starts from 4th address
	success &= ASSERT_INT(0, pool4_foreach_taddr4(&cpool, cback, NULL, 78),
			"pool4 for each taddr4, offset = 78");
	success &= ASSERT_BOOL(true, match_masks(), "Checking masks visited");

	// cpool; when cb gets port 64
	success &= ASSERT_INT(0, pool4_flush(&cpool), "Cleanning pool4");

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
	three.range.min = 60;
	three.range.max = 70;

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

	pr_info("\n");
	pool4_print_all(&cpool);
	pr_info("\n");

	success &= ASSERT_INT(0, fill_exp_masks(), "Fill exp masks to array");

	// cpool; when cb gets port 64, offset = 0, arg = NULL, starts from 1st
	success &= ASSERT_INT(1, pool4_foreach_taddr4(&cpool, cback, NULL, 0),
			"Port 64 and offset = 0");
	success &= ASSERT_BOOL(true, match_masks(), "Match masks visited");
	pr_info("\n");

	// cpool; when cb gets port 64, offset = 4, arg = 0, starts from 5th
	a = 1;
	pr_info("%d\n", a);
	success &= ASSERT_INT(1, pool4_foreach_taddr4(&cpool, cback, &a, 4),
			"Port 64 and offset = 4");
	pr_info("%d", a);
	success &= ASSERT_BOOL(true, match_masks(), "Match masks visited");


	/* client_for_each test */

	// client; offset = 0, arg = 1, so it starts from 1st
	success &= ASSERT_INT(0, fill_exp_in6_addr(), "Fill exp in6 addrs");

	a = 1;
	pr_info("\n%d\n", a);
	success &= ASSERT_INT(0, client_for_each(&client, callback, &a, 0),
			"client for each, offset = 0");
	pr_info("%d\n", a);
	success &= ASSERT_BOOL(true, match_in6_addrs(), "Match in6 nodes");
	pr_info("\n");

	// client; offset = 2, arg = NULL; it starts from 3rd
	success &= ASSERT_INT(0, client_for_each(&client, callback, NULL, 2),
			"client for each, offset = 2");
	success &= ASSERT_BOOL(true, match_in6_addrs(), "Match in6 nodes");
	pr_info("\n");

	// client; offset = 80, arg = NULL; it starts from 6th
	success &= ASSERT_INT(0, client_for_each(&client, callback, NULL, 80),
			"client for each, offset = 80");
	success &= ASSERT_BOOL(true, match_in6_addrs(), "Match in6 nodes");
	pr_info("\n");

	// client; When cb gets addr 14

	success &= ASSERT_INT(0, client_flush(&client), "Cleanning client");

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
	prefix2.address.s6_addr[15] = 14; /* Adding addr 14 */

	prefix3.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix3.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix3.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix3.address.s6_addr32[3] = cpu_to_be32(0x0003);
	prefix3.len = 128;

	success &= ASSERT_INT(0, client_add(&client, &prefix0), "add prefix 0");
	success &= ASSERT_BOOL(true, client_addr_exist(&client,
			&prefix0.address), "exist prefix0 test");

	success &= ASSERT_INT(0, client_add(&client, &prefix1), "add prefix 1");
	success &= ASSERT_BOOL(true, client_addr_exist(&client,
			&prefix1.address), "exist prefix1 test");

	success &= ASSERT_INT(0, client_add(&client, &prefix2), "add prefix 2");
	success &= ASSERT_BOOL(true, client_addr_exist(&client,
			&prefix2.address), "exist prefix2 test");

	success &= ASSERT_INT(0, client_add(&client, &prefix3), "add prefix 3");
	success &= ASSERT_BOOL(true, client_addr_exist(&client,
			&prefix3.address), "exist prefix3 test");

	pr_info("\n");
	client_print_all(&client);
	pr_info("\n");

	success &= ASSERT_INT(0, fill_exp_in6_addr(), "Fill exp in6 addrs");

	// client; when cb gets addr 14, offset = 0, it starts from 1st
	success &= ASSERT_INT(1, client_for_each(&client, callback, NULL, 0),
			"Stops at addr 14, offset = 0");
	success &= ASSERT_BOOL(true, match_in6_addrs(), "Match in6 nodes");

	// client; when cb gets addr 14, offset = 1, it starts from 2nd
	a = 0;
	pr_info("\n%d\n", a);
	success &= ASSERT_INT(1, client_for_each(&client, callback, &a, 1),
			"Stops at addr 14, offset = 1");
	pr_info("%d\n\n", a);
	success &= ASSERT_BOOL(true, match_in6_addrs(), "Match in6 nodes");

	return success;
}

static bool mask_domain_test(void)
{
	bool success = true;
	struct client_mask_domain domain;
	struct client_mask_domain expected;
	struct ipv6_prefix prefix7;

	pr_info("\n");
	success &= ASSERT_INT(0, pool4_print_all(&cpool), "Actual pool4 nodes");
	pr_info("\n");

	expected.first.l3.s_addr = cpu_to_be32(0xc0000201);
	expected.first.l4 = 4;
	expected.step = 1;
	expected.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&expected, &client, &cpool,
			&prefix0.address, &domain, 10), "1st addr domain");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	expected.first.l3.s_addr = cpu_to_be32(0xc0000202);
	expected.first.l4 = 16;
	expected.step = 1;
	expected.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&expected, &client, &cpool,
			&prefix1.address, &domain, 10), "2nd addr domain");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	expected.first.l3.s_addr = cpu_to_be32(0xc0000203);
	expected.first.l4 = 30;
	expected.step = 1;
	expected.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&expected, &client, &cpool,
			&prefix2.address, &domain, 10), "3rd addr domain");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	expected.first.l3.s_addr = cpu_to_be32(0xc0000204);
	expected.first.l4 = 40;
	expected.step = 1;
	expected.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&expected, &client, &cpool,
			&prefix3.address, &domain, 10), "4th addr domain");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	expected.first.l3.s_addr = cpu_to_be32(0xc0000204);
	expected.first.l4 = 50;
	expected.step = 1;
	expected.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&expected, &client, &cpool,
			&prefix4.address, &domain, 10), "5th addr domain");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	expected.first.l3.s_addr = cpu_to_be32(0xc0000204);
	expected.first.l4 = 60;
	expected.step = 1;
	expected.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&expected, &client, &cpool,
			&prefix5.address, &domain, 10), "6th addr domain");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	expected.first.l3.s_addr = cpu_to_be32(0xc0000205);
	expected.first.l4 = 71;
	expected.step = 1;
	expected.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&expected, &client, &cpool,
			&prefix6.address, &domain, 10), "7th addr domain");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);
	pr_info("\n");

	// If are more clients than pool4 addresses
	prefix7.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix7.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix7.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix7.address.s6_addr32[3] = cpu_to_be32(0x0007);
	prefix7.len = 128;

	success &= ASSERT_INT(0, client_add(&client, &prefix7), "add prefix 7");
	success &= ASSERT_BOOL(true, client_exist(&client, &prefix7),
			"exist prefix7");

	success &= ASSERT_BOOL(false, match_domain(&expected, &client, &cpool,
			&prefix7.address, &domain, 10), "8th addr domain");
	pr_info("\n");

	success &= ASSERT_INT(0, client_delete(&client, &prefix7),
			"delete prefix7");
	success &= ASSERT_BOOL(false, client_addr_exist(&client,
			&prefix7.address), "exist prefix7");

	//if client addr doesn't exist in client database
	success &= ASSERT_INT(-ESRCH, client_get_mask_domain(&client, &cpool,
			&prefix7.address, &domain, 10), "addr not contained");

	success &= ASSERT_BOOL(false, match_domain(&expected, &client, &cpool,
			&prefix7.address, &domain, 10), "addr does not exist\n");

	prefix7.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix7.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix7.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix7.address.s6_addr32[3] = cpu_to_be32(0x0008);

	success &= ASSERT_INT(-ESRCH, client_get_mask_domain(&client, &cpool,
			&prefix7.address, &domain, 10), "addr not contained");

	success &= ASSERT_BOOL(false, match_domain(&expected, &client, &cpool,
			&prefix7.address, &domain, 10), "addr does not exist\n");

	prefix7.address.s6_addr32[0] = cpu_to_be32(0x2001);
	prefix7.address.s6_addr32[1] = cpu_to_be32(0x0db8);
	prefix7.address.s6_addr32[2] = cpu_to_be32(0x0000);
	prefix7.address.s6_addr32[3] = cpu_to_be32(0x0009);

	success &= ASSERT_INT(-ESRCH, client_get_mask_domain(&client, &cpool,
			&prefix7.address, &domain, 10), "addr not contained");

	success &= ASSERT_BOOL(false, match_domain(&expected, &client, &cpool,
			&prefix7.address, &domain, 10), "addr does not exist\n");

	//if 0 is given for masks_per_client
	success &= ASSERT_BOOL(false, match_domain(&expected, &client, &cpool,
			&prefix6.address, &domain, 0), "0 in masks_per_client");
	pr_info("\n");

	return success;
}

static bool get_nth_taddr_test(void)
{
	bool success = true;
	struct client_mask_domain domain;
	struct client_mask_domain dom_exp;
	struct ipv4_transport_addr result;
	struct ipv4_transport_addr taddr_exp;

	pr_info("\n");
	success &= ASSERT_INT(0, pool4_print_all(&cpool), "print all test");
	pr_info("\n");

	// Domain of 1st prefix address
	dom_exp.first.l3.s_addr = cpu_to_be32(0xc0000201);
	dom_exp.first.l4 = 4;
	dom_exp.step = 1;
	dom_exp.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&dom_exp, &client, &cpool,
			&prefix0.address, &domain, 10), "1st addr domain");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000201);
	taddr_exp.l4 = 4;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			0, &result), "Get 1st taddr");
	pr_info("1st taddr: %pI4: %u\n", &result.l3, result.l4);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000201);
	taddr_exp.l4 = 5;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			1, &result), "Get 2nd taddr");
	pr_info("2nd taddr: %pI4: %u\n", &result.l3, result.l4);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000201);
	taddr_exp.l4 = 6;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			2, &result), "Get 3rd taddr");
	pr_info("3rd taddr: %pI4: %u\n", &result.l3, result.l4);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000201);
	taddr_exp.l4 = 7;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			3, &result), "Get 4th taddr");
	pr_info("4th taddr: %pI4: %u\n", &result.l3, result.l4);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000202);
	taddr_exp.l4 = 10;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			4, &result), "Get 5th taddr");
	pr_info("5th taddr: %pI4: %u\n", &result.l3, result.l4);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000202);
	taddr_exp.l4 = 11;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			5, &result), "Get 6th taddr");
	pr_info("6th taddr: %pI4: %u\n", &result.l3, result.l4);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000202);
	taddr_exp.l4 = 12;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			6, &result), "Get 7th taddr");
	pr_info("7th taddr: %pI4: %u\n", &result.l3, result.l4);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000202);
	taddr_exp.l4 = 13;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			7, &result), "Get 8th taddr");
	pr_info("8th taddr: %pI4: %u\n", &result.l3, result.l4);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000202);
	taddr_exp.l4 = 14;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			8, &result), "Get 9th taddr");
	pr_info("9th taddr: %pI4: %u\n", &result.l3, result.l4);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000202);
	taddr_exp.l4 = 15;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			9, &result), "Get 10th taddr");
	pr_info("10th taddr: %pI4: %u\n", &result.l3, result.l4);
	pr_info("\n");

	//Domain of 2nd prefix address
	dom_exp.first.l3.s_addr = cpu_to_be32(0xc0000202);
	dom_exp.first.l4 = 16;
	dom_exp.step = 1;
	dom_exp.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&dom_exp, &client, &cpool,
			&prefix1.address, &domain, 10), "2nd addr domain");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000203);
	taddr_exp.l4 = 25;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			5, &result), "Get 5th taddr");
	pr_info("5th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	//Domain of 3rd address
	dom_exp.first.l3.s_addr = cpu_to_be32(0xc0000203);
	dom_exp.first.l4 = 30;
	dom_exp.step = 1;
	dom_exp.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&dom_exp, &client, &cpool,
			&prefix2.address, &domain, 10), "3rd addr domain");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000203);
	taddr_exp.l4 = 33;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			3, &result), "Get 3rd taddr");
	pr_info("3rd taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	//Domain of 4th prefix address
	dom_exp.first.l3.s_addr = cpu_to_be32(0xc0000204);
	dom_exp.first.l4 = 40;
	dom_exp.step = 1;
	dom_exp.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&dom_exp, &client, &cpool,
			&prefix3.address, &domain, 10), "4th addr domain");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000204);
	taddr_exp.l4 = 48;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			8, &result), "Get 8th taddr");
	pr_info("8th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	//Domain of 5th prefix address
	dom_exp.first.l3.s_addr = cpu_to_be32(0xc0000204);
	dom_exp.first.l4 = 50;
	dom_exp.step = 1;
	dom_exp.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&dom_exp, &client, &cpool,
			&prefix4.address, &domain, 10), "5th addr domain");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000204);
	taddr_exp.l4 = 57;
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			7, &result), "Get 7th taddr");
	pr_info("7th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	//Domain of 6th prefix address
	dom_exp.first.l3.s_addr = cpu_to_be32(0xc0000204);
	dom_exp.first.l4 = 60;
	dom_exp.step = 1;
	dom_exp.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&dom_exp, &client, &cpool,
			&prefix5.address, &domain, 10), "6th addr domain");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000205);
	taddr_exp.l4 = 67;
	success &=ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			6, &result), "Get 6th taddr");
	pr_info("6th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	//Domain of 7th prefix address
	dom_exp.first.l3.s_addr = cpu_to_be32(0xc0000205);
	dom_exp.first.l4 = 71;
	dom_exp.step = 1;
	dom_exp.count = 10;

	success &= ASSERT_BOOL(true, match_domain(&dom_exp, &client, &cpool,
			&prefix6.address, &domain, 10), "7th addr domain");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	taddr_exp.l3.s_addr = cpu_to_be32(0xc0000205);
	taddr_exp.l4 = 73;

	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool, &domain,
			2, &result), "Get 3rd taddr");
	pr_info("2nd taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	//If n > domain.count, continues counting from first address
	success &= ASSERT_BOOL(true, match_nth_taddr(&taddr_exp, &cpool,
			&domain, 12, &result), "n > domain.count");
	pr_info("13th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	//When gets a domain address that does not exist in pool4
	domain.first.l3.s_addr = cpu_to_be32(0xc0000208);
	domain.first.l4 = 71;
	domain.step = 1;
	domain.count = 10;

	success &= ASSERT_INT(-ESRCH, pool4_get_nth_taddr(&cpool, &domain, 1,
			&result), "domain addr non existent");

	//When gets a range that does not correspond to an address
	domain.first.l3.s_addr = cpu_to_be32(0xc0000201);
	domain.first.l4 = 71;
	domain.step = 1;
	domain.count = 10;

	success &= ASSERT_INT(-ESRCH, pool4_get_nth_taddr(&cpool, &domain, 1,
			&result), "range out of address ports");

	return success;
}

static bool get_mask_test(void)
{
	bool success = true;
	struct ipv4_transport_addr mask;
	struct ipv4_transport_addr exp_mask;
	struct packet packet;

	pr_info("\n");
	success &= ASSERT_INT(0, pool4_print_all(&cpool), "print pool4s");
	pr_info("\n");

	success &= ASSERT_INT(0, client_print_all(&client), "print clients");
	pr_info("\n");

	packet.hdr = kmalloc(sizeof(struct ipv6hdr), GFP_KERNEL);
	if (!packet.hdr)
		return -ENOMEM;

	//Get mask for 1st prefix address
	packet.hdr->saddr = prefix0.address;
	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0xc0000201);
	exp_mask.l4 = 4;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 10), "1st addr mask");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	//Get mask for 2nd prefix address, domain not available, gets spool
	packet.hdr->saddr = prefix1.address;
	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0x00000000);
	exp_mask.l4 = 0;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 3), "2nd addr mask");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	//Mask for 3rd prefix address
	packet.hdr->saddr = prefix2.address;
	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0xc0000203);
	exp_mask.l4 = 32;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 11), "3rd addr mask");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	//Mask for 4th prefix address, domain not available, gets spool
	packet.hdr->saddr = prefix3.address;
	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0x00000000);
	exp_mask.l4 = 0;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 5), "4th addr mask");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	//Mask for 5th prefix address
	packet.hdr->saddr = prefix4.address;
	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0xc0000204);
	exp_mask.l4 = 58;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 12), "5th addr mask");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	//Mask for 6th prefix address
	packet.hdr->saddr = prefix5.address;
	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0xc0000202);
	exp_mask.l4 = 16;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 2), "6th addr mask");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	//Mask for 7th prefix address
	packet.hdr->saddr = prefix6.address;
	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0xc0000203);
	exp_mask.l4 = 34;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 4), "7th addr mask");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	//When client addresses does not exist on database
	pr_info("Dynamic assignment\n\n");
	success &= ASSERT_INT(0, client_flush(&client), "Flushing cpool");

	/* 1st address */
	packet.hdr->saddr.s6_addr32[0] = cpu_to_be32(0x2001);
	packet.hdr->saddr.s6_addr32[1] = cpu_to_be32(0x0db8);
	packet.hdr->saddr.s6_addr32[2] = cpu_to_be32(0x0000);
	packet.hdr->saddr.s6_addr32[3] = cpu_to_be32(0x0010);

	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0xc0000201);
	exp_mask.l4 = 4;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 10), "dynamic assignment");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	/* 2nd address */
	packet.hdr->saddr.s6_addr32[0] = cpu_to_be32(0x2001);
	packet.hdr->saddr.s6_addr32[1] = cpu_to_be32(0x0db8);
	packet.hdr->saddr.s6_addr32[2] = cpu_to_be32(0x0000);
	packet.hdr->saddr.s6_addr32[3] = cpu_to_be32(0x0011);

	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0xc0000202);
	exp_mask.l4 = 16;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 10), "dynamic assignment");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	/* 3rd address */
	packet.hdr->saddr.s6_addr32[0] = cpu_to_be32(0x2001);
	packet.hdr->saddr.s6_addr32[1] = cpu_to_be32(0x0db8);
	packet.hdr->saddr.s6_addr32[2] = cpu_to_be32(0x0000);
	packet.hdr->saddr.s6_addr32[3] = cpu_to_be32(0x0012);

	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0xc0000203);
	exp_mask.l4 = 30;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 10), "dynamic assignment");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	success &= ASSERT_INT(0, client_print_all(&client), "print client");
		pr_info("\n");

	//When pool4 is empty, gets spool
	pr_info("Gets spool cause cpool is empty\n\n");
	success &= ASSERT_INT(0, pool4_flush(&cpool), "Flushing cpool");

	/* 1st address */
	packet.hdr->saddr.s6_addr32[0] = cpu_to_be32(0x2001);
	packet.hdr->saddr.s6_addr32[1] = cpu_to_be32(0x0db8);
	packet.hdr->saddr.s6_addr32[2] = cpu_to_be32(0x0000);
	packet.hdr->saddr.s6_addr32[3] = cpu_to_be32(0x0010);

	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0x00000000);
	exp_mask.l4 = 0;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 4), "when pool4 is empty");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	/* 2nd address */
	packet.hdr->saddr.s6_addr32[0] = cpu_to_be32(0x2001);
	packet.hdr->saddr.s6_addr32[1] = cpu_to_be32(0x0db8);
	packet.hdr->saddr.s6_addr32[2] = cpu_to_be32(0x0000);
	packet.hdr->saddr.s6_addr32[3] = cpu_to_be32(0x0011);

	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0x00000000);
	exp_mask.l4 = 0;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 4), "when pool4 is empty");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	/* 3rd address */
	packet.hdr->saddr.s6_addr32[0] = cpu_to_be32(0x2001);
	packet.hdr->saddr.s6_addr32[1] = cpu_to_be32(0x0db8);
	packet.hdr->saddr.s6_addr32[2] = cpu_to_be32(0x0000);
	packet.hdr->saddr.s6_addr32[3] = cpu_to_be32(0x0012);

	pr_info("%x.%x.%x.%x\n", be32_to_cpu(packet.hdr->saddr.s6_addr32[0]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[1]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[2]),
			be32_to_cpu(packet.hdr->saddr.s6_addr32[3]));

	exp_mask.l3.s_addr = cpu_to_be32(0x00000000);
	exp_mask.l4 = 0;

	success &= ASSERT_BOOL(true, valid_mask(&exp_mask, &packet, &cpool,
			&spool, &client, &mask, 4), "when pool4 is empty");
	pr_info("%pI4: %u\n\n", &mask.l3, mask.l4);

	kfree(packet.hdr);
	return success;
}

static int nat64_init(void)
{

	START_TESTS("pool4 test");

	INIT_CALL_END(init(), remove_entries(), end(), "remove functions");
	INIT_CALL_END(init(), flush_dbases(), end(), "flush functions");
	INIT_CALL_END(init(), count_addr(), end(), "count functions");
	INIT_CALL_END(init(), print_all_addr(), end(), "print functions");
	INIT_CALL_END(init(), elements_exist(), end(), "entry exist functions");
	INIT_CALL_END(init(), foreach_sample(), end(), "for each sample");
	INIT_CALL_END(init(), for_each_addr(), end(), "for each addr");
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
