#ifndef ENGINE_SERVER_AI
#define ENGINE_SERVER_AI
#include <game/generated/protocol.h>
#include <game/server/entities/character.h>

#define ai_NRAYS 8
#define ai_RAYLEN (32 * 16)

//#define ai_AREADIM (g_Config.m_ClAreaSize)

extern int ai_nenvs;

int ai_getinp(int cid, CNetObj_PlayerInput *inp, int *sk);
void ai_reply(int cid, CCharacter *ch);
#endif
