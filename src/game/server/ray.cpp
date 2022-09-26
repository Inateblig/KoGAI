#include <math.h>
#include <game/collision.h>
#include <game/generated/protocol.h>
#include <engine/server/ai.h>

void
gettiledist(float ds[], size_t nd, CCollision *cln, vec2 p, int tile)
{
	vec2 e, cp;
	float a;
	size_t i;
	int t;

	for (i = 0; i < nd; i++) {
		a = i * 2*pi / nd;
		e = vec2(sinf(a), -cosf(a)) * ai_RAYLEN;
		if ((t = cln->IntersectLineAllTiles(p, p + e, &cp, 0)) && t == tile)
			ds[i] = length(cp - p) / ai_RAYLEN;
		else
			ds[i] = -1.f;
	}
}
