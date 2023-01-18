#include <math.h>
#include <game/generated/protocol.h>
#include <game/collision.h>
#include <game/client/prediction/entities/character.h>
#include <engine/client/ai.h>

void
gettiledist(float ds[], size_t nd, CCollision *cln, vec2 p, int tile)
{
	vec2 e, cp;
	float a;
	size_t i;

	for (i = 0; i < nd; i++) {
		a = i * 2*pi / nd;
		e = vec2(sinf(a), -cosf(a)) * ai_RAYLEN;
		if (!cln->IntersectLineTile(&ds[i], p, e, tile))
			ds[i] = -1;
	}
}
