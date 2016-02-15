#include "pool4/pool4.h"
#include "types.h"
#include "client/client.h"
#include <linux/types.h>
#include <linux/list.h>
#include <linux/in6.h>
#include <linux/ipv6.h>
#include <linux/in.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/slab.h>

void pool4_init(struct pool4 *pool4)
{
	INIT_LIST_HEAD(&pool4->list);
}

static int dynamic_assigment = 1;

int pool4_add(struct pool4 *pool4, __u32 mark, __u8 proto,
		struct in_addr *addr, struct port_range *range)
	struct pool4_entry *add;
	add = kmalloc(sizeof(struct pool4_entry), GFP_KERNEL);
	if (!add) {
		return -ENOMEM;
	}

	add->mark = mark;
	add->proto = proto;
	add->addr.s_addr = addr->s_addr;
	add->range.min = range->min;
	add->range.max = range->max;
	list_add_tail(&(add->list_hook), &(pool4->list));

	return 0;
}

static int pool4_equals(struct pool4_entry *one,
		struct pool4_entry *two)
{
	if (one->mark == two->mark && one->proto == two->proto &&
			one->addr.s_addr == two->addr.s_addr &&
			one->range.max == two->range.max &&
			one->range.min == two->range.min) {
		return 1;
	} else {
		return 0;
	}
}

static int pool4_compare(struct pool4_entry *one, struct pool4_entry *two)
{
	if (one->proto == two->proto && one->addr.s_addr == two->addr.s_addr) {
		return 1;
	} else {
		return 0;
	}
}

int pool4_rm(struct pool4 *pool4, __u32 mark, __u8 proto, struct in_addr *addr,
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

	list_for_each_safe(iter, tmp, &pool4->list) {
		exist = list_entry(iter, struct pool4_entry, list_hook);
		if (pool4_equals(&rm, exist)) {
			list_del(&exist->list_hook);
			kfree(exist);
		}
	}

	return 0;
}

int pool4_flush(struct pool4 *pool4)
{
	struct list_head *iter;
	struct list_head *tmp;
	struct pool4_entry *entry;

	list_for_each_safe(iter, tmp, &pool4->list) {
		entry = list_entry(iter, struct pool4_entry, list_hook);
		list_del(iter);
		kfree(entry);
	}
	return 0;
}

int pool4_is_empty(struct pool4 *pool4)
{
	if (!list_empty(&pool4->list)){
		printk("It is not empty.\n\n");
		return 0;
	}
	printk("It is empty.\n\n");
	return 1;

}

void pool4_print_all(struct pool4 *pool4)
{
	printk("Elements in the list:\n\n");
	struct list_head *iter;
	struct list_head *tmp;
	struct pool4_entry *entry;

	list_for_each_safe(iter, tmp, &pool4->list) {
		entry = list_entry(iter, struct pool4_entry, list_hook);
		pr_debug("%u, ", entry->mark);
		pr_debug("%u, ", entry->proto);
		pr_debug("%pI4, ", &entry->addr);
		pr_debug("%u-%u\n", entry->range.min, entry->range.max);
	}
}

int pool4_contains(struct pool4 *pool4, __u32 mark, __u8 proto,
		struct in_addr *addr, struct port_range *range)
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

	list_for_each_safe(iter, tmp, &pool4->list) {
		listed = list_entry(iter, struct pool4_entry, list_hook);
		if (pool4_compare(&requested, listed)) {
			pr_debug("It is in the list.\n\n");
			return 1;
		}
	}

	pr_debug("It is not in the list.\n\n");
	return 0;
}

int pool4_count(struct pool4 *pool4)
{
	struct list_head *iter;
	struct pool4_entry *entry;
	unsigned int entries = 0;
	unsigned int ports = 0;

	list_for_each(iter, &pool4->list) {
		entry = list_entry(iter, struct pool4_entry, list_hook);
		ports = entry->range.max - entry->range.min + 1;
			entries = entries + ports;
	}

	return entries;
}

int pool4_foreach_sample(struct pool4 *pool4,
		int (*cb)(struct pool4_entry *, void *), void *arg,
		struct pool4_entry *offset)
{
	struct list_head *iter;
	struct pool4_entry *tmp;
	int error = 0;

