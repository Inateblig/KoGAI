struct fifo {
	FILE *f;
	const char *evnm;
	const char *fnm;
};

extern const char *prognm;
extern struct fifo fifoout, fifoin;
