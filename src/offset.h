#ifndef __OFFSET_GUARD__
#define __OFFSET_GUARD__

#include <stdio.h>
#include <stdlib.h>
#include "io.h"

struct zone {
	unsigned char	*start;
	unsigned char	*inventory;
	unsigned char	*stats;
	unsigned char	*kills;
	unsigned char	*tags;
	unsigned char	*perks;
	unsigned char	*exp;
	unsigned char	*traits;
};

typedef struct zone	ZONE;

ZONE getzone (unsigned char *pointer);

#endif
