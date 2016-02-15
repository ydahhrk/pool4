#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include "pool4/pool4.h"

int callb(struct pool4_entry *entry, void *arg)
{
	int *arg_int = arg;
	char addr[16];

	if (arg) {
		printk("%s %u-%u %d\n", ip4_to_str(entry->addr.s_addr, addr),
				entry->range.min, entry->range.max, *arg_int);
		(*arg_int)++;
	} else {
		printk("%s %u-%u\n", ip4_to_str(entry->addr.s_addr, addr),
				entry->range.min, entry->range.max);
	}

	return 0;
}

int cback(struct pool4_mask *mask, void *arg)
{
	int *arg_int = arg;
	char addr[16];

	if (arg) {
		printk("%s %u %d\n", ip4_to_str(mask->addr.s_addr, addr),
				mask->port, *arg_int);
		(*arg_int)++;
	} else {
		printk("%s %u\n", ip4_to_str(mask->addr.s_addr, addr),
				mask->port);
	}

	if (mask->port == 64)
		return 1; /* Interrumpir temprano. */

	return 0;
}

int  cb(struct ipv6_prefix *prefix, void *arg)
{
	int *arg_int = arg;
	if (arg) {
		printk("Address: %x:%x:%x:%x\nLength:%u  %d\n",
				prefix->address.s6_addr32[0],
				prefix->address.s6_addr32[1],
				prefix->address.s6_addr32[2],
				prefix->address.s6_addr32[3],
				prefix->len, *arg_int);
		(*arg_int)++;
	} else {
		printk("Address: %x:%x:%x:%x\nLength:%u\n",
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
		printk("%x:%x:%x:%x %d\n",
				addr->s6_addr32[0],
				addr->s6_addr32[1],
				addr->s6_addr32[2],
				addr->s6_addr32[3],
				*arg_int);
		(*arg_int)++;
	} else {
		printk("%x:%x:%x:%x\n",
				addr->s6_addr32[0],
				addr->s6_addr32[1],
				addr->s6_addr32[2],
				addr->s6_addr32[3]);
	}

	if (addr->s6_addr[15] == 14)
		return 1; /* Interrumpir temprano. */

	return 0;
}


static int __init nat64_init(void)
{
		return 0;
}

static void __exit nat64_exit(void)
{

}

module_init(nat64_init);
module_exit(nat64_exit);
