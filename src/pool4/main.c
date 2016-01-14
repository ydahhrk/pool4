#include "types.h"
#include "pool4.h"

int main()
{
	pool4_init();

	struct pool4_entry *one = malloc(sizeof(*one));
	one->mark = 1;
	one->proto = 1;
	one->addr.s_addr = cpu_to_be32(0xc0000201);
	one->range.min = 10;
	one->range.max = 20;

	struct pool4_entry *two = malloc(sizeof(*two));
	two->mark = 2;
	two->proto = 2;
	two->addr.s_addr = cpu_to_be32(0xc0000202);
	two->range.min = 30;
	two->range.max = 40;

	struct pool4_entry *three = malloc(sizeof(*three));
	three->mark = 3;
	three->proto = 3;
	three->addr.s_addr = cpu_to_be32(0xc0000201);
	three->range.min = 50;
	three->range.max = 60;


	struct pool4_entry *four = malloc(sizeof(*four));
	four->mark = 4;
	four->proto = 4;
	four->addr.s_addr = cpu_to_be32(0xc0000201);
	four->range.min = 70;
	four->range.max = 80;

	//Adding 4 elements to the list: test_list
	printf("Adding 4 elements to the list...\n\n");
	pool4_add(one->mark, one->proto, one->addr, &one->range);

	pool4_add(two->mark, two->proto, two->addr, &two->range);

	pool4_add(three->mark, three->proto, three->addr, &three->range);

	pool4_add(four->mark, four->proto, four->addr, &four->range);


	struct client_mask_domain domain;

	struct ipv4_transport_addr mask;
	int error;
	char addr[16];
	domain.first = malloc(sizeof(domain.first));

	domain.first->l3.s_addr = cpu_to_be32(0xc0000201);
	domain.first->l4 = 14;
	domain.step = 1;
	domain.count = 7;
	error = pool4_get_nth_taddr(&domain, 5, &mask);
	printf("%s #%u\n\n\n", ip4_to_str(mask.l3.s_addr,addr), mask.l4);


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



	//Flushing the list...
	printf("Flushing the list...\n");
	pool4_flush();
	pool4_print_all();
	printf("%d\n\n", pool4_count());

	//Checking if list is empty...
	pool4_is_empty();

*/

	return 0;
}
