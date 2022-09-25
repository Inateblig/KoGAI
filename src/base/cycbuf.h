#include "util.h"

struct cycbuf {
	size_t elsz;
	size_t nelm;
	size_t nea, fp;
	void *buf;
};

void cbinit(struct cycbuf *cb, FPARS(size_t, elsz, nelm), void *buf);
void cbadd(struct cycbuf *cb, void *elp);
void *cbget(struct cycbuf *cb);
void *cbelmat(struct cycbuf *cb, size_t idx);
