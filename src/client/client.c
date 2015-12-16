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


#define client(prefix, list_hook)

static unsigned int power;

struct client{
	struct ipv6_prefix ipx;
	struct list_head list_hook;
}client;

struct client client_list;

void client_init()
{
	INIT_LIST_HEAD(&client_list.list_hook);
}

void client_add(struct ipv6_prefix *prefix)
{
	struct client *tmp;
	int error;
	tmp= (struct client *)malloc(sizeof(struct client));
	tmp->ipx = *prefix;
	list_add(&(tmp->list_hook), &(client_list.list_hook));

}
/*
int pool4db_add(const __u32 mark, enum l4_protocol proto,
		struct ipv4_prefix *prefix, struct port_range *ports)
{
	struct hlist_head *database;
	struct pool4_table *table;
	int error;

	mutex_lock(&lock);

	database = rcu_dereference_protected(db, lockdep_is_held(&lock));
	table = find_table(database, mark, proto);
	if (!table) {
		table = pool4table_create(mark, proto);
		if (!table) {
			error = -ENOMEM;
			goto end;
		}

		error = pool4table_add(table, prefix, ports);
		if (error) {
			pool4table_destroy(table);
			goto end;
		}

		tables++;
		hlist_add_head_rcu(&table->hlist_hook,
				&database[hash_32(mark, power)]);
		if (tables > slots()) {
			log_warn_once("You have lots of pool4s, which can lag "
					"Jool. Consider increasing "
					"pool4_size.");
		}

	} else {
		error = pool4table_add(table, prefix, ports);

	}

end:
	mutex_unlock(&lock);
	return error;
}
*/


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
		printf("Address: %x\nLength:%i\n", objPtr->ipx.address, objPtr->ipx.len);
	}
}

/*
int client_foreach_sample(int (*cb)(struct client *, void *), void *arg,
		struct client *offset)
{
	struct hlist_head *database;
	struct pool4_table *table;
	struct hlist_node *node;
	__u32 hash = offset ? hash_32(offset->ipx, power) : 0;
	int error = 0;

	database = rcu_dereference_bh();
	for (; hash < slots(); hash++) {
		hlist_for_each_rcu_bh(node, &database||||[hash]) {
			table = table_entry(node);
			if (offset) {
				if ((prefix6_equals(table->ipx, offset->ipx)) {
					error = pool4table_foreach_sample(table,
							cb, arg, offset);
					if (error)
						goto end;
					offset = NULL;
				}
			} else {
				error = pool4table_foreach_sample(table, cb,
						arg, NULL);
				if (error)
					goto end;
			}
		}
	}

end:
	rcu_read_unlock_bh();
	return error;
}
*/