	list_for_each(iter, &pool4->list) {
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

int pool4_taddr4_find_pos(struct pool4 *pool4, int ipv6_pos,
		struct client_mask_domain *result,
		unsigned int masks_per_client)
{
	struct list_head *iter;
	struct pool4_entry *entry;
	int ports_pos = 0;
	int counter = 0;
	int i;

	list_for_each(iter, &pool4->list) {
		entry = list_entry(iter, struct pool4_entry, list_hook);
		for(i = entry->range.min; i <= entry->range.max; i++) {
			if (counter % masks_per_client == 0) {
				if (ipv6_pos == ports_pos) {
					result->first.l3 = entry->addr;
					result->first.l4 = i;
					result->step = 1;
					result->count = masks_per_client;
				}
				ports_pos++;
			}
			counter++;
		}
	}

	return 0;
}

int pool4_foreach_taddr4(struct pool4 *pool4,
		int (*cback)(struct pool4_mask *, void *), void *arg,
		unsigned int offset)
{
	struct list_head *iter;
	struct pool4_mask mask;
	struct pool4_entry *entry;
	int error = 0;
	int entries = 0;
	int indx = 0;
	unsigned int i;

	entries = pool4_count(pool4);
//	printf("%d entries\n\n", entries);
	if (offset > entries) {
		offset = offset % entries;
	}

	//Iteration to apply cback to elements from offset to end
	list_for_each(iter, &pool4->list) {
		entry = list_entry(iter, struct pool4_entry, list_hook);
		for (i = entry->range.min; i <= entry->range.max; i++) {
			if (indx >= offset){
				mask.mark = entry->mark;
				mask.proto = entry->proto;
				mask.addr.s_addr = entry->addr.s_addr;
				mask.port = i;
				error = cback(&mask, arg);
				if (error) {
					return error;
				}
			}
			indx++;
		}
	}
	indx = 0;

	//Iteration to apply cback to elements from first to offset - 1
	list_for_each(iter, &pool4->list) {
		if (offset == 0)
			break;
		entry = list_entry(iter, struct pool4_entry, list_hook);
		for (i = entry->range.min; i <= entry->range.max; i++) {
			if (indx < offset) {
				mask.mark = entry->mark;
				mask.proto = entry->proto;
				mask.addr.s_addr = entry->addr.s_addr;
				mask.port = i;
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


int pool4_get_nth_taddr(struct pool4 *pool4, struct client_mask_domain *domain,
			unsigned int n, struct ipv4_transport_addr *result)
{

	struct list_head *iter;
	struct pool4_entry *entry;
	int flag = 0;
	int i;
	n = n % domain->count;
	list_for_each(iter, &pool4->list) {
		entry = list_entry(iter, struct pool4_entry, list_hook);
		if (((domain->first.l3.s_addr == entry->addr.s_addr) &&
				(entry->range.min < domain->first.l4) &&
				(entry->range.max > domain->first.l4))
				|| flag) {
			if (!flag) {
				for (i = domain->first.l4;
					i <= entry->range.max;
					i += domain->step) {
					if (!n) {
						result->l3 = entry->addr;
						result->l4 = i;
						return 0;
					}
					n--;
				}
				flag = 1;
			}
			else{
				for (i = entry->range.min;
						i <= entry->range.max;
						i += domain->step) {
					if (!n) {
						result->l3 = entry->addr;
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

/*This doesn't work! Still checking what to do with it.
 * How to write the correct things.
 */

struct ipv4_transport_addr get_mask(struct packet *packet, struct pool4 *cpool,
	struct pool4 *spool, struct client *client)
{
	struct ipv4_transport_addr dummy;
	if (pool4_is_empty(cpool)) {
		if(spool == NULL)
			return 0; // the original spool is also empty and it just ends
		return get_mask(packet, spool, NULL);
//		//call again to use spool
	}
//	/*check if pack addr does not exists in client db */
	if (!(client_addr_exist(client, packet->hdr->saddr))) {
		if (dynamic_assigment)
			client_add(*cpool); /*add client to db*/
		else{
			if(pool4_is_empty(*spool)
					return NULL;

			//use spool

		}
		return NULL;
	}
//
//	if (/*check if all cpool4 masks are used*/) {
//		/*use spool4, but what if from the beggining it's using spool...*/
//	}

	/*get_mask_domain() with the client that was created 	*/
	/* get_nth_addr() send the client created and use it to search nth
	 * but where to get nth
	 * */
	return dummy;
}
