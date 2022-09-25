#define NELM(X) (sizeof (X) / sizeof (X)[0])

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

extern const char *prognm;
#if DBG
//	#define perrpref()
//		eprintf("%s: %s[%d]:%s: ", prognm, __FILE__, __LINE__, __func__)
	#define perrpref() do {} while (0)
	#define dbgf(...) perrf(__VA_ARGS__)
#else
	#define perrpref() eprintf("%s: ", prognm)
	#define dbgf(...) 0
#endif


#define perrnand(DOWHAT, ...)\
	do {\
		perrpref();\
		eprintf(__VA_ARGS__);\
		eprintf(": %s\n", strerror(errno));\
		DOWHAT;\
	} while (0)
#define perrfand(DOWHAT, ...)\
	do {\
		perrpref();\
		eprintf(__VA_ARGS__);\
		eprintf("\n");\
		DOWHAT;\
	} while (0)

#define perrn(...) perrnand(, __VA_ARGS__)
#define perrf(...) perrfand(, __VA_ARGS__)
#define ferrn(...) perrnand(exit(1), __VA_ARGS__)
#define ferrf(...) perrfand(exit(1), __VA_ARGS__)

#define MIN(X, Y) ((X) < (Y) ? X : (Y))
#define MAX(X, Y) ((X) > (Y) ? X : (Y))
#define CLAMP(X, A, B) ((X) < (A) ? A : (X) > (B) ? B : (X))

#define RET(V)\
	do {\
		rv = (V);\
		goto befret;\
	} while (0)

#define SWAP(A, B)\
	do {\
		char t_[sizeof A];\
		memcpy(t_, &(A), sizeof t_);\
		memcpy(&(A), &(B), sizeof t_);\
		memcpy(&(B), t_, sizeof t_);\
	} while (0)

#define intern static

#define PPCAT(A, B) A##B
#define PPECAT(A, B) PPCAT(A, B)
#define PPARG8(_0, _1, _2, _3, _4, _5, _6, _7, _8, ...) _8
#define PPNARGS(...) PPARG8(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

#define FPARS(T, ...)\
	PPECAT(FPARS, PPNARGS(__VA_ARGS__))(T, __VA_ARGS__)

#define FPARS2(T, A, B)\
	T A, T B
#define FPARS3(T, A, B, C)\
	T A, T B, T C
#define FPARS4(T, A, B, C, D)\
	T A, T B, T C, T D
#define FPARS5(T, A, B, C, D, E)\
	T A, T B, T C, T D, T E
#define FPARS6(T, A, B, C, D, E, F)\
	T A, T B, T C, T D, T E, T F
#define FPARS7(T, A, B, C, D, E, F, G)\
	T A, T B, T C, T D, T E, T F, T G
#define FPARS8(T, A, B, C, D, E, F, G, H)\
	T A, T B, T C, T D, T E, T F, T G, T H
