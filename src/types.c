#include "types.h"

char *ip4_to_str (unsigned int ip, char *buffer)
{
	sprintf (buffer, "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xff,
			(ip >> 8) & 0xff, ip & 0xff);
	return buffer;
}
