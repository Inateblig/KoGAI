#include <base/do.h>
#include <base/vmath.h>
#include <base/cycbuf.h>
#include "area.h"

int
areahaspos(struct area *a, vec2 p)
{
	#define W(I, C) p.C >= a->c.C - a->d && p.C <= a->c.C + a->d
	return do2e(W, &&);
	#undef W
}

int
ckareas(struct area *cur, struct cycbuf *prev, vec2 p)
{
	struct area *a;
	size_t i;

	#define ARF V2F " %d"
	#define ARA(A) V2A((A).c), (A).d

//	printf("p:" V2F " -- " ARF "\n", V2A(p), ARA(*cur));
	if (areahaspos(cur, p))
		return 0;

	for (i = 1; i <= prev->nea; i++) {
		a = (struct area *)cbelmat(prev, prev->fp + prev->nea - i);
		if (areahaspos(a, p))
			goto cut;
	}
	do {
		cbadd(prev, cur);
		if (p.x < cur->c.x - cur->d)
			cur->c.x -= 2 * cur->d;
		else if (p.x > cur->c.x + cur->d)
			cur->c.x += 2 * cur->d;
		else if (p.y < cur->c.y - cur->d)
			cur->c.y -= 2 * cur->d;
		else if (p.y > cur->c.y + cur->d)
			cur->c.y += 2 * cur->d;
	} while (!areahaspos(cur, p));
	return 1;
cut:
	prev->nea -= i;
	*cur = *a;
	return -(int)i;
}

