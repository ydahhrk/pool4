#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "types.h"
#include "in.h"
#include "port_range.h"
#include "list.h"

#define cpu_to_be32(x) x

typedef enum { false, true } bool;

struct pool4_entry {
	__u32 mark;
	__u8 proto;
	struct in_addr addr;
	struct port_range range;
	struct list_hook list;
} pool4_dbase;

struct pool4_entry pool4_list;

void pool4_init()
{
	INIT_LIST_HEAD(&pool4_list.list);
}

void pool4_add(__u32 mark, __u8 proto,  struct in_addr addr,
		struct port_range *range, struct pool4_entry *entry)
{
	printf("pool4_add() started running...\n");
	struct pool4_entry *tmp;
	tmp = malloc(sizeof(struct pool4_entry));
	tmp->mark = mark;
	tmp->proto = proto;
	tmp->addr.s_addr = addr.s_addr;
	tmp->range.min = range->min;
	tmp->range.max = range->max;
	list_add(&(tmp->list), &(entry->list));
}

bool pool4_entry_equals(struct pool4_entry new, struct pool4_entry old)
{
	bool equals;
	if (new.mark == old.mark && new.proto == old.proto &&
			new.addr.s_addr == old.addr.s_addr &&
			new.range.max == old.range.max &&
			new.range.min == old.range.min) {
		equals = true;
	} else {
		equals = false;
	}
	return equals;
}

void pool4_rm(__u32 mark, __u8 proto, struct in_addr addr,
		struct port_range *range, struct list_hook *entry)
{
	struct list_hook *iter;
	struct pool4_entry *tmp1;
	struct pool4_entry *tmp2;
	tmp1 = malloc(sizeof(struct pool4_entry));
	tmp1->mark = mark;
	tmp1->proto = proto;
	tmp1->addr.s_addr = addr.s_addr;
	tmp1->range.min = range->min;
	tmp1->range.max = range->max;

	list_for_each(iter, entry) {
		tmp2 = list_entry(iter, struct pool4_entry, list);
		if(pool4_entry_equals(*tmp1, *tmp2)) {
			list_del(&tmp2->list);
			free(tmp1);
			free(tmp2);
			return;
		}
	}
}

void pool4_flush(struct list_hook *head)
{
	struct list_hook *iter;
	struct list_hook *tmp;
	struct pool4_entry *ptr;

	list_for_each_safe(iter, tmp, head) {
		ptr = list_entry(iter, struct pool4_entry, list);
		list_del(&ptr->list);
		free(ptr);
	}
}

bool pool4_is_empty(struct pool4_entry entry)
{
	bool empty;
	if(!list_empty(&entry.list)){
		empty = false;
	} else {
		empty = true;
	}
	return empty;
}

void pool4_print_all(struct pool4_entry entry)
{
	struct pool4_entry *iter;

	list_for_each_entry(iter, &entry.list, list) {
		printf("%u, ", iter->mark);
		printf("%u, ", iter->proto);
		printf("%pI4, ", &iter->addr.s_addr);
		printf("%u-%u\n", iter->range.min, iter->range.max);
	}
}
