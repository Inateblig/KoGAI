#include <cstdio>
#include <cerrno>
#include <poll.h>

#include <base/util.h>
#include <base/cycbuf.h>
#include <base/vmath.h>
#include <game/generated/protocol.h>
#include <game/collision.h>
#include <game/mapitems.h>
#include <game/server/area.h>
#include <game/server/ray.h>
#include <game/server/entities/character.h>
#include <engine/shared/protocol.h>
#include <engine/shared/fifos.h>

#include "ai.h"

#define V2F "%d %d"
#define V2A(V) (int)(V).x, (int)(V).y

int ai_nenvs;

int
ai_getinp(int cid, CNetObj_PlayerInput *inp, int *sk)
{
	struct pollfd pfd;
	int haveread, tsk;
	char inpbuf[256];

	*sk = haveread = 0;
	pfd.fd = fileno(infifos[cid]);
	pfd.events = POLLIN;
ckinp:
	switch (poll(&pfd, 1, 0)) {
	case -1:
		ferrn("poll");
	case 0:
		return haveread;
	}

	if (!(fgets(inpbuf, sizeof inpbuf, infifos[cid])))
		ferrn("fgets");
	sscanf(inpbuf, "%d %d %d %d %d %d",
		&inp->m_Direction,
		&inp->m_TargetX,
		&inp->m_TargetY,
		&inp->m_Jump,
		&inp->m_Hook,
		&tsk);
	*sk |= tsk;
	haveread = 1;
	goto ckinp;
}

void
ai_reply(int cid, CCharacter *ch)
{
	CCharacterCore core;
	CCollision *cln;
	vec2 ppos, pos, hp, vel;
	size_t i;
	int hs, j;

	core = ch->GetCore();
	cln = core.Collision();
	pos = core.m_Pos;
	ppos = ch->m_PrevPos;
	hp = core.m_HookPos - pos;
	vel = core.m_Vel;
	hs = core.m_HookState;
	j = core.m_Jumps & 3;

	static struct {
		int area;
		int freeze;
		int start;
		int finish;
		int ckpnt;
	} rwd;

	rwd.area = ckareas(&ch->curarea, &ch->prevareas, pos);

	rwd.freeze = cln->MovedThruTile(ppos, pos, TILE_FREEZE);
	rwd.start = cln->MovedThruTile(ppos, pos, TILE_START);
	rwd.finish = cln->MovedThruTile(ppos, pos, TILE_FINISH);
	rwd.ckpnt = cln->MovedThruRange(ppos, pos, TILE_TIME_CHECKPOINT_FIRST,
		TILE_TIME_CHECKPOINT_LAST);

	float htds[ai_NRAYS], ftds[ai_NRAYS]; /* hookable/freeze tiles distatnce-s */
	gettiledist(htds, NELM(htds), cln, pos, TILE_SOLID);
	gettiledist(ftds, NELM(ftds), cln, pos, TILE_FREEZE);

	fprintf(outfifos[cid], V2F " " V2F " %d %d" " %d %d %d %d %d",
		V2A(vel), V2A(hp),
		hs, j,
		rwd.freeze, rwd.start, rwd.finish, rwd.area, rwd.ckpnt);

	for (i = 0; i < NELM(htds); i++)
		fprintf(outfifos[cid], " %a", htds[i]);
	for (i = 0; i < NELM(ftds); i++)
		fprintf(outfifos[cid], " %a", ftds[i]);
	fprintf(outfifos[cid], "\n"); /* line-buffered */
}
