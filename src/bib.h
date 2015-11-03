#ifndef _JOOL_BIB_H
#define _JOOL_BIB_H

#include "types.h"

int bibdb_init(void);
void bibdb_destroy(void);

bool bibdb_contains4(const struct ipv4_transport_addr *addr,
		const l4_protocol proto);

#endif /* _JOOL_BIB_H */
