#define ai_NRAYS 8
#define ai_RAYLEN (32 * 16)

#define ai_AREADIM (g_Config.m_ClAreaSize)

extern CNetObj_PlayerInput ai_inp;
extern int ai_gaveinp;
extern int ai_selfkill;
extern int ai_availjumps;
extern int ai_isdone;
extern int ai_CID;
extern float ai_htds[ai_NRAYS];
extern float ai_ftds[ai_NRAYS];

extern struct area ai_curarea;
extern struct cycbuf ai_prevareas;

