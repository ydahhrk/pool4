#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <stdbool.h>
#include <linux/list.h>
#include "pool4/pool4.h"
#include "unit_test.h"

// Global variables

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

//static bool is_valid_taddr(struct pool4 *cpool, struct in_addr *addr)
//{
//	bool valid_taddr = true;
//	struct pool4_entry *listed;
//	struct list_head *iter;
//	struct list_head *tmp;
//
//	list_for_each_safe(iter, tmp, &cpool->list)
//	{
//		listed = list_entry(iter, struct pool4_entry, list_hook);
//		if (listed->addr.s_addr == addr->s_addr)
//			return valid_taddr;
//	}
//
//	return false;
//}

static bool init(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;

	success &= ASSERT_INT(0, pool4_init(cpool), "Initializing cpool");
	success &= ASSERT_INT(0, pool4_init(spool), "Initializing spool");
	success &= ASSERT_INT(0, client_init(client), "Initializing client");

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
			pool4_add(cpool, one.mark, one.proto, &one.addr,
					&one.range), "add one test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, two.mark, two.proto, &two.addr,
					&two.range), "add two test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, three.mark, three.proto,
					&three.addr, &three.range),
			"add three test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, four.mark, four.proto, &four.addr,
					&four.range), "add four test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, five.mark, five.proto, &five.addr,
					&five.range), "add five test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, six.mark, six.proto, &six.addr,
					&six.range), "add six test");
	success &= ASSERT_INT(0,
			pool4_add(cpool, seven.mark, seven.proto,
					&seven.addr, &seven.range),
			"add seven test");

	// Filling client database...

	success &= ASSERT_INT(0, client_add(client, &prefix0), "add 0 test");
	success &= ASSERT_INT(0, client_add(client, &prefix1), "add 1 test");
	success &= ASSERT_INT(0, client_add(client, &prefix2), "add 2 test");
	success &= ASSERT_INT(0, client_add(client, &prefix3), "add 3 test");
	success &= ASSERT_INT(0, client_add(client, &prefix4), "add 4 test");
	success &= ASSERT_INT(0, client_add(client, &prefix5), "add 5 test");
	success &= ASSERT_INT(0, client_add(client, &prefix6), "add 6 test");

	return success;
}

static void end(struct pool4 *cpool, struct pool4 *spool, struct client *client)
{
	pool4_flush(cpool);
	pool4_flush(spool);
	client_flush(client);
}

static bool remove_entries(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;

	/* Removing 5 elements from cpool... */

	success &= ASSERT_INT(0,
			pool4_rm(cpool, one.mark, one.proto, &one.addr,
					&one.range), "pool4_rm test");
	success &= ASSERT_INT(0,
			pool4_rm(cpool, two.mark, two.proto, &two.addr,
					&two.range), "pool4_rm test");
	success &= ASSERT_INT(0,
			pool4_rm(cpool, three.mark, three.proto, &three.addr,
					&three.range), "pool_rm test");
	success &= ASSERT_INT(0,
			pool4_rm(cpool, four.mark, four.proto, &four.addr,
					&four.range), "pool4_rm test");
	success &= ASSERT_INT(0,
			pool4_rm(cpool, five.mark, five.proto, &five.addr,
					&five.range), "pool4_rm test");

	success &= ASSERT_INT(0, pool4_print_all(cpool), "print test");
	pr_info("\n");

	/* Removing 5 elements from client... */

	success &= ASSERT_INT(0, client_delete(client, &prefix0),
			"client_delete test");
	success &= ASSERT_INT(0, client_delete(client, &prefix1),
			"client_delete test");
	success &= ASSERT_INT(0, client_delete(client, &prefix2),
			"client_delete test");
	success &= ASSERT_INT(0, client_delete(client, &prefix3),
			"client_delete test");
	success &= ASSERT_INT(0, client_delete(client, &prefix4),
			"client_delete test");

	success &= ASSERT_INT(0, client_print_all(client), "print test");
	pr_info("\n");

	return success;
}

static bool count_addr(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;

	int cpool_addrs = 0;
	int client_addrs = 0;

	success &= ASSERT_BOOL(false, pool4_is_empty(cpool), "is empty test");

	cpool_addrs = pool4_count(cpool);
	client_addrs = client_count(client);

	pr_info("\nAddresses in cpool: %d\n", cpool_addrs);
	pr_info("Addresses in client: %d\n\n", client_addrs);

	return success;
}

