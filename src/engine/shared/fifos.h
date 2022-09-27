extern const char *prognm;
extern FILE *infifos[MAX_CLIENTS];
extern FILE *outfifos[MAX_CLIENTS];

FILE *openfifo(FPARS(const char, *fnm, *mode));

