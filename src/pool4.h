#ifndef _JOOL_POOL4_H
#define _JOOL_POOL4_H

#include "types.h"

int pool4_init(void);
void pool4_destroy(void);

int pool4_allocate(struct ipv4_transport_addr *result);

#endif /* _JOOL_POOL4_H */
