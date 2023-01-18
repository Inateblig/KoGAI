#ifndef ENGINE_SERVER_AI
#define ENGINE_SERVER_AI
#include <game/generated/protocol.h>
#include <game/client/prediction/entities/character.h>

#define ai_NRAYS 3
#define ai_RAYLEN (32 * 16)

//#define ai_AREADIM (g_Config.m_ClAreaSize)

extern CNetObj_PlayerInput ai_inp;

extern int ai_wantskill;
extern int ai_gaveinp;
extern int ai_CID;
extern int ai_killtick;
extern bool ai_gotrwd[256];

int ai_getinp();
void ai_reply(CCharacter *ch, int tick);
#endif
