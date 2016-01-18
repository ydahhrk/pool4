#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "list.h"
#include "pool4.h"
#include "errno.h"


struct list_hook pool4_list;

void pool4_init()
{
	INIT_LIST_HEAD(&pool4_list);
}

int pool4_add(__u32 mark, __u8 proto,  struct in_addr addr,
		struct port_range *range)
{
	struct pool4_entry *tmp;
	tmp = malloc(sizeof(struct pool4_entry));
	if (!tmp) {
		return -ENOMEM;
	}

	tmp->mark = mark;
	tmp->proto = proto;
	tmp->addr.s_addr = addr.s_addr;
	tmp->range.min = range->min;
	tmp->range.max = range->max;
	list_add_tail(&(tmp->list), &(pool4_list));

	return 0;
}

static bool pool4_equals(struct pool4_entry *new, struct pool4_entry *old)
{
	if (new->mark == old->mark && new->proto == old->proto &&
			new->addr.s_addr == old->addr.s_addr &&
			new->range.max == old->range.max &&
			new->range.min == old->range.min) {
		return true;
	} else {
		return false;
	}
}

static bool pool4_compare(struct pool4_entry *new, struct pool4_entry *old)
{
	if (new->proto == old->proto &&	new->addr.s_addr == old->addr.s_addr) {
			return true;
		} else {
			return false;
		}
}

int pool4_rm(__u32 mark, __u8 proto, struct in_addr addr,
		struct port_range *range)
{
	struct list_hook *iter;
	struct list_hook *tmp;
	struct pool4_entry *tmp1;
	struct pool4_entry *tmp2;

	tmp1 = malloc(sizeof(struct pool4_entry));
	if (!tmp1) {
		return -ENOMEM;
	}

	tmp1->mark = mark;
	tmp1->proto = proto;
	tmp1->addr.s_addr = addr.s_addr;
	tmp1->range.min = range->min;
	tmp1->range.max = range->max;

	list_for_each_safe(iter, tmp, &pool4_list) {
		tmp2 = list_entry(iter, struct pool4_entry, list);
		if (pool4_equals(tmp1, tmp2)) {
			list_del(&tmp2->list);
			free(tmp1);
			free(tmp2);
		}
	}

	return 0;
}

int pool4_flush(void)
{
	printf("List flushed.\n\n");
	struct list_hook *iter;
	struct list_hook *tmp;
	struct pool4_entry *tmpx;

	tmpx = malloc(sizeof(struct pool4_entry));
		if (!tmpx) {
			return -ENOMEM;
		}

	list_for_each_safe(iter, tmp, &pool4_list) {
		tmpx = list_entry(iter, struct pool4_entry, list);
		list_del(iter);
		free(tmpx);
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
	struct pool4_entry *tmpx;

	list_for_each_safe(iter, tmp, &pool4_list) {
		tmpx = list_entry(iter, struct pool4_entry, list);
		printf("%u, ", tmpx->mark);
		printf("%u, ", tmpx->proto);
		printf("%pI4, ", &tmpx->addr.s_addr);
		printf("%u-%u\n", tmpx->range.min, tmpx->range.max);
	}
}

bool pool4_contains(__u32 mark, __u8 proto, struct in_addr addr,
		struct port_range *range)
{
	struct list_hook *iter;
	struct list_hook *tmp;
	struct pool4_entry *tmp1;
	struct pool4_entry *tmp2;

	tmp1 = malloc(sizeof(struct pool4_entry));
	if (!tmp1) {
		return -ENOMEM;
	}

	tmp1->mark = mark;
	tmp1->proto = proto;
	tmp1->addr.s_addr = addr.s_addr;
	tmp1->range.min = range->min;
	tmp1->range.max = range->max;

	list_for_each_safe(iter, tmp, &pool4_list) {
		tmp2 = list_entry(iter, struct pool4_entry, list);
		if (pool4_compare(tmp1, tmp2)) {
			printf("It is in the list.\n\n");
			return true;
			free(tmp1);
			free(tmp2);
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

int pool4_get_nth_taddr(struct client_mask_domain *domain,
			unsigned int n,
			struct ipv4_transport_addr *result)
{

	struct list_hook *iter;
	struct pool4_entry *pool4;
	bool flag = false;
	int i;
	n = n % domain->count;
	list_for_each(iter, &pool4_list) {
		pool4 = list_entry(iter, struct pool4_entry, list);
		if (((domain->first->l3.s_addr == pool4->addr.s_addr) &&
				(pool4->range.min < domain->first->l4) &&
				(pool4->range.max > domain->first->l4))
				|| flag) {
			if (!flag) {
				for (i = domain->first->l4;
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
