#include "types.h"
<<<<<<< HEAD
#include "list.h"
=======
>>>>>>> refs/remotes/origin/A.-Avalos
#include "pool4.h"


int cb(struct pool4_entry *entry, void *arg)
{
	int *arg_int = arg;
	char addr[16];

	if (arg) {
		printf("%s %u-%u %d\n", ip_to_str(entry->addr.s_addr, addr),
				entry->range.min, entry->range.max, *arg_int);
		(*arg_int)++;
	} else {
		printf("%s %u-%u\n", ip_to_str(entry->addr.s_addr, addr),
				entry->range.min, entry->range.max);
	}

	return 0;
}

int cback(struct pool4_mask *mask, void *arg)
{
	int *arg_int = arg;
	char addr[16];

	if (arg) {
		printf("%s %u %d\n", ip_to_str(mask->addr.s_addr, addr),
				mask->port, *arg_int);
		(*arg_int)++;
	} else {
		printf("%s %u\n", ip_to_str(mask->addr.s_addr, addr),
				mask->port);
	}

<<<<<<< HEAD
=======
	if (mask->port == 64)
		return 1; /* Interrumpir temprano. */

	return 0;
}

int main()
{
	pool4_init();

>>>>>>> refs/remotes/origin/A.-Avalos
	struct pool4_entry *one = malloc(sizeof(*one));
	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 4;
	one->range.max = 7;

	struct pool4_entry *two = malloc(sizeof(*two));
	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000232);
	two->range.min = 100;
	two->range.max = 100;

	struct pool4_entry *three = malloc(sizeof(*three));
	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000280);
	three->range.min = 253;
	three->range.max = 256;

<<<<<<< HEAD
	//Adding 3 elements to the list: test_list
	printf("Adding 3 elements to the list...\n\n");
	pool4_add(one->mark, one->proto, one->addr, &one->range, &test_list);

	pool4_add(two->mark, two->proto, two->addr, &two->range, &test_list);

	pool4_add(three->mark, three->proto, three->addr, &three->range,
			&test_list);

	//Checking if list is empty...
	printf("Checking if the list is empty:\n");
	pool4_is_empty(&test_list);

	//Printing the whole list...
	printf("Printing the list...\n");
	pool4_print_all(&test_list);
	printf("\n");

	//Removing structure 'one' and printing...
	printf("Removing structure 'one' and printing list...\n");
	pool4_rm(one->mark, one->proto, one->addr, &one->range, &test_list);
	pool4_print_all(&test_list);
	printf("\n");

	//Removing structure 'two' and printing...
	printf("Removing structure 'two' and printing list...\n");
	pool4_rm(two->mark, two->proto, two->addr, &two->range, &test_list);
	pool4_print_all(&test_list);
	printf("\n");

	//Adding again removed elements and printing...
	printf("Adding again removed elements and printing...\n");
	pool4_add(one->mark, one->proto, one->addr, &one->range, &test_list);
	pool4_add(two->mark, two->proto, two->addr, &two->range, &test_list);
	pool4_print_all(&test_list);
	printf("\n");

	//Flushing the list...
	printf("Flushing the list...\n");
	pool4_flush(&test_list);
	pool4_print_all(&test_list);

	//Checking if list is empty...
	pool4_is_empty(&test_list);
=======
	struct pool4_entry *four = malloc(sizeof(*four));
	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000201);
	four->range.min = 63;
	four->range.max = 68;


>>>>>>> refs/remotes/origin/A.-Avalos


	//Adding 4 elements to the list
	printf("Adding 3 elements to the list...\n\n");
	pool4_add(one->mark, one->proto, &one->addr, &one->range);

	pool4_add(two->mark, two->proto, &two->addr, &two->range);

	pool4_add(three->mark, three->proto, &three->addr, &three->range);

	pool4_add(four->mark, four->proto, &four->addr, &four->range);

	pool4_print_all();
	printf("\n");
	/*
	//Checking if list is empty...
	printf("Checking if the list is empty:\n");
	pool4_is_empty();

	//Counting the elements...
	printf("Counting the elements:\n");
	printf("%d\n\n", pool4_count());

	//Printing the list...
	printf("Printing the list...\n");
	pool4_print_all();
	printf("\n");

	//Evaluating if structure one is in the list...
	printf("Evaluating if structure one is in the list:\n");
	pool4_contains(one->mark, one->proto, &one->addr, &one->range);


	//Removing structure 'one' and printing...
	printf("Removing structure 'one' and printing list...\n");
	pool4_rm(one->mark, one->proto, &one->addr, &one->range);
	pool4_print_all();
	printf("\n");

	//Counting the elements...
	printf("Counting the elements:\n");
	printf("%d\n\n", pool4_count());

	//Evaluating again if structure one is in the list...
	printf("Evaluating if structure one is in the list:\n");
	pool4_contains(one->mark, one->proto, &one->addr, &one->range);


	//Removing structure 'two' and printing...
	printf("Removing structure 'two' and printing list...\n");
	pool4_rm(two->mark, two->proto, &two->addr, &two->range);
	pool4_print_all();
	printf("\n");

	//Counting the elements...
	printf("Counting the elements:\n");
	printf("%d\n\n", pool4_count());

	//Adding again removed elements and printing...
	printf("Adding again removed elements and printing...\n");
	pool4_add(one->mark, one->proto, &one->addr, &one->range);
	pool4_add(two->mark, two->proto, &two->addr, &two->range);
	pool4_print_all();
	printf("\n");


	//Flushing the list...
	printf("Flushing the list...\n");
	pool4_flush();
	pool4_print_all();
	printf("%d\n\n", pool4_count());

	//Checking if list is empty...
	pool4_is_empty();
	*/


	int error;
	int a = 12;

	printf("%d\n", a);
	error = pool4_foreach_sample(cb, &a, NULL);
	if (error)
		return error;
	printf("%d\n\n", a);

	error = pool4_foreach_sample(cb, NULL, one);
	if (error)
	 	return error;
	printf("-------------------------\n");


	/*
	a = 0;
	printf("%d\n", a);
	error = pool4_foreach_taddr4(cback, &a, 0);
	if (error)
		return error;
	printf("%d\n", a);
	*/

	printf("\n");
	a = 0;
	error = pool4_foreach_taddr4(cback, NULL, 1027);
	if (error)
		return error;

	printf("\n");



	return error;
}
