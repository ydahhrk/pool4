#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/slab.h>
#include "pool4/pool4.h"

//INT CALLB(STRUCT POOL4_ENTRY *ENTRY, VOID *ARG)
//{
//	INT *ARG_INT = ARG;
//	CHAR ADDR[16];
//
//	IF (ARG) {
//		PRINTK("%S %U-%U %D\N", IP4_TO_STR(ENTRY->ADDR.S_ADDR, ADDR),
//				ENTRY->RANGE.MIN, ENTRY->RANGE.MAX, *ARG_INT);
//		(*ARG_INT)++;
//	} ELSE {
//		PRINTK("%S %U-%U\N", IP4_TO_STR(ENTRY->ADDR.S_ADDR, ADDR),
//				ENTRY->RANGE.MIN, ENTRY->RANGE.MAX);
//	}
//
//	RETURN 0;
//}
//
//INT CBACK(STRUCT POOL4_MASK *MASK, VOID *ARG)
//{
//	INT *ARG_INT = ARG;
//	CHAR ADDR[16];
//
//	IF (ARG) {
//		PRINTK("%S %U %D\N", IP4_TO_STR(MASK->ADDR.S_ADDR, ADDR),
//				MASK->PORT, *ARG_INT);
//		(*ARG_INT)++;
//	} ELSE {
//		PRINTFK(%S %U\N", IP4_TO_STR(MASK->ADDR.S_ADDR, ADDR),
//				MASK->PORT);
//	}
//
//	IF (MASK->PORT == 64)
//		RETURN 1; /* INTERRUMPIR TEMPRANO. */
//
//	RETURN 0;

//int  cb(struct ipv6_prefix *prefix, void *arg)
//{
//	int *arg_int = arg;
//	if (arg) {
//		printf("Address: %x:%x:%x:%x\nLength:%u  %d\n",
//				prefix->address.s6_addr32[0],
//				prefix->address.s6_addr32[1],
//				prefix->address.s6_addr32[2],
//				prefix->address.s6_addr32[3],
//				prefix->len, *arg_int);
//		(*arg_int)++;
//	} else {
//		printf("Address: %x:%x:%x:%x\nLength:%u\n",
//				prefix->address.s6_addr32[0],
//				prefix->address.s6_addr32[1],
//				prefix->address.s6_addr32[2],
//				prefix->address.s6_addr32[3],
//				prefix->len);
//	}
//
//	return 0;
//}
//
//int callback(struct in6_addr *addr, void *arg)
//{
//	int *arg_int = arg;
//
//	if (arg) {
//		printf("%x:%x:%x:%x %d\n",
//				addr->s6_addr32[0],
//				addr->s6_addr32[1],
//				addr->s6_addr32[2],
//				addr->s6_addr32[3],
//				*arg_int);
//		(*arg_int)++;
//	} else {
//		printf("%x:%x:%x:%x\n",
//				addr->s6_addr32[0],
//				addr->s6_addr32[1],
//				addr->s6_addr32[2],
//				addr->s6_addr32[3]);
//	}
//
//	if (addr->s6_addr[15] == 14)
//		return 1; /* Interrumpir temprano. */
//
//	return 0;
//}


static int __init nat64_init(void)
{
//
//	printk("prueba");
	int error = 0;
//	struct pool4 cpool;
//	struct pool4 spool;
//
//	pool4_init(&cpool);
//	pool4_init(&spool);
//
//	struct pool4_entry *one = kmalloc(sizeof(*one), GFP_KERNEL);
//	one->mark = 1;
//	one->proto = 1;
//	one->addr.s_addr = cpu_to_be32(0xc0000201);
//	one->range.min = 10;
//	one->range.max = 20;
//
//	struct pool4_entry *two = kmalloc(sizeof(*two), GFP_KERNEL );
//	one->mark = 1;
//	one->proto = 1;
//	one->addr.s_addr = cpu_to_be32(0xc0000202);
//	one->range.min = 30;
//	one->range.max = 40;
//
//	struct pool4_entry *three = kmalloc(sizeof(*three), GFP_KERNEL);
//	one->mark = 1;
//	one->proto = 1;
//	one->addr.s_addr = cpu_to_be32(0xc0000201);
//	one->range.min = 50;
//	one->range.max = 60;
//
//	struct pool4_entry *four = kmalloc(sizeof(*four), GFP_KERNEL);
//	one->mark = 1;
//	one->proto = 1;
//	one->addr.s_addr = cpu_to_be32(0xc0000203);
//	one->range.min = 70;
//	one->range.max = 80;
//
//	pool4_add(&cpool, one->mark, one->proto, &one->addr, &one->range);


//	struct client_mask_domain domain;
//
//	struct ipv4_transport_addr mask;
//
//
//	domain.first.l3.s_addr = cpu_to_be32(0xc0000201);
//	domain.first.l4 = 55;
//	domain.step = 3;
//	domain.count = 4;
//	pool4_get_nth_taddr(&domain, 10, &mask);
//	printf("%s #%u\n\n\n", ip4_to_str(mask.l3.s_addr,addr), mask.l4);

	return error;

}

static void __exit nat64_exit(void)
{

}

module_init(nat64_init);
module_exit(nat64_exit);
