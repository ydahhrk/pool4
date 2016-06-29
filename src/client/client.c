
#include "../types.h"
#include "../prefixes.h"
#include "../pool4/pool4.h"
#include <linux/list.h>
#include <linux/slab.h>
#include "client.h"

#define MAXipv6		cpu_to_be32(0xffffffff)

struct list_head client_hook;

static unsigned int get_addr6_count(struct ipv6_prefix *prefix)
{
	return 1 << (128-prefix->len);
}

int client_init(struct client *client)
{
	INIT_LIST_HEAD(&client->list_hook);
	return 0;
}

int client_add(struct client *client, struct ipv6_prefix *prefix)
{
	struct ipv6_client *add;

	add = kmalloc(sizeof(struct ipv6_client), GFP_KERNEL);
	if (!add)
		return -ENOMEM;

	add->ipx = *prefix;
	list_add_tail(&(add->list_hook), &(client->list_hook));

	return 0;
}

int client_delete(struct client *client, struct ipv6_prefix *prefix)
{
	struct list_head *iter;
	struct ipv6_client *exist;
	list_for_each(iter, &client->list_hook) {
		exist = list_entry(iter, struct ipv6_client, list_hook);
		if (prefix6_equals(prefix, &exist->ipx)) {
			list_del(&exist->list_hook);
			kfree(exist);
			return 0;
		}
	}

	return 0;
}

int client_flush(struct client *client)
{
	struct list_head *iter;
	struct list_head *client_dummy;
	struct ipv6_client *objPtr;

	list_for_each_safe(iter, client_dummy, &client->list_hook) {
		objPtr = list_entry(iter, struct ipv6_client , list_hook);
		list_del(&objPtr->list_hook);
		kfree(objPtr);
	}

	return 0;
}

int client_exist(struct client *client, struct ipv6_prefix *prefix)
{
	struct list_head *iter;
	struct ipv6_client *exist;
	list_for_each(iter, &client->list_hook){
		exist = list_entry(iter, struct ipv6_client, list_hook);
		if (prefix6_equals(prefix, &exist->ipx)) {
			return 1;
		}

	}
	return 0;
}

unsigned int client_count(struct client *client)
{
        struct list_head *iter;
        struct ipv6_client *exist;
        unsigned int i = 0;

        list_for_each(iter, &client->list_hook) {
                exist = list_entry(iter, struct ipv6_client, list_hook);
                i = i + get_addr6_count(&exist->ipx);
        }

        return i;


}

int client_print_all(struct client *client)
{
	struct list_head *iter;
	struct ipv6_client *obj_ptr;

	list_for_each(iter, &client->list_hook) {
		obj_ptr = list_entry(iter, struct ipv6_client , list_hook);
		pr_info("Address: %x.%x.%x.%x\nLength:%u\n",
				be32_to_cpu(obj_ptr->ipx.address.s6_addr32[0]),
				be32_to_cpu(obj_ptr->ipx.address.s6_addr32[1]),
				be32_to_cpu(obj_ptr->ipx.address.s6_addr32[2]),
				be32_to_cpu(obj_ptr->ipx.address.s6_addr32[3]),
				obj_ptr->ipx.len);
	}

	return 0;
}

int client_for_eachsample(struct client *client, int (*func)(struct ipv6_prefix *,
		void *), void *arg, struct ipv6_prefix *offset)
{
	struct list_head *iter;
	struct ipv6_client *obj_ptr;
	int error = 0;

	list_for_each(iter, &client->list_hook) {
		obj_ptr = list_entry(iter, struct ipv6_client, list_hook);

		if (!offset) {
			error = func(&obj_ptr->ipx, arg);
		if (error)
			break;

		} else if (prefix6_equals(offset, &obj_ptr->ipx)) {
		offset = NULL;
		}
	}
	return offset ? -ESRCH : error;
}

void addr6_iterations(struct in6_addr *client)
{
	if (!(client->s6_addr32[3] == MAXipv6)) {
		client->s6_addr32[3]++;
	}else if (!(client->s6_addr32[2] == MAXipv6)) {
		client->s6_addr32[3] = cpu_to_be32(0x0);
		client->s6_addr32[2]++;
	}else if (!(client->s6_addr32[1] == MAXipv6)) {
		client->s6_addr32[3] = cpu_to_be32(0x0);
		client->s6_addr32[2] = cpu_to_be32(0x0);
		client->s6_addr32[1]++;
	}else if (!(client->s6_addr32[1] == MAXipv6)) {
		client->s6_addr32[3] = cpu_to_be32(0x0);
		client->s6_addr32[2] = cpu_to_be32(0x0);
		client->s6_addr32[1] = cpu_to_be32(0X0);
	}
}

