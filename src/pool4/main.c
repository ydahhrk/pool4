#include "types.h"
#include "list.h"
#include "pool4.h"

int main()
{
	struct pool4_entry test_list;
	INIT_LIST_HEAD(&test_list.list);

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

	return 0;
}
