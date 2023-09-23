#include <cstdio>
#include <cerrno>
#include <poll.h>
#include <set>

#include <base/util.h>
#include <base/cycbuf.h>
#include <base/vmath.h>
#include <game/generated/protocol.h>
#include <game/collision.h>
#include <game/mapitems.h>
#include <game/client/ray.h>
#include <game/client/prediction/entities/character.h>
#include <engine/shared/protocol.h>
#include <engine/shared/fifos.h>

#include "ai.h"

#define V2I "%d %d"
#define V2AI(V) (int)(V).x, (int)(V).y
#define V2F "%f %f"
#define V2A "%a %a"
#define V2AF(V) (V).x, (V).y

CNetObj_PlayerInput ai_inp;
float ai_htds[ai_NRAYS], ai_ftds[ai_NRAYS];
int ai_wantskill;
int ai_gaveinp;
int ai_CID;
int ai_killtick;
int ai_waitsreply;
int ai_rinp_tick, ai_reply_tick;
bool ai_gotrwd[256];
float rwdcrctpath;
//float sum_rwdcrctpath = 0;
//float sum_rwdwrngpath = 0;

int
ai_getinp(int tick)
{
	struct pollfd pfd;
	int haveread, tsk;
	char inpbuf[256];

	haveread = 0;
	pfd.fd = fileno(infifo);
	pfd.events = POLLIN;
ckinp:
	switch (poll(&pfd, 1, 0)) {
	case -1:
		ferrn("poll");
	case 0:
		ai_waitsreply |= haveread;
		return haveread;
	}

	ai_rinp_tick = tick;
	if (!(fgets(inpbuf, sizeof inpbuf, infifo)))
		ferrn("fgets");
	sscanf(inpbuf, "%d %d %d %d %d %d",
		&ai_inp.m_Direction,
		&ai_inp.m_TargetX,
		&ai_inp.m_TargetY,
		&ai_inp.m_Jump,
		&ai_inp.m_Hook,
		&tsk);
	ai_wantskill |= tsk;
	haveread = 1;
	ai_gaveinp = 1;
	goto ckinp;
}

void
ai_reply(CCharacter *ch, int tick)
{
	CCharacterCore core;
	CCollision *cln;
	vec2 ppos, pos, hp, vel;
	size_t i;
	int hs = 0;

	if (!ai_waitsreply)
		return;
	ai_waitsreply = 0;
	ai_reply_tick = tick;

	core = ch->GetCore();
	cln = core.Collision();
	pos = core.m_Pos;
	ppos = ch->m_PrevPos;
	hp = core.m_HookPos - pos;
	vel = core.m_Vel;
	vel.x = (int)vel.x;
	vel.y = (int)vel.y;
	hs = core.m_HookState;

	struct rwdtile {
		int tf, tl; /* tile first, tile last */
		int rwd;
	} rwdtiles[] = {
		{ TILE_FREEZE },
		{ TILE_START },
		{ TILE_FINISH },
//		{ TILE_TIME_CHECKPOINT_FIRST, TILE_TIME_CHECKPOINT_LAST },
	};
	for (i = 0; i < NELM(rwdtiles); i++) {
		struct rwdtile *rt;
		int t;
		rt = &rwdtiles[i];
		/* zogtib big-brain magic 🪄 */
		if ((!rt->tl && !ai_gotrwd[t = rt->tf] && cln->MovedThruTile(ppos, pos, t) >= 0) ||
		(rt->tl && (t = cln->MovedThruRange(ppos, pos, rt->tf, rt->tl)) >= 0 && !ai_gotrwd[t])) {
			if (t < TILE_TIME_CHECKPOINT_FIRST) {
				rt->rwd = 1;
			} else {
				rt->rwd = t - TILE_TIME_CHECKPOINT_FIRST + 1;
			}
			ai_gotrwd[t] = 1;
			if (t == TILE_FREEZE) {
				ai_killtick = tick;
			}
		}
	}

	if (tick >= ai_killtick + 10)
		ai_gotrwd[TILE_FREEZE] = 0;

	if (cln->MovedThruTile(ppos, pos, ENTITY_OFFSET + ENTITY_SPAWN) >= 0 && tick <= ai_killtick + 10) {
		memset(ai_gotrwd, 0, sizeof ai_gotrwd);
		ai_killtick = 0;
	}

//	float htds[ai_NRAYS], ftds[ai_NRAYS]; /* hookable/freeze tiles distatnce-s */
	gettiledist(ai_htds, NELM(ai_htds), cln, pos, TILE_SOLID);
	gettiledist(ai_ftds, NELM(ai_ftds), cln, pos, TILE_FREEZE);

	vec2 pathv = ai_getareafld(pos.x / 32, pos.y / 32, ai_ASZ);
	/* correct path reward */
	rwdcrctpath = dot(vel, pathv);

	vel = vel / 6000.0;
	hp = hp / 800.0;

	float hsf = hs / 7.0;

	fprintf(outfifo, V2A " " V2A " " V2A " %a",
		V2AF(vel), V2AF(hp), V2AF(pathv), hsf);
	for (i = 0; i < NELM(rwdtiles); i++)
		fprintf(outfifo, " %d", rwdtiles[i].rwd);
	fprintf(outfifo, " %a", rwdcrctpath);

//	printf("vel:" V2I " step: %5f, sumcrct: %5f, sumwrng: %5f\n", V2AI(vel), rwdcrctpath, sum_rwdcrctpath, sum_rwdwrngpath);

	for (i = 0; i < NELM(ai_htds); i++)
		fprintf(outfifo, " %a", ai_htds[i]);
	for (i = 0; i < NELM(ai_ftds); i++)
		fprintf(outfifo, " %a", ai_ftds[i]);
	fprintf(outfifo, "\n"); /* line-buffered */
}

