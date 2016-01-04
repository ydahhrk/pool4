#ifndef POOL4_H_
#define POOL4_H_

#include <stdio.h>
#include <stdlib.h>


void pool4_init();

int pool4_add(__u32 mark, __u8 proto,  struct in_addr addr,
		struct port_range *range);

int pool4_rm(__u32 mark, __u8 proto, struct in_addr addr,
		struct port_range *range);

int pool4_flush(void);

bool pool4_contains(__u32 mark, __u8 proto, struct in_addr addr,
		struct port_range *range);

bool pool4_is_empty(void);

int pool4_count(void);

void pool4_print_all(void);

int pool4_foreach_sample(int (*cb)(struct pool4_entry *, void *), void *arg,
		struct pool4_entry *offset);

#endif /* POOL4_H_ */