static bool print_all_addr(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;

	pr_info("\n");
	success &= ASSERT_INT(0, pool4_print_all(cpool), "print pool4 test");

	success &= ASSERT_INT(0, client_print_all(client), "print client test");
	pr_info("\n");

	return success;
}

static bool addr_exist(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;

	pr_info("\n");
	success &= pool4_contains(cpool, one.mark, one.proto, &one.addr,
			&one.range);
	success &= pool4_contains(cpool, two.mark, two.proto, &two.addr,
			&two.range);
	success &= pool4_contains(cpool, three.mark, three.proto,
			&three.addr, &three.range);

	success &= ASSERT_INT(0, client_addr_exist(client, &prefix0.address),
			"exist prefix0 test");
	success &= ASSERT_INT(0, client_addr_exist(client, &prefix1.address),
			"exist prefix1 test");
	success &= ASSERT_INT(0, client_addr_exist(client, &prefix2.address),
			"exist prefix2 test");

	return success;
}

static bool foreach_sample(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;
	int a = 12;

	/* pool4_foreach_sample test */

	pr_info("\n%d\n", a);
	success &= ASSERT_INT(0, pool4_foreach_sample(cpool, callb, &a, NULL),
			"pool4 for each sample, offset = NULL");
	pr_info("%d\n", a);
	pr_info("\n");

	success &= ASSERT_INT(0, pool4_foreach_sample(cpool, callb, NULL, &two),
			"pool4 for each sample, offset with value");
	pr_info("\n");

	/* client_foreach_sample test */

	pr_info("%d\n", a);
	success &= ASSERT_INT(0, client_for_eachsample(client, cb, &a, NULL),
			"client for each sample, offset = NULL");
	pr_info("%d\n", a);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_for_eachsample(client, cb, NULL, &prefix1),
			"client for each sample, offset with value");
	pr_info("\n");

	return success;
}

static bool for_each_addr(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;
	int a = 0;

	/* pool4_foreach_taddr4 test */

	pr_info("\n%d\n", a);
	success = ASSERT_INT(0, pool4_foreach_taddr4(cpool, cback, &a, 0),
			"pool4 for each taddr4, offset = 0");
	pr_info("%d\n", a);
	pr_info("\n");

	success = ASSERT_INT(0, pool4_foreach_taddr4(cpool, cback, NULL, 6),
			"pool4 for each taddr4, offset = 6");

	/* client_for_each test */

	a = 12;

	pr_info("\n%d\n", a);
	success = ASSERT_INT(0, client_for_each(client, callback, &a, 0),
			"client for each, offset = 0");
	pr_info("%d\n", a);
	pr_info("\n");

	success = ASSERT_INT(0, client_for_each(client, callback, NULL, 2),
			"client for each, offset = 2");
	pr_info("\n");

	return success;
}

static bool client_get_mask_domain_test(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;
	struct client_mask_domain domain;

	/* Testing client_get_mask_domain */

	pr_info("\n");
	success &= ASSERT_INT(0, pool4_print_all(cpool), "print all test");
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix0.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix1.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix2.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix3.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix4.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix5.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix6.address,
					&domain, 10), "get mask domain test");
	pr_info("%pI4: %u %u %u\n", &domain.first.l3, domain.first.l4,
			domain.step, domain.count);
	pr_info("\n");

	return success;
}

static bool pool4_get_nth_taddr_test(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;
	struct client_mask_domain domain;
	struct ipv4_transport_addr result;

	pr_info("\n");
	success &= ASSERT_INT(0, pool4_print_all(cpool), "print all test");
	pr_info("\n");

	/* pool4_get_nth_taddr test */

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix0.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(cpool, &domain, 9, &result),
			"Asking for the 9th taddr...");
//	success &= ASSERT_BOOL(true, get_valid_nth_taddr(cpool, &domain, 3,
//			&result), "Asking for the 3rd taddr...");
	pr_info("9th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix1.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(cpool, &domain, 5, &result),
			"Asking for the 5th taddr...");
//	success &= ASSERT_BOOL(true, get_valid_nth_taddr(cpool, &domain, 3,
//			&result), "Asking for the 3rd taddr...");
	pr_info("5th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix2.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(cpool, &domain, 3, &result),
			"Asking for the 3rd taddr...");
