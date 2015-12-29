/*
 * client.h
 *
 *  Created on: Nov 24, 2015
 *      Author: cfrutos
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

#ifndef CLIENT_H_
#define CLIENT_H_

void client_init();

int client_add(struct ipv6_prefix *prefix);

void client_delete(struct ipv6_prefix *prefix);

void client_flush();

bool client_exist(struct ipv6_prefix *prefix);

unsigned int client_count();

void client_print_all();

int client_for_eachsample(int (*func)(struct ipv6_prefix *, void *), void *arg,
		struct ipv6_prefix *offset);

int client_for_each(int (*cb)(struct in6_addr *, void *),
		void *arg, unsigned int *offset);



#endif /* CLIENT_H_ */
