/*
 * client.h
 *
 *  Created on: Nov 24, 2015
 *      Author: cfrutos
 */


#ifndef CLIENT_H_
#define CLIENT_H_
#include "client.c"



void client_add(struct ipv6_prefix prefix);

void client_delete(struct ipv6_prefix *prefix, struct list_head *entry);

void client_delete_all(struct list_head *head);

int client_is_empty(struct list_head *root);

int client_count(struct list_head *root);


#endif /* CLIENT_H_ */