//	success &= ASSERT_BOOL(true, get_valid_nth_taddr(cpool, &domain, 3,
//			&result), "Asking for the 3rd taddr...");
	pr_info("3rd taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix3.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(cpool, &domain, 8, &result),
			"Asking for the 8th taddr...");
//	success &= ASSERT_BOOL(true, get_valid_nth_taddr(cpool, &domain, 3,
//			&result), "Asking for the 3rd taddr...");
	pr_info("8th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix4.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(cpool, &domain, 7, &result),
			"Asking for the 7th taddr...");
//	success &= ASSERT_BOOL(true, get_valid_nth_taddr(cpool, &domain, 3,
//			&result), "Asking for the 3rd taddr...");
	pr_info("7th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix5.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(cpool, &domain, 6, &result),
			"Asking for the 6th taddr...");
//	success &= ASSERT_BOOL(true, get_valid_nth_taddr(cpool, &domain, 3,
//			&result), "Asking for the 3rd taddr...");
	pr_info("6th taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");

	success &= ASSERT_INT(0,
			client_get_mask_domain(client, cpool, &prefix6.address,
					&domain, 10), "get mask domain test");
	pr_info("Mask domain: %pI4: %u %u %u\n", &domain.first.l3,
			domain.first.l4, domain.step, domain.count);

	success &= ASSERT_INT(0,
			pool4_get_nth_taddr(cpool, &domain, 2, &result),
			"Asking for the 2nd taddr...");
//	success &= ASSERT_BOOL(true, get_valid_nth_taddr(cpool, &domain, 3,
//			&result), "Asking for the 3rd taddr...");
	pr_info("2nd taddr: %pI4: %u", &result.l3, result.l4);
	pr_info("\n");


	return success;
}

static bool get_mask_test(struct pool4 *cpool, struct pool4 *spool,
		struct client *client)
{
	bool success = true;
	struct ipv4_transport_addr result;
	struct packet packet;

	pr_info("\n");
	success &= ASSERT_INT(0, pool4_print_all(cpool), "print all test");
	pr_info("\n");

	success &= ASSERT_INT(0, client_print_all(client), "print all test");
	pr_info("\n");

	packet.hdr = kmalloc(sizeof(struct ipv6hdr), GFP_KERNEL);
	if (!packet.hdr) {
			return -ENOMEM;
		}

	packet.hdr->saddr = prefix6.address;

	success &= ASSERT_INT(0,
			get_mask(&packet, cpool, spool, client, &result, 1),
			"get mask test");
	pr_info("%pI4: %u\n\n", &result.l3, result.l4);

	packet.hdr->saddr = prefix0.address;

	success &= ASSERT_INT(0,
			get_mask(&packet, cpool, spool, client, &result, 8),
			"get mask test");
	pr_info("%pI4: %u\n\n", &result.l3, result.l4);

	packet.hdr->saddr = prefix2.address;

	success &= ASSERT_INT(0,
			get_mask(&packet, cpool, spool, client, &result, 5),
			"get mask test");
	pr_info("%pI4: %u\n\n", &result.l3, result.l4);

	kfree(packet.hdr);
	return success;
}

static int nat64_init(void)
{

	struct pool4 cpool;
	struct pool4 spool;
	struct client client;

	START_TESTS("pool4 test");

	INIT_CALL_END(init(&cpool, &spool, &client),
			remove_entries(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "remove functions");
	INIT_CALL_END(init(&cpool, &spool, &client),
			count_addr(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "count functions");
	INIT_CALL_END(init(&cpool, &spool, &client),
			print_all_addr(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "print functions");
	INIT_CALL_END(init(&cpool, &spool, &client),
			addr_exist(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "entry exist functions");
	INIT_CALL_END(init(&cpool, &spool, &client),
			foreach_sample(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "for each sample");
	INIT_CALL_END(init(&cpool, &spool, &client),
			for_each_addr(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "for each addr");
	INIT_CALL_END(init(&cpool, &spool, &client),
			client_get_mask_domain_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "get mask domain");
	INIT_CALL_END(init(&cpool, &spool, &client),
			pool4_get_nth_taddr_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "get nth taddr");
	INIT_CALL_END(init(&cpool, &spool, &client),
			get_mask_test(&cpool, &spool, &client),
			end(&cpool, &spool, &client), "get mask");

	END_TESTS;

	return 0;
}

static void nat64_exit(void)
{

}

module_init( nat64_init);
module_exit( nat64_exit);
