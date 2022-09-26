#include <game/generated/protocol.h>
#include <base/cycbuf.h>
#include <base/vmath.h>
#include "ai.h"
#include <game/server/area.h>

int ai_gaveinp;
int ai_selfkill;
int ai_availjumps;
int ai_isdone;
int ai_CID;
float ai_htds[ai_NRAYS];
float ai_ftds[ai_NRAYS];
struct area ai_curarea;
struct cycbuf ai_prevareas;
