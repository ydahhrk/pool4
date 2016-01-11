<<<<<<< HEAD
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
=======
#ifndef POOL4_H_
#define POOL4_H_

#include <stdio.h>
#include <stdlib.h>
>>>>>>> refs/remotes/origin/A.-Avalos


void pool4_init();

<<<<<<< HEAD
int pool4_add(__u32 mark, __u8 proto,  struct in_addr addr,
		struct port_range *range, struct pool4_entry *entry);

static bool pool4_entry_equals(struct pool4_entry new, struct pool4_entry old);

int pool4_rm(__u32 mark, __u8 proto, struct in_addr addr,
		struct port_range *range, struct pool4_entry *entry);

int pool4_flush(struct pool4_entry *entry);

bool pool4_is_empty(struct pool4_entry *entry);

void pool4_print_all(struct pool4_entry *entry);
=======
int pool4_add(__u32 mark, __u8 proto,  struct in_addr *addr,
		struct port_range *range);

int pool4_rm(__u32 mark, __u8 proto, struct in_addr *addr,
		struct port_range *range);

int pool4_flush(void);

bool pool4_contains(__u32 mark, __u8 proto, struct in_addr *addr,
		struct port_range *range);

bool pool4_is_empty(void);

int pool4_count(void);

void pool4_print_all(void);

int pool4_foreach_sample(int (*cb)(struct pool4_entry *, void *), void *arg,
		struct pool4_entry *offset);

int pool4_foreach_taddr4(int (*cback)(struct pool4_mask *, void *), void *arg,
		unsigned int offset);
>>>>>>> refs/remotes/origin/A.-Avalos

#endif /* POOL4_H_ */