static bool prefix_contains(struct ipv6_prefix *prefix, struct in6_addr *addr)
{
	__u32 mask = ((__u64) MAXipv6) << (128 - prefix->len);

	__u32 prefixaddr0 = prefix->address.s6_addr32[0] & mask;
	__u32 prefixaddr1 = prefix->address.s6_addr32[1] & mask;
	__u32 prefixaddr2 = prefix->address.s6_addr32[2] & mask;
	__u32 prefixaddr3 = prefix->address.s6_addr32[3] & mask;

	__u32 addr0 = addr->s6_addr32[0] & mask;
	__u32 addr1 = addr->s6_addr32[1] & mask;
	__u32 addr2 = addr->s6_addr32[2] & mask;
	__u32 addr3 = addr->s6_addr32[3] & mask;

	return (prefixaddr0 == addr0 && prefixaddr1 == addr1 &&
			prefixaddr2 == addr2 && prefixaddr3 == addr3);
}

bool client_addr_exist(struct client *client, struct in6_addr *addr)
{
	struct list_head *iter;
	struct ipv6_client *obj_ptr;
	struct in6_addr dummy;
	int i = 0;

	list_for_each(iter, &client->list_hook) {
		obj_ptr = list_entry(iter, struct ipv6_client, list_hook);
		dummy = obj_ptr->ipx.address;
		/*Saving the original value of the address */
		for (i = 0; i < get_addr6_count(&obj_ptr->ipx); i++) {
			if(addr->s6_addr32[0] == dummy.s6_addr32[0]
				&& addr->s6_addr32[1] == dummy.s6_addr32[1]
				&& addr->s6_addr32[2] == dummy.s6_addr32[2]
				&& addr->s6_addr32[3] == dummy.s6_addr32[3])
				return true;
			addr6_iterations(&dummy);
		}
	}

	return false;
}

int client_for_each(struct client *client, int (*cb)(struct in6_addr *, void *),
		void *arg, unsigned int offset)
{

	struct list_head *iter;
	struct ipv6_client *obj_ptr;
	struct in6_addr dummy;
	int flag = 0;
	int error = 0;
	int i;
	int total_clients = 0;
	int client_index;
	int client_index_aux = 0;
	list_for_each(iter, &client->list_hook) {
		obj_ptr = list_entry(iter, struct ipv6_client, list_hook);
		total_clients = total_clients + get_addr6_count(&obj_ptr->ipx);
	}

	offset = offset%total_clients;
	client_index = offset;
	list_for_each(iter, &client->list_hook) {
		obj_ptr = list_entry(iter, struct ipv6_client, list_hook);
		dummy = obj_ptr->ipx.address;
		/*Saving the original value of the address */
		for (i = 0; i < get_addr6_count(&obj_ptr->ipx); i++) {
			if (offset == 0) {
				flag = 1;
				error = cb(&dummy, arg);
				if (error){
					return error;
				}
			} else {
				offset--;
			}
			addr6_iterations(&dummy);
		}

	}

	if (flag){
		list_for_each(iter, &client->list_hook){
			obj_ptr = list_entry(iter, struct ipv6_client, list_hook);
			dummy = obj_ptr->ipx.address;
			for (i = 0; i < get_addr6_count(&obj_ptr->ipx); i++) {
				if (client_index_aux < client_index) {
					error = cb(&dummy,arg);
					if (error)
						return error;
					addr6_iterations(&dummy);
				}else{
					return error;
				}
				client_index_aux++;
			}
		}
	}
	return offset ? -ESRCH : error;

}

int client_get_mask_domain(struct client *client, struct pool4 *pool4,
		struct in6_addr *addr,
		struct client_mask_domain *result,
		unsigned int masks_per_client)
{
	struct list_head *iter;
	struct ipv6_client *ipv6_listed;
	int ipv6_pos = 0;
	int error = 0;
	bool addr_exist = false;

	if (client_count(client) > pool4_count(pool4)) {
//		pr_info("More clients than mask entries\n");
		return 1;
	}

	list_for_each(iter, &client->list_hook) {
		ipv6_listed = list_entry(iter, struct ipv6_client, list_hook);
		if (prefix_contains(&ipv6_listed->ipx, addr)) {
			addr_exist = true;
			break;
		}
		ipv6_pos++;
	}

	if (!addr_exist) {
		pr_info("addr does not exist in client database\n");
		return -ESRCH;
	}

	error = pool4_taddr4_find_pos(pool4, ipv6_pos, result, masks_per_client);
	if (error)
		return error;

	return 0;
}
