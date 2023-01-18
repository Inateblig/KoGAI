#include <cstdio>
#include <cerrno>
#include <poll.h>

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

#define V2F "%d %d"
#define V2A(V) (int)(V).x, (int)(V).y

CNetObj_PlayerInput ai_inp;
int ai_wantskill;
int ai_gaveinp;
int ai_CID;
int ai_killtick;
int ai_waitsreply;
bool ai_gotrwd[256];

int
ai_getinp(void)
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

	if (!(fgets(inpbuf, sizeof inpbuf, infifo)))
		ferrn("fgets");
	sscanf(inpbuf, "%d %d %d %d %d %d",
		&ai_inp.m_Direction,
		&ai_inp.m_TargetX,
		&ai_inp.m_TargetY,
		&ai_inp.m_Jump,
		&ai_inp.m_Hook,
		&tsk);
//	printf("inpbuf %s", inpbuf);
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
	int hs, j;

	if (!ai_waitsreply)
		return;
	ai_waitsreply = 0;

	core = ch->GetCore();
	cln = core.Collision();
	pos = core.m_Pos;
	ppos = ch->m_PrevPos;
	hp = core.m_HookPos - pos;
	vel = core.m_Vel;
	hs = core.m_HookState;
	j = core.m_Jumps & 3;

	struct rwdtile {
		int tf, tl; /* tile first, tile last */
		int rwd;
	} rwdtiles[] = {
		{ TILE_FREEZE },
		{ TILE_START },
		{ TILE_FINISH },
		{ TILE_TIME_CHECKPOINT_FIRST, TILE_TIME_CHECKPOINT_LAST },
	};
	if (ai_gotrwd[TILE_FREEZE] && ai_killtick + 10 <= tick)
		ai_gotrwd[TILE_FREEZE] = 0;
	for (i = 0; i < NELM(rwdtiles); i++) {
		struct rwdtile *rt;
		int t;
		rt = &rwdtiles[i];
		/* zogtib big-brain magic 🪄 */
		if ((!rt->tl && !ai_gotrwd[t = rt->tf] && cln->MovedThruTile(ppos, pos, t) >= 0) ||
		(rt->tl && (t = cln->MovedThruRange(ppos, pos, rt->tf, rt->tl)) >= 0 && !ai_gotrwd[t])) {
			if (t < TILE_TIME_CHECKPOINT_FIRST) {
				rt->rwd = 1;
//				printf("rt->rwd = %d\n", rt->rwd);
			} else {
				rt->rwd = t - TILE_TIME_CHECKPOINT_FIRST + 1;
//				printf("rt->rwd = %d\n", rt->rwd);
			}
				ai_gotrwd[t] = 1;
			if (t == TILE_FREEZE)
				ai_killtick = tick;
		}
	}

	float htds[ai_NRAYS], ftds[ai_NRAYS]; /* hookable/freeze tiles distatnce-s */
	gettiledist(htds, NELM(htds), cln, pos, TILE_SOLID);
	gettiledist(ftds, NELM(ftds), cln, pos, TILE_FREEZE);

	fprintf(outfifo, V2F " " V2F " %d %d",
		V2A(vel), V2A(hp), hs, j);
	for (i = 0; i < NELM(rwdtiles); i++)
		fprintf(outfifo, " %d", rwdtiles[i].rwd);
//	for (i = 0; i < NELM(rwdtiles); i++)
//		printf("%d ", rwdtiles[i].rwd);
//	printf("\n");
	for (i = 0; i < NELM(htds); i++)
		fprintf(outfifo, " %a", htds[i]);
	for (i = 0; i < NELM(ftds); i++)
		fprintf(outfifo, " %a", ftds[i]);
	fprintf(outfifo, "\n"); /* line-buffered */
}
