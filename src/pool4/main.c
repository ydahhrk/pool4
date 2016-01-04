#include "types.h"
#include "pool4.h"

int cb(struct pool4_entry *entry, void *arg)
{
	int *arg_int = arg;

	/*
	 * Nota que aquí estoy imprimiendo la dirección según
	 * se hace en el kernel (%pI4).
	 * En tu caso probablemente vas a tener que hacerlo
	 * distinto.
	 */
	if (arg) {
		printf("%pI4 %u-%u %d\n", &entry->addr.s_addr,
				entry->range.min,
				entry->range.max,
				*arg_int);
		(*arg_int)++;
	} else {
		printf("%pI4 %u-%u\n", &entry->addr.s_addr,
				entry->range.min,
				entry->range.max);
	}

	return 0;
}

int main()
{
	pool4_init();

	struct pool4_entry *one = malloc(sizeof(*one));
	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 1;
	one->range.max = 1;

	struct pool4_entry *two = malloc(sizeof(*two));
	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000201);
	two->range.min = 2;
	two->range.max = 2;

	struct pool4_entry *three = malloc(sizeof(*three));
	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000201);
	three->range.min = 3;
	three->range.max = 3;

	struct pool4_entry *four = malloc(sizeof(*four));
	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000201);
	four->range.min = 4;
	four->range.max = 4;

	//Adding 4 elements to the list
	printf("Adding 4 elements to the list...\n\n");
	pool4_add(one->mark, one->proto, one->addr, &one->range);

	pool4_add(two->mark, two->proto, two->addr, &two->range);

	pool4_add(three->mark, three->proto, three->addr, &three->range);

	pool4_add(four->mark, four->proto, four->addr, &four->range);

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
	pool4_contains(one->mark, one->proto, one->addr, &one->range);


	//Removing structure 'one' and printing...
	printf("Removing structure 'one' and printing list...\n");
	pool4_rm(one->mark, one->proto, one->addr, &one->range);
	pool4_print_all();
	printf("\n");

	//Counting the elements...
	printf("Counting the elements:\n");
	printf("%d\n\n", pool4_count());

	//Evaluating again if structure one is in the list...
	printf("Evaluating if structure one is in the list:\n");
	pool4_contains(one->mark, one->proto, one->addr, &one->range);


	//Removing structure 'two' and printing...
	printf("Removing structure 'two' and printing list...\n");
	pool4_rm(two->mark, two->proto, two->addr, &two->range);
	pool4_print_all();
	printf("\n");

	//Counting the elements...
	printf("Counting the elements:\n");
	printf("%d\n\n", pool4_count());

	//Adding again removed elements and printing...
	printf("Adding again removed elements and printing...\n");
	pool4_add(one->mark, one->proto, one->addr, &one->range);
	pool4_add(two->mark, two->proto, two->addr, &two->range);
	pool4_print_all();
	printf("\n");

	/*
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

	error = pool4_foreach_sample(cb, NULL, four);
	if (error)
	 	return error;

	return 0;
}
