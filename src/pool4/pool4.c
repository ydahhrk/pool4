#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "types.h"
#include "list.h"
#include "pool4.h"

struct pool4_entry pool4_list;

void pool4_init()
{
	INIT_LIST_HEAD(&pool4_list.list);
}

int pool4_add(__u32 mark, __u8 proto,  struct in_addr addr,
		struct port_range *range, struct pool4_entry *entry)
{
	int err;
	struct pool4_entry *tmp;
	tmp = malloc(sizeof(struct pool4_entry));
	if (!tmp) {
		err = -ENOMEM;
	} else {
		err = 0;
	}

	tmp->mark = mark;
	tmp->proto = proto;
	tmp->addr.s_addr = addr.s_addr;
	tmp->range.min = range->min;
	tmp->range.max = range->max;
	list_add_tail(&(tmp->list), &(entry->list));
	return err;
}

static bool pool4_entry_equals(struct pool4_entry new, struct pool4_entry old)
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

int pool4_rm(__u32 mark, __u8 proto, struct in_addr addr,
		struct port_range *range, struct pool4_entry *entry)
{
	struct list_hook *iter;
	struct list_hook *tmp;
	struct pool4_entry *tmp1;
	struct pool4_entry *tmp2;
	int err;

	tmp1 = malloc(sizeof(struct pool4_entry));
	if (!tmp1) {
		err = -ENOMEM;
	} else {
		err = 0;
	}

	tmp1->mark = mark;
	tmp1->proto = proto;
	tmp1->addr.s_addr = addr.s_addr;
	tmp1->range.min = range->min;
	tmp1->range.max = range->max;

	list_for_each_safe(iter, tmp, &entry->list) {
		tmp2 = list_entry(iter, struct pool4_entry, list);
		if (pool4_entry_equals(*tmp1, *tmp2)) {
			list_del(&tmp2->list);
			free(tmp1);
			free(tmp2);
		}
	}
	return err;
}

int pool4_flush(struct pool4_entry *entry)
{
	printf("List flushed.\n\n");
	int err;
	struct list_hook *iter;
	struct list_hook *tmp;
	struct pool4_entry *tmpx;

	tmpx = malloc(sizeof(struct pool4_entry));
		if (!tmpx) {
			err = -ENOMEM;
		} else {
			err = 0;
		}

	list_for_each_safe(iter, tmp, &entry->list) {
		tmpx = list_entry(iter, struct pool4_entry, list);
		list_del(iter);
		free(tmpx);
	}
	return err;
}

bool pool4_is_empty(struct pool4_entry *entry)
{
	bool empty;

	if (!list_empty(&entry->list)){
		empty = false;
		printf("It is not empty.\n\n");
	} else {
		empty = true;
		printf("It is empty.\n\n");
	}
	return empty;
}

void pool4_print_all(struct pool4_entry *entry)
{
	printf("Elements in the list:\n\n");
	struct list_hook *iter;
	struct list_hook *tmp;
	struct pool4_entry *tmpx;

	list_for_each_safe(iter, tmp, &entry->list) {
		tmpx = list_entry(iter, struct pool4_entry, list);
		printf("%u, ", tmpx->mark);
		printf("%u, ", tmpx->proto);
		printf("%pI4, ", &tmpx->addr.s_addr);
		printf("%u-%u\n", tmpx->range.min, tmpx->range.max);
	}
}

//In construction...
/*
bool pool4_contains(struct pool4_entry new, struct pool4_entry *entry)
{
	bool found = false;
	int err;
	struct list_hook *iter;
	struct list_hook *tmp;
	struct pool4_entry *tmp1;
	struct pool4_entry *tmp2;

	tmp1 = malloc(sizeof(struct pool4_entry));
	if (!tmp1){
		err = -ENOMEM;
	} else {
		err = 0;
	}

	tmp1->mark = new.mark;
	tmp1->proto = new.proto;
	tmp1->addr.s_addr = new.addr.s_addr;
	tmp1->range.min = new.range.min;
	tmp1->range.max = new.range.max;

	list_for_each_safe(iter, tmp, &entry->list){
		tmp2 = list_entry(iter, struct pool4_entry, list);
		if (pool4_entry_equals(*tmp1, *tmp2)){
			found = true;
		}
	}
	return found;
}
*/
