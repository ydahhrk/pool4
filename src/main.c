#include "bib.h"
#include "pool4.h"

#include <stdio.h>
#include <stdlib.h>

static int do_thing(void)
{
	struct ipv4_transport_addr result;
	int error;

	error = pool4_allocate(&result);
	if (error) {
		printf("taddr_allocate() threw errcode %d.\n", error);
		return error;
	}

	printf("The result is %s#%u\n", inet_ntoa(result.l3), result.l4);
	return 0;
}

static int do_things(void)
{
	int error;

	error = do_thing();
	if (error)
		return error;
	error = do_thing();
	if (error)
		return error;
	error = do_thing();

	return error;
}

static int init(void)
{
	int error;

	error = bibdb_init();
	if (error) {
		printf("bibdb_init() threw errcode %d.\n", error);
		return error;
	}

	error = pool4_init();
	if (error) {
		printf("pool4_init() threw errcode %d.\n", error);
		bibdb_destroy();
		return error;
	}

	return 0;
}

static void destroy(void)
{
	pool4_destroy();
	bibdb_destroy();
}

int main(void)
{
	int error;

	error = init();
	if (error)
		return error;

	bibdb_print();

	error = do_things();
	if (error)
		printf("do_things() threw errcode %d.\n", error);

	destroy();
	return error;
}
