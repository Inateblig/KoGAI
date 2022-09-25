#include <base/do.h>
#include <base/vmath.h>
#include "area.h"

int
areahaspos(struct area *a, vec2 p)
{
	#define W(I, C) p.C >= a->c.C - a->d && p.C <= a->c.C + a->d
	return do2e(W, &&);
	#undef W
}
