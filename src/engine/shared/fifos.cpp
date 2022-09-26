#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#include <base/system.h>
#include <base/util.h>
#include <sys/stat.h>
#include "protocol.h"

#include "fifos.h"

const char *prognm;
FILE *infifos[MAX_CLIENTS];
FILE *outfifos[MAX_CLIENTS];
int nenvs;

FILE *
openfifo(FPARS(const char, *fnm, *mode))
{
	FILE *f;
	struct stat st;
	int fd, ofl;

	if (*mode == 'r')
		ofl = O_RDONLY;
	else
		ofl = O_WRONLY;

	if ((fd = open(fnm, ofl)) < 0)
		ferrn("open: %s", fnm);
	if (fstat(fd, &st))
		ferrn("fstat: %s", fnm);
	if ((st.st_mode & S_IFMT) != S_IFIFO)
		ferrf("%s: not a fifo file", fnm);
	if (!(f = fdopen(fd, mode)))
		ferrn("fdopen: %s", fnm);
	return f;
}
