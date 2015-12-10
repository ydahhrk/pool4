#include "pool4.c"

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

	pool4_add(one->mark, one->proto, one->addr, &one->range,
			&test_list);

	pool4_add(two->mark, two->proto, two->addr, &two->range,
			&test_list);

	pool4_add(three->mark, three->proto, three->addr,
			&three->range, &test_list);

	return 0;
}
