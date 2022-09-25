#include <string.h>

#include "util.h"
#include "cycbuf.h"

void
cbinit(struct cycbuf *cb, FPARS(size_t, elsz, nelm), void *buf)
{
	cb->elsz = elsz;
	cb->nelm = nelm;
	cb->nea = cb->fp = 0;
	cb->buf = buf;
}

void
cbadd(struct cycbuf *cb, void *elp)
{
	memcpy(cbelmat(cb, cb->nea), elp, cb->elsz);

	if (cb->nea < cb->nelm)
		cb->nea++;
	else
		cb->fp++;
}

void *
cbget(struct cycbuf *cb)
{
	if (!cb->nea)
		return 0;
	cb->nea--;
	cb->fp++;
	return cbelmat(cb, -1);
}

void *
cbelmat(struct cycbuf *cb, size_t idx)
{
	return (char *)cb->buf + (cb->fp + idx) % cb->nelm * cb->elsz;
}
