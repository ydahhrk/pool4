#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "list.h"
#include "pool4.h"

<<<<<<< HEAD
struct pool4_entry pool4_list;
=======

struct list_hook pool4_list;
>>>>>>> refs/remotes/origin/A.-Avalos

void pool4_init()
{
	INIT_LIST_HEAD(&pool4_list);
}

<<<<<<< HEAD
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
=======
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
	list_add_tail(&(add->list), &(pool4_list));

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
>>>>>>> refs/remotes/origin/A.-Avalos
	} else {
		return false;
	}
}

<<<<<<< HEAD
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
=======
int pool4_rm(__u32 mark, __u8 proto, struct in_addr *addr,
		struct port_range *range)
{
	struct list_hook *iter;
	struct list_hook *tmp;
	struct pool4_entry rm;
	struct pool4_entry *exist;

	rm.mark = mark;
	rm.proto = proto;
	rm.addr.s_addr = addr->s_addr;
	rm.range.min = range->min;
	rm.range.max = range->max;

	list_for_each_safe(iter, tmp, &pool4_list) {
		exist = list_entry(iter, struct pool4_entry, list);
		if (pool4_equals(&rm, exist)) {
			list_del(&exist->list);
			free(exist);
		}
	}

	return 0;
}

int pool4_flush(void)
>>>>>>> refs/remotes/origin/A.-Avalos
{
	printf("List flushed.\n\n");
	int err;
	struct list_hook *iter;
	struct list_hook *tmp;
<<<<<<< HEAD
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
=======
	struct pool4_entry *entry;

	list_for_each_safe(iter, tmp, &pool4_list) {
		entry = list_entry(iter, struct pool4_entry, list);
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
	struct list_hook *iter;
	struct list_hook *tmp;
	struct pool4_entry *entry;
	char addr[16];

	list_for_each_safe(iter, tmp, &pool4_list) {
		entry = list_entry(iter, struct pool4_entry, list);
		printf("%u, ", entry->mark);
		printf("%u, ", entry->proto);
		printf("%s, ", ip_to_str(entry->addr.s_addr, addr));
		printf("%u-%u\n", entry->range.min, entry->range.max);
>>>>>>> refs/remotes/origin/A.-Avalos
	}
}

<<<<<<< HEAD
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
=======
bool pool4_contains(__u32 mark, __u8 proto, struct in_addr *addr,
		struct port_range *range)
{
	struct list_hook *iter;
	struct list_hook *tmp;
	struct pool4_entry requested;
	struct pool4_entry *listed;

	requested.mark = mark;
	requested.proto = proto;
	requested.addr.s_addr = addr->s_addr;
	requested.range.min = range->min;
	requested.range.max = range->max;

	list_for_each_safe(iter, tmp, &pool4_list) {
		listed = list_entry(iter, struct pool4_entry, list);
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

	struct list_hook *iter;
	int counter = 0;

	list_for_each(iter, &pool4_list) {
		counter++;
	}

	return counter;
}

int pool4_foreach_sample(int (*cb)(struct pool4_entry *, void *), void *arg,
		struct pool4_entry *offset)
{
	struct list_hook *iter;
	struct pool4_entry *tmp;
	int error = 0;

	list_for_each(iter, &pool4_list) {
		tmp = list_entry(iter, struct pool4_entry, list);

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

int pool4_foreach_taddr4(int (*cback)(struct pool4_mask *, void *), void *arg,
		unsigned int offset)
{
	struct list_hook *iter;
	struct pool4_mask mask;
	struct pool4_entry *entry;
	int error = 0;
	int entries = 0;
	int indx = 0;
	unsigned int i;

	list_for_each(iter, &pool4_list) {
		entry = list_entry(iter, struct pool4_entry, list);
		for (i = entry->range.min; i <= entry->range.max; i++) {
			mask.mark = entry->mark;
			mask.proto = entry->proto;
			mask.addr.s_addr = entry->addr.s_addr;
			mask.port = i;
			entries++;
		}
	}

	printf("%d entries", entries);
	printf("\n\n");

	if (offset > entries) {
		offset = offset % entries;
	}

	list_for_each(iter, &pool4_list) {
		entry = list_entry(iter, struct pool4_entry, list);
		for (i = entry->range.min; i <= entry->range.max; i++) {
			mask.mark = entry->mark;
			mask.proto = entry->proto;
			mask.addr.s_addr = entry->addr.s_addr;
			mask.port = i;
			if (indx >= offset){
				error = cback(&mask, arg);
				if (error) {
					break;
				}
			}
			indx++;
		}
	}

	indx = 0;
	if (error)
		return error;

	list_for_each(iter, &pool4_list) {
		if (offset == 0)
			break;
		entry = list_entry(iter, struct pool4_entry, list);
		for (i = entry->range.min; i <= entry->range.max; i++) {
			mask.mark = entry->mark;
			mask.proto = entry->proto;
			mask.addr.s_addr = entry->addr.s_addr;
			mask.port = i;
			if (indx < offset) {
				error = cback(&mask, arg);
				if (error) {
					break;
				}
			}
			indx++;
		}
	}

	return 0;
}












>>>>>>> refs/remotes/origin/A.-Avalos
