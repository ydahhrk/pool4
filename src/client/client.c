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


#define client(prefix, root)

static unsigned int power;

struct client{
	struct ipv6_prefix ipx;
	struct list_head root;
};

void client_add(struct ipv6_prefix prefix){
	struct client *tmp;
	struct client client_list;
	INIT_LIST_HEAD(&client_list.root);
	tmp= (struct client *)malloc(sizeof(struct client));
	list_add(&(tmp->root), &(client_list.root));
}


void client_delete(struct ipv6_prefix *prefix, struct list_head *entry){
	struct list_head *iter;
	struct client *tmp;
	list_for_each(iter, entry) {
		tmp = list_entry(iter, struct client, root);
		if(prefix6_equals(prefix, &tmp->ipx)) {
			list_del(&tmp->root);
			free(tmp);
			return;
		}
	}
}

void client_delete_all(struct list_head *head)
{
	struct list_head *iter;
	struct client *objPtr;

	list_for_each(iter, head) {
		objPtr = list_entry(iter, struct client , root);
		list_del(&objPtr->root);
		free(objPtr);
	}
}

int client_is_empty(struct list_head *root){
	return list_empty(root);
}

int client_count(struct list_head *root){
	struct list_head *iter;
	int i = 0;

	list_for_each(iter, root) {
	i++;
	}
	return i;
}

void client_print(struct ipv6_prefix *prefix, struct list_head *entry){
	char prefix_str[INET6_ADDRSTRLEN];
	struct list_head *iter;
	struct client *tmp;
	int i;
	list_for_each(iter, entry) {
		tmp = list_entry(iter, struct client, root);
		if(prefix6_equals(prefix, &tmp->ipx)) {
			for (i = 0; i < 16; i += 2) {
				printf("%x:\n", tmp->ipx.address);
			}
			return;
		}
	}
}

void client_print_all(struct list_head *head)
{
	struct list_head *iter;
	struct client *objPtr;

	list_for_each(iter, head) {
		objPtr = list_entry(iter, struct client , root);
		printf("%x:\n", &objPtr->ipx.address);
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
		objPtr = list_entry(iter, struct client, root);
		printf("%ip6", objPtr->ipx);
	}
	printf("\n");
}
*/


//p = ipv6_prefix entonces tiene un address y un len,
//con ello se sacara el prefijo con el lenght

//a::a

//0000:0000:0000:0000:0000:0000
