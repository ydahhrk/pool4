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

void client_add(struct ipv6_prefix *prefix);

void client_delete(struct ipv6_prefix *prefix);

void client_flush();

void client_exist(struct ipv6_prefix *prefix);

int client_count();

void client_print_all();


#endif /* CLIENT_H_ */