intern int fldw, fldh;
intern signed char *ai_field;

struct TileV {
	float d;
	ivec2 p;

	int operator<(const TileV &t) const
	{
		return d < t.d;
	}
};

intern int
issolid(int t)
{
	return t == TILE_SOLID || t == TILE_NOHOOK || t == TILE_FREEZE;
}

intern int
tileidxat(FPARS(int, x, y))
{
	return y * fldw + x;
}

intern int
tileidxatv(ivec2 p)
{
	return tileidxat(p.x, p.y);
}

intern int
tileat(CCollision *c, FPARS(int, x, y))
{
	return c->GetTileIndex(tileidxat(x, y));
}

intern int
tileatv(CCollision *c, ivec2 p)
{
	return tileat(c, p.x, p.y);
}

void
ai_setupfield(CCollision *cln)
{
	std::multiset<TileV> tilevs;
	TileV tv;
	float *bd; /* best distance */
	int dx, dy, x, y, i;

	fldw = cln->GetWidth();
	fldh = cln->GetHeight();

	if (!(ai_field = (decltype(ai_field))realloc(ai_field, fldw * fldh * sizeof *ai_field)))
		ferrn("realloc");
	if (!(bd = (decltype(bd))malloc(fldw * fldh * sizeof *bd)))
		ferrn("malloc");

	tv.d = 0.f;
	for (x = 0; x < fldw; x++) {
		tv.p.x = x;
		for (y = 0; y < fldh; y++) {
			i = tileidxat(x, y);
			ai_field[i] = 4;
			if (cln->GetTileIndex(i) == TILE_FINISH) {
				tv.p.y = y;
				tilevs.insert(tv);
				bd[i] = 0.f;
			} else
				bd[i] = HUGE_VALF;
		}
	}

	while (tilevs.size()) {
		auto const ctv = tilevs.begin();

		for (dx = -1; dx <= 1; dx++)
		for (dy = -1; dy <= 1; dy++) {
			if (!dx && !dy)
				continue;
			tv.p.x = ctv->p.x + dx;
			tv.p.y = ctv->p.y + dy;
			if (tv.p.x < 0 || tv.p.x >= fldw ||
			    tv.p.y < 0 || tv.p.y >= fldh)
				continue;

			tv.d = ctv->d;
			if (dx && dy) {
				if (issolid(tileat(cln, tv.p.x, ctv->p.y)) ||
				    issolid(tileat(cln, ctv->p.x, tv.p.y)))
					continue;
				tv.d += M_SQRT2;
			} else
				tv.d += 1.f;
			if (issolid(tileatv(cln, tv.p)))
				continue;

			i = tileidxatv(tv.p);
			if (bd[i] <= tv.d)
				continue;
			bd[i] = tv.d;
			ai_field[i] = (1 - dy) * 3 + (1 - dx);
			tilevs.insert(tv);
		}
		tilevs.erase(ctv);
	}

	free(bd);
}

vec2
ai_getfield(FPARS(int, x, y))
{
	vec2 dir;
	int v;

	if (x < 0 || x >= fldw ||
	    y < 0 || y >= fldh)
		return vec2(0, 0);
	v = ai_field[tileidxat(x, y)];
	dir.x = v % 3 - 1;
	dir.y = v / 3 - 1;
	return dir;
}

vec2
ai_getareafld(FPARS(int, x, y, asz))
{
	vec2 af;
	int dx, dy, n;

	n = 0;
	for (dx = -asz; dx <= asz; dx++)
	for (dy = -asz; dy <= asz; dy++) {
		vec2 v = ai_getfield(x + dx, y + dy);
		if (!v.x && !v.y) {
			if (!dx && !dy)
				goto retnone;
			continue;
		}
		af += v;
		n++;
	}
	if (n) {
		return normalize(af / n);
	}
retnone:
	return vec2(0.f, 0.f);
}
