#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "list.h"
#include "prefixes.h"
#include "client.h"
#include "errno.h"
#include <math.h>

#define MAXipv6		cpu_to_be32(0xffffffff)

struct ipv6_client{
	struct ipv6_prefix ipx;
	struct list_head list_hook;
};

struct list_head client_hook;

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
		printf("Memory allocation error");
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
			return true;
		}

	}
	return false;
}

unsigned int client_count()
{
	struct list_head *iter;
	unsigned int i = 0;

	list_for_each(iter, &client_hook){
		i++;
	}
	return i;
}


void client_print_all()
{
	struct list_head *iter;

	struct ipv6_client *obj_ptr;
	list_for_each(iter, &client_hook) {
		obj_ptr = list_entry(iter, struct ipv6_client , list_hook);
		printf("Address: %x.%x.%x.%x\nLength:%u\n",
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

void addr6_iterations(struct ipv6_client *client){
	if (!(client->ipx.address.s6_addr32[3] == MAXipv6)) {
		client->ipx.address.s6_addr32[3]++;
	}else if (!(client->ipx.address.s6_addr32[2] == MAXipv6)) {
		client->ipx.address.s6_addr32[3] = cpu_to_be32(0x0);
		client->ipx.address.s6_addr32[2]++;
	}else if (!(client->ipx.address.s6_addr32[1] == MAXipv6)) {
		client->ipx.address.s6_addr32[3] = cpu_to_be32(0x0);
		client->ipx.address.s6_addr32[2] = cpu_to_be32(0x0);
		client->ipx.address.s6_addr32[1]++;
	}else if (!(client->ipx.address.s6_addr32[1] == MAXipv6)) {
		client->ipx.address.s6_addr32[3] = cpu_to_be32(0x0);
		client->ipx.address.s6_addr32[2] = cpu_to_be32(0x0);
		client->ipx.address.s6_addr32[1] = cpu_to_be32(0X0);
	}
}

int client_for_each(int (*cb)(struct in6_addr *, void *),
		void *arg, unsigned int offset)
{

	struct list_head *iter;
	struct ipv6_client *client;
	struct ipv6_client dummy;
	bool flag = false;
	int error = 0;
	int i;
	int total_clients = 0;
	int client_index = 0;
	int client_index_aux = 0;

	list_for_each(iter, &client_hook) {
		client = list_entry(iter, struct ipv6_client, list_hook);
		total_clients = total_clients + pow(2, 128- client->ipx.len);

	}

	list_for_each(iter, &client_hook) {
		client = list_entry(iter, struct ipv6_client, list_hook);
		dummy.ipx.address = client->ipx.address; //Saving the original value of the address
		for (i = 0; i < pow(2, 128-client->ipx.len); i++){
			if (offset)
				client_index++;
			if (!offset) {
				error = cb(&client->ipx.address,arg);
				addr6_iterations(client);
			} else if (client_index == offset){
				flag = true;
				offset = 0;
			}

		}
		client->ipx.address = dummy.ipx.address;
	}
	if (flag){
		list_for_each(iter, &client_hook){
			client = list_entry(iter, struct ipv6_client, list_hook);
			dummy.ipx.address = client->ipx.address;
			for (i = 0; i < pow(2, 128-client->ipx.len); i++){
				client_index_aux++;
				if (client_index_aux <= client_index){
					error = cb(&client->ipx.address,arg);
					addr6_iterations(client);
				}
			}
			client->ipx.address = dummy.ipx.address;
		}
	}


	return offset ? -ESRCH : error;

}

