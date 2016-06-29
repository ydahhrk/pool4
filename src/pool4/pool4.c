#include "pool4/pool4.h"
#include "types.h"
#include "client/client.h"
#include <linux/types.h>
#include <linux/list.h>
#include <linux/in6.h>
#include <linux/ipv6.h>
#include <linux/in.h>
#include <linux/random.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/slab.h>

struct client;
static int dynamic_assignment = 1;

int pool4_init(struct pool4 *pool4)
{
	INIT_LIST_HEAD(&pool4->list);
	return 0;
}

int pool4_add(struct pool4 *pool4, __u32 mark, __u8 proto,
		struct in_addr *addr, struct port_range *range)
{
	struct pool4_entry *add = kmalloc(sizeof(struct pool4_entry),
			GFP_KERNEL);
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
			one->range.min == two->range.min)
		return 1;
	else
		return 0;
}

static int pool4_compare(struct pool4_entry *one, struct pool4_entry *two)
{
	if (one->proto == two->proto && one->addr.s_addr == two->addr.s_addr)
		return 1;
	else
		return 0;
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
		list_del(&entry->list_hook);
		kfree(entry);
	}
	return 0;
}

int pool4_is_empty(struct pool4 *pool4)
{
	if (!list_empty(&pool4->list))
//		pr_info("It is not empty.\n\n");
		return 0;

//	pr_info("It is empty.\n\n");
	return 1;

}

int pool4_print_all(struct pool4 *pool4)
{
	struct list_head *iter;
	struct list_head *tmp;
	struct pool4_entry *entry;
	pr_info("Elements in pool4:\n");

	list_for_each_safe(iter, tmp, &pool4->list) {
		entry = list_entry(iter, struct pool4_entry, list_hook);
		pr_info("%u %u %pI4 %u-%u\n", entry->mark, entry->proto,
				&entry->addr, entry->range.min,
				entry->range.max);
	}

	return 0;
}

bool pool4_contains(struct pool4 *pool4, __u32 mark, __u8 proto,
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
		if (pool4_compare(&requested, listed))
//			pr_info("It is in the list.\n\n");
			return true;
	}

//	pr_info("It is not in the list.\n\n");
	return false;
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
		} else if (pool4_equals(offset, tmp))
			offset = NULL;
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

	if(masks_per_client == 0) {
		pr_info("Number of masks per client is invalid.\n");
		return 1;
	}

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
				if (error)
					return error;
			}
			indx++;
		}
	}

	if (offset == 0)
		return 0;

	//Iteration to apply cback to elements from first to offset - 1
	indx = 0;
	list_for_each(iter, &pool4->list) {
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
				(entry->range.min <= domain->first.l4) &&
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
			else {
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

int bibdb_contains4(const struct ipv4_transport_addr *addr){
	return addr->l4 & 1;
}

static int get_spool_mask(struct packet *packet, struct pool4 *spool,
		struct ipv4_transport_addr *result)
{
//	if (pool4_is_empty(spool))
//		return 1;

	result->l3.s_addr = cpu_to_be32(0x00000000);
	result->l4 = 0;

	return 0;
}

int get_mask(struct packet *packet, struct pool4 *cpool, struct pool4 *spool,
		struct client *client, struct ipv4_transport_addr *result,
		unsigned int masks_per_client)
{
	int i;
	int n;
	int error = 0;
	struct list_head *iter;
	struct pool4_entry *entry;
	struct client_mask_domain mask_domain;
	struct ipv6_prefix prefix;

	if (pool4_is_empty(cpool)){
		// cpool4 is not available; use spool4
		error = get_spool_mask(packet, spool, result);
			return error;
	}

	if (!client_addr_exist(client, &packet->hdr->saddr)) {
		if (dynamic_assignment) {
			prefix.address = packet->hdr->saddr;
			prefix.len = 128;
			client_add(client, &prefix);
		}
		else {
			// cpool4 cannot be used; use spool4
			error = get_spool_mask(packet, spool, result);
				return error;
		}
	}

	error = client_get_mask_domain(client, cpool, &packet->hdr->saddr,
			&mask_domain, masks_per_client);
	if (error)
		return error;

	n = mask_domain.count;

	list_for_each(iter, &cpool->list) {
		entry = list_entry(iter, struct pool4_entry, list_hook);
		for (i = entry->range.min; i <= entry->range.max; i++) {
			if (entry->addr.s_addr == mask_domain.first.l3.s_addr &&
					i == mask_domain.first.l4) {
				result->l3.s_addr = entry->addr.s_addr;
				result->l4 = i;
				if (!bibdb_contains4(result))
					return error;
				n--;
				if (n == 0)
					break;
			}
		}
	}

	pr_info("Mask domain not available, get spool");
	error = get_spool_mask(packet, spool, result);

	return error;
}

