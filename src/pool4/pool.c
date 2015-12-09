#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "types.h"
#include "in.h"
#include "port_range.h"
#include "list.h"

#define cpu_to_be32(x) x

typedef enum { false, true } bool;

typedef struct pool4_entry {
	__u32 mark;
	__u8 proto;
	struct in_addr addr;
	struct port_range range;
	struct list_head list;
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
	if(new.mark == old.mark && new.proto == old.proto &&
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
		struct port_range *range, struct list_head *entry)
{
	struct list_head *iter;
	struct pool4_entry *tmp1 = 0;
	struct pool4_entry *tmp2;
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


/*
void pool4_rm(__u32 mark, __u8 proto,  struct in_addr addr,
		struct port_range range)
{
	struct pool4_entry *tmp;
	tmp = malloc(sizeof(struct pool4_entry));
	tmp->mark = mark;
	tmp->proto = proto;
	tmp->addr = addr;
	tmp->range = range;

	if (tmp == list_entry(tmp, struct pool4_entry, list))
		list_del(&(tmp.list));
}

void rem_entry(struct list_head *head)
{
	struct list_head *iter;
	struct pool4_entry *tmp;

	list_for_each(iter, head){
		tmp = list_entry(iter, struct pool4_entry, list);
		list_del(&tmp->list);
		free(tmp);
	}
}
*/

void pool4_rm_all(struct list_head *head)
{
	struct list_head *iter;
	struct pool4_entry *objPtr;

	list_for_each(iter, head) {
		objPtr = list_entry(iter, struct pool4_entry, list);
		list_del(&objPtr->list);
		free(objPtr);
	}
}

/*
void pool4_contains(struct pool4_entry)
{
	struct list_head *head;
	struct pool4_entry *tmp;
	tmp = pool4_one.list;

	list_entry(ptr, type, member);
	container_of(head, struct pool4_entry, list);
}
*/

void pool4_is_empty(struct pool4_entry entry)
{

	if(!list_empty(&entry.list)){
		printf("List is not empty!\n");
	} else {
		printf("List is empty!\n");
	}
}

/*
void pool4_is_empty(struct list_head *head)
{
	if(!list_empty(&pool4_one.list)){
		printf("List is not empty!\n");
	} else {
		printf("List is empty!\n");
	}
}
*/

void pool4_print_all(struct pool4_entry entry)
{
	struct pool4_entry *iter;

	list_for_each_entry(iter, &entry.list, list);
	printf("%u, ", iter->mark);
	printf("%u, ", iter->proto);
	printf("%pI4, ", &iter->addr.s_addr);
	printf("%u-%u\n", iter->range.min, iter->range.max);
}

/* void pool4_print_all(struct list_head *head)
{
	struct list_head *iter;
	struct pool4_entry *objPtr;

	list_for_each(iter, head) {
		objPtr = list_entry(iter, struct pool4_entry, list);
		printf("%u, ", objPtr->mark);
		printf("%u, ", objPtr->proto);
		printf("%pI4, ", &objPtr->addr.s_addr);
		printf("%u-%u\n", objPtr->range.min, objPtr->range.max);
	}
	printf("\n");
} */

int main() {

	struct pool4_entry pool4_list;

	INIT_LIST_HEAD(&pool4_list.list);

	pool4_list.mark = 0;
	pool4_list.proto = 0;
	pool4_list.addr.s_addr = cpu_to_be32(0xc0000201); /* 192.0.2.1 */
	pool4_list.range.min = 1;
	pool4_list.range.max = 10;

	pool4_add(pool4_list.mark, pool4_list.proto, pool4_list.addr,
			&pool4_list.range, &pool4_list);
	return 0;
}
