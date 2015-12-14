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
	int error;

	error= -ENOMEM;
	struct client *tmp;
	tmp= (struct client *)malloc(sizeof(struct client));
	list_add(&(tmp->list_hook), &(client_list.list_hook));
}


void client_delete(struct ipv6_prefix *prefix)
{
	struct list_head *iter;
	struct list_head *tmpdummy;
	struct client *tmp;
	list_for_each_safe(iter, tmpdummy, client_list.list_hook) {
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

	list_for_each_safe(iter, tmpdummy, client_list.list_hook) {
		objPtr = list_entry(iter, struct client , list_hook);
		list_del(&objPtr->list_hook);
		free(objPtr);
	}
}

bool client_is_empty(struct ipv6_prefix *prefix)
{
	struct list_head *iter;
	struct list_head *tmpdummy;
	struct client *tmp;
	list_for_each_safe(iter, tmpdummy, client_list.list_hook) {
		tmp = list_entry(iter, struct client, list_hook);
		if (prefix6_equals(prefix, &tmp->ipx)) {
			return true;
		}
		else
			return false;
	}
}

int client_count()
{
	struct list_head *iter;
	struct list_head *tmpdummy;
	int i = 0;

	list_for_each_safe(iter, tmpdummy, client_list.list_hook){
		i++;
	}
	return i;
}


void client_print_all()
{
	struct list_head *iter;
	struct list_head *tmpdummy;
	struct client *objPtr;

	list_for_each_safe(iter, tmpdummy, client_list.list_hook) {
		objPtr = list_entry(iter, struct client , list_hook);
		printf("%x:\n %i:\n", &objPtr->ipx.address, &objPtr->ipx.len);
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
/*
void client_print(struct list_head *head){
	struct list_head *iter;
	struct pool4_entry *objPtr;
	list_for_each(iter, head) {
		objPtr = list_entry(iter, struct client, list_hook);
		printf("%ip6", objPtr->ipx);
	}
	printf("\n");
}
*/


//p = ipv6_prefix entonces tiene un address y un len,
//con ello se sacara el prefijo con el lenght

//a::a

//0000:0000:0000:0000:0000:0000
