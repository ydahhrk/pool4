#include "client/client.h"
#include "types.h"
#include "prefixes.h"
#include "pool4/pool4.h"
#include <linux/types.h>
#include <linux/in6.h>
#include <linux/in.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>

#define MAXipv6		cpu_to_be32(0xffffffff)

struct ipv6_client{
	struct ipv6_prefix ipx;
	struct list_head list_hook;
};

struct list_head client_hook;

static unsigned int get_addr6_count(struct ipv6_prefix *prefix)
{
	return 1 << (128-prefix->len);
}

void client_init()
{
	INIT_LIST_HEAD(&client_hook);
}

int client_add(struct ipv6_prefix *prefix)
{
	struct ipv6_client *client;
	int error;
	client = kmalloc(sizeof(*client), GFP_KERNEL);
	if (!client) {
		printk("Memory allocation error");
		error = -ENOMEM;
		return error;

	}
	client->ipx = *prefix;
	list_add_tail(&(client->list_hook), &(client_hook));
	return 0;
}

void client_delete(struct ipv6_prefix *prefix)
{
	struct list_head *iter;

	struct ipv6_client *client;
	list_for_each(iter, &client_hook) {
		client = list_entry(iter, struct ipv6_client, list_hook);
		if (prefix6_equals(prefix, &client->ipx)) {
			list_del(&client->list_hook);
			kfree(client);
			return;
		}
	}
}

void client_flush()
{
	struct list_head *iter;
	struct list_head *client_dummy;
	struct ipv6_client *objPtr;

	list_for_each_safe(iter, client_dummy, &client_hook) {
		objPtr = list_entry(iter, struct ipv6_client , list_hook);
		list_del(&objPtr->list_hook);
		kfree(objPtr);
	}
}

bool client_exist(struct ipv6_prefix *prefix)
{
	struct list_head *iter;
	struct ipv6_client *client;
	list_for_each(iter, &client_hook) {
		client = list_entry(iter, struct ipv6_client, list_hook);
		if (prefix6_equals(prefix, &client->ipx)) {
			return 1;
		}

	}
	return 0;
}



unsigned int client_count()
{
        struct list_head *iter;
        struct ipv6_client *client;
        unsigned int i = 0;

        list_for_each(iter, &client_hook) {
                client = list_entry(iter, struct ipv6_client, list_hook);
                i = i + get_addr6_count(&client->ipx);
        }

        return i;


}


void client_print_all()
{
	struct list_head *iter;

	struct ipv6_client *obj_ptr;
	list_for_each(iter, &client_hook) {
		obj_ptr = list_entry(iter, struct ipv6_client , list_hook);
		pr_debug("Address: %x.%x.%x.%x\nLength:%u\n",
				obj_ptr->ipx.address.s6_addr32[0],
				obj_ptr->ipx.address.s6_addr32[1],
				obj_ptr->ipx.address.s6_addr32[2],
				obj_ptr->ipx.address.s6_addr32[3],
				obj_ptr->ipx.len);
	}
}

int client_for_eachsample(int (*func)(struct ipv6_prefix *, void *),
		void *arg, struct ipv6_prefix *offset)
{
	struct list_head *iter;
	struct ipv6_client *client;
	int error = 0;

	list_for_each(iter, &client_hook) {
		client = list_entry(iter, struct ipv6_client, list_hook);

		if (!offset) {
			error = func(&client->ipx, arg);
		if (error)
			break;

		} else if (prefix6_equals(offset, &client->ipx)) {
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

int client_addr_exist(struct in6_addr *addr)
{
	struct list_head *iter;
	struct ipv6_client *client;
	struct in6_addr dummy;
	int i = 0;

	list_for_each(iter, &client_hook) {
		client = list_entry(iter, struct ipv6_client, list_hook);
		dummy = client->ipx.address;
		/*Saving the original value of the address */
		for (i = 0; i < get_addr6_count(&client->ipx); i++) {
			if(addr->s6_addr32[0] == dummy.s6_addr32[0]
				&& addr->s6_addr32[1] == dummy.s6_addr32[1]
				&& addr->s6_addr32[2] == dummy.s6_addr32[2]
				&& addr->s6_addr32[3] == dummy.s6_addr32[3])
				return 0;
			addr6_iterations(&dummy);
		}
	}
	return -1;
}

int client_for_each(int (*cb)(struct in6_addr *, void *),
		void *arg, unsigned int offset)
{

	struct list_head *iter;
	struct ipv6_client *client;
	struct in6_addr dummy;
	int flag = 0;
	int error = 0;
	int i;
	int total_clients = 0;
	int client_index;
	int client_index_aux = 0;
	list_for_each(iter, &client_hook) {
		client = list_entry(iter, struct ipv6_client, list_hook);
		total_clients = total_clients + get_addr6_count(&client->ipx);

	}

	offset = offset%total_clients;
	client_index = offset;
	list_for_each(iter, &client_hook) {
		client = list_entry(iter, struct ipv6_client, list_hook);
		dummy = client->ipx.address;
		/*Saving the original value of the address */
		for (i = 0; i < get_addr6_count(&client->ipx); i++) {
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
		list_for_each(iter, &client_hook){
			client = list_entry(iter, struct ipv6_client, list_hook);
			dummy = client->ipx.address;
			for (i = 0; i < get_addr6_count(&client->ipx); i++) {
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
//
//int client_get_mask_domain(struct in6_addr *client,
//		struct client_mask_domain *result,
//		unsigned int masks_per_client)
//{
//	struct list_head *iter;
//	struct ipv6_client *ipv6_listed;
//	int ipv6_pos = 0;
//	int error = 0;
//
//	if (client_count() > pool4_count(pool4)) {
//		printf("There are more clients than mask entries\n");
//		return 0;
//	}
//
//	list_for_each(iter, &client_hook) {
//		ipv6_listed = list_entry(iter, struct ipv6_client, list_hook);
//		if (ipv6_addr_equal(client, &ipv6_listed->ipx.address)) {
//			break;
//		}
//		ipv6_pos++;
//	}
//
//	error = pool4_taddr4_find_pos(pool4, ipv6_pos, result, masks_per_client);
//	if (error)
//		return error;
//
//	return 0;
//}
