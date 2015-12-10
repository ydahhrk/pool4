/*
 * main.c
 *
 *  Created on: Dec 10, 2015
 *      Author: cfrutos
 */
#include "client.h"


#define cpu_to_be32(x) x


int main() {

	struct client client_list;

	INIT_LIST_HEAD(&client_list.list_hook);

	client_list.ipx.address -> cpu_to_be32(0xc0000201);

	return 0;
}
