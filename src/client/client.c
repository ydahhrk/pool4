/*
 ============================================================================
 Name        : clientes2.c
 Author      : Carlos Frutos
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
//<>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "list.h"
#include "prefixes.h"
#include "client.h"
#include "errno.h"


#define client(prefix, list_hook)

struct client{
	struct ipv6_prefix ipx;
	struct list_head list_hook;
}client;

struct client client_list;

void client_init()
{
	INIT_LIST_HEAD(&client_list.list_hook);
}

int client_add(struct ipv6_prefix *prefix)
{
	struct client *tmp;
	int error;
	tmp= (struct client *)malloc(sizeof(struct client));
	tmp = kmalloc(sizeof(*tmp), GFP_KERNEL);
	if (!tmp) {
		printf("Memory allocation error");
		error = -ENOMEM;
		goto end;

	}
	tmp->ipx = *prefix;
	list_add(&(tmp->list_hook), &(client_list.list_hook));

	end:
	return error;

}

void client_delete(struct ipv6_prefix *prefix)
{
	struct list_head *iter;
	struct list_head *tmpdummy;
	struct client *tmp;
	list_for_each_safe(iter, tmpdummy, &client_list.list_hook) {
		tmp = list_entry(iter, struct client, list_hook);
		if (prefix6_equals(prefix, &tmp->ipx)) {
			list_del(&tmp->list_hook);
			free(tmp);
			return;
		}
	}
}

void client_flush()
{
	struct list_head *iter;
	struct list_head *tmpdummy;
	struct client *objPtr;

	list_for_each_safe(iter, tmpdummy, &client_list.list_hook) {
		objPtr = list_entry(iter, struct client , list_hook);
		list_del(&objPtr->list_hook);
		free(objPtr);
	}
}

void client_exist(struct ipv6_prefix *prefix)
{
	struct list_head *iter;
	struct list_head *tmpdummy;
	struct client *tmp;
	list_for_each_safe(iter, tmpdummy, &client_list.list_hook) {
		tmp = list_entry(iter, struct client, list_hook);
		if (prefix6_equals(prefix, &tmp->ipx)) {
			printf("Client exist\n");
			return;
		}

	}
	printf("Client does not exist\n");
}

int client_count()
{
	struct list_head *iter;
	struct list_head *tmpdummy;
	int i = 0;

	list_for_each_safe(iter, tmpdummy, &client_list.list_hook){
		i++;
	}
	return i;
}


void client_print_all()
{
	struct list_head *iter;
	struct list_head *tmpdummy;
	struct client *objPtr;
	list_for_each_safe(iter, tmpdummy, &client_list.list_hook) {
		objPtr = list_entry(iter, struct client , list_hook);
		printf("Address: %x.%x.%x.%x\nLength:%u\n",
				objPtr->ipx.address.s6_addr32[0],
				objPtr->ipx.address.s6_addr32[1],
				objPtr->ipx.address.s6_addr32[2],
				objPtr->ipx.address.s6_addr32[3],
				objPtr->ipx.len);
	}
}

int clinet_for_each(int (*func)(struct ipv6_prefix *, void *),
		void *arg, struct ipv6_prefix *offset)
{
	struct list_head *iter;
	struct list_head *tmpdummy;
	struct client *tmp;
	int error = 0;

	list_for_each_safe(iter, tmpdummy, &client_list.list_hook) {

		tmp = list_entry(iter, struct client, list_hook);

		if (!offset) {
			error = func(&tmp->ipx, arg);
		if (error)
			break;

		} else if (prefix6_equals(offset, &tmp->ipx)) {
		offset = NULL;
		}
	}
	return offset ? -ESRCH : error;
}

/*
int pool6_for_each(int (*func)(struct ipv6_prefix *, void *), void *arg,
		struct ipv6_prefix *offset)
{
	struct list_head *list;
	struct list_head *node;
	struct pool_entry *entry;
	int error = 0;

	rcu_read_lock_bh();
	list = rcu_dereference_bh(pool);

	list_for_each_rcu_bh(node, list) {
		entry = get_entry(node);
		if (!offset) {
			error = func(&entry->prefix, arg);
			if (error)
				break;
		} else if (prefix6_equals(offset, &entry->prefix)) {
			offset = NULL;
		}
	}

	rcu_read_unlock_bh();
	return offset ? -ESRCH : error;
}
*/

