extern const char *prognm;
extern FILE *infifo;
extern FILE *outfifo;

FILE *openfifo(FPARS(const char, *fnm, *mode));

