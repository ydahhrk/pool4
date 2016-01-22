#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../types.h"
#include "../list.h"
#include "../errno.h"
#include "pool4.h"

struct list_head pool4_list;

void pool4_init()
{
	INIT_LIST_HEAD(&pool4_list);
}

int pool4_add(__u32 mark, __u8 proto,  struct in_addr *addr,
		struct port_range *range)
{
	struct pool4_entry *add;
	add = malloc(sizeof(struct pool4_entry));
	if (!add) {
		return -ENOMEM;
	}

	add->mark = mark;
	add->proto = proto;
	add->addr.s_addr = addr->s_addr;
	add->range.min = range->min;
	add->range.max = range->max;
	list_add_tail(&(add->list_hook), &(pool4_list));

	return 0;
}

static bool pool4_equals(struct pool4_entry *one, struct pool4_entry *two)
{
	if (one->mark == two->mark && one->proto == two->proto &&
			one->addr.s_addr == two->addr.s_addr &&
			one->range.max == two->range.max &&
			one->range.min == two->range.min) {
		return true;
	} else {
		return false;
	}
}

static bool pool4_compare(struct pool4_entry *one, struct pool4_entry *two)
{
	if (one->proto == two->proto && one->addr.s_addr == two->addr.s_addr) {
		return true;
	} else {
		return false;
	}
}

int pool4_rm(__u32 mark, __u8 proto, struct in_addr *addr,
		struct port_range *range)
{
	struct list_head *iter;
	struct list_head *tmp;
	struct pool4_entry rm;
	struct pool4_entry *exist;

	rm.mark = mark;
	rm.proto = proto;
	rm.addr.s_addr = addr->s_addr;
	rm.range.min = range->min;
	rm.range.max = range->max;

	list_for_each_safe(iter, tmp, &pool4_list) {
		exist = list_entry(iter, struct pool4_entry, list_hook);
		if (pool4_equals(&rm, exist)) {
			list_del(&exist->list_hook);
			free(exist);
		}
	}

	return 0;
}

int pool4_flush(void)
{
	struct list_head *iter;
	struct list_head *tmp;
	struct pool4_entry *entry;

	list_for_each_safe(iter, tmp, &pool4_list) {
		entry = list_entry(iter, struct pool4_entry, list_hook);
		list_del(iter);
		free(entry);
	}
	return 0;
}

bool pool4_is_empty(void)
{
	if (!list_empty(&pool4_list)){
		printf("It is not empty.\n\n");
		return false;
	} else {
		printf("It is empty.\n\n");
		return true;
	}
}

void pool4_print_all(void)
{
	printf("Elements in the list:\n\n");
	struct list_head *iter;
	struct list_head *tmp;
	struct pool4_entry *entry;
	char addr[16];

	list_for_each_safe(iter, tmp, &pool4_list) {
		entry = list_entry(iter, struct pool4_entry, list_hook);
		printf("%u, ", entry->mark);
		printf("%u, ", entry->proto);
		printf("%s, ", ip4_to_str(entry->addr.s_addr, addr));
		printf("%u-%u\n", entry->range.min, entry->range.max);
	}
}

bool pool4_contains(__u32 mark, __u8 proto, struct in_addr *addr,
		struct port_range *range)
{
	struct list_head *iter;
	struct list_head *tmp;
	struct pool4_entry requested;
	struct pool4_entry *listed;

	requested.mark = mark;
	requested.proto = proto;
	requested.addr.s_addr = addr->s_addr;
	requested.range.min = range->min;
	requested.range.max = range->max;

	list_for_each_safe(iter, tmp, &pool4_list) {
		listed = list_entry(iter, struct pool4_entry, list_hook);
		if (pool4_compare(&requested, listed)) {
			printf("It is in the list.\n\n");
			return true;
		}
	}

	printf("It is not in the list.\n\n");
	return false;
}

int pool4_count(void)
{

	struct list_head *iter;
	int counter = 0;

	list_for_each(iter, &pool4_list) {
		counter++;
	}

	return counter;
}

int pool4_foreach_sample(int (*cb)(struct pool4_entry *, void *), void *arg,
		struct pool4_entry *offset)
{
	struct list_head *iter;
	struct pool4_entry *tmp;
	int error = 0;

	list_for_each(iter, &pool4_list) {
		tmp = list_entry(iter, struct pool4_entry, list_hook);

		if(!offset) {
			error = cb(tmp, arg);
			if (error)
				break;
		} else if (pool4_equals(offset, tmp)) {
			offset = NULL;
		}
	}

	return offset ? -ESRCH : error;
}

int taddr4_count()
{
	struct list_head *iter;
	struct pool4_entry *entry;
	unsigned int entries = 0;
	unsigned int i;

	list_for_each(iter, &pool4_list) {
		entry = list_entry(iter, struct pool4_entry, list_hook);
		for (i = entry->range.min; i <= entry->range.max; i++) {
			entries++;
		}
	}

	return entries;
}

int pool4_foreach_taddr4(int (*cback)(struct pool4_mask *, void *), void *arg,
		unsigned int offset)
{
	struct list_head *iter;
	struct pool4_mask mask;
	struct pool4_entry *entry;
	int error = 0;
	int entries = 0;
	int indx = 0;
	unsigned int i;

	entries = taddr4_count();
	printf("%d entries\n\n", entries);
	if (offset > entries) {
		offset = offset % entries;
	}

	list_for_each(iter, &pool4_list) {
		entry = list_entry(iter, struct pool4_entry, list_hook);
		for (i = entry->range.min; i <= entry->range.max; i++) {
			mask.mark = entry->mark;
			mask.proto = entry->proto;
			mask.addr.s_addr = entry->addr.s_addr;
			mask.port = i;
			if (indx >= offset){
				error = cback(&mask, arg);
				if (error) {
					return error;
				}
			}
			indx++;
		}
	}
	indx = 0;

	list_for_each(iter, &pool4_list) {
		if (offset == 0)
			break;
		entry = list_entry(iter, struct pool4_entry, list_hook);
		for (i = entry->range.min; i <= entry->range.max; i++) {
			mask.mark = entry->mark;
			mask.proto = entry->proto;
			mask.addr.s_addr = entry->addr.s_addr;
			mask.port = i;
			if (indx < offset) {
				error = cback(&mask, arg);
				if (error) {
					return error;
				}
			}
			indx++;
		}
	}

	return 0;
}


int pool4_get_nth_taddr(struct client_mask_domain *domain,
			unsigned int n,
			struct ipv4_transport_addr *result)
{

	struct list_head *iter;
	struct pool4_entry *pool4;
	bool flag = false;
	int i;
	n = n % domain->count;
	list_for_each(iter, &pool4_list) {
		pool4 = list_entry(iter, struct pool4_entry, list_hook);
		if (((domain->first.l3.s_addr == pool4->addr.s_addr) &&
				(pool4->range.min < domain->first.l4) &&
				(pool4->range.max > domain->first.l4))
				|| flag) {
			if (!flag) {
				for (i = domain->first.l4;
					i <= pool4->range.max;
					i += domain->step) {
					if (!n) {
						result->l3 = pool4->addr;
						result->l4 = i;
						return 0;
					}
					n--;
				}
				flag = true;
			}
			else{
				for (i = pool4->range.min;
						i <= pool4->range.max;
						i += domain->step) {
					if (!n) {
						result->l3 = pool4->addr;
						result->l4 = i;
						return 0;
						}
					n--;
				}
			}
		}
	}

	return -ESRCH;
}

int ipv4_transport_addr(struct packet *packet)
{


return 0;
}
