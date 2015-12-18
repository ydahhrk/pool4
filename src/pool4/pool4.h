#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "list.h"

#ifndef POOL4_H_
#define POOL4_H_

struct pool4_entry {
	__u32 mark;
	__u8 proto;
	struct in_addr addr;
	struct port_range range;
	struct list_hook list;
} pool4_dbase;


void pool4_init();

int pool4_add(__u32 mark, __u8 proto,  struct in_addr addr,
		struct port_range *range, struct pool4_entry *entry);

static bool pool4_entry_equals(struct pool4_entry new, struct pool4_entry old);

int pool4_rm(__u32 mark, __u8 proto, struct in_addr addr,
		struct port_range *range, struct pool4_entry *entry);

int pool4_flush(struct pool4_entry *entry);

bool pool4_is_empty(struct pool4_entry *entry);

void pool4_print_all(struct pool4_entry *entry);

#endif /* POOL4_H_ */
