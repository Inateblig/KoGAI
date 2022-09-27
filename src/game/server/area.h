#ifndef GAME_SERVER_AREA_H
#define GAME_SERVER_AREA_H
/* square area */
struct area {
	vec2 c; /* center */
	int d; /* dimension */
};

int areahaspos(struct area *a, vec2 p);
int ckareas(struct area *cur, struct cycbuf *prev, vec2 p);
#endif
