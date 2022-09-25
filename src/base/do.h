/* do.h */

#define do2r(W, B, A, J)\
	B W(0, x) A J\
	B W(1, y) A
#define do2rv(W, B, A, J, ...)\
	B W(0, x, __VA_ARGS__) A J\
	B W(1, y, __VA_ARGS__) A

#define do2(W)\
	do {\
		W(0, x)\
		W(1, y)\
	} while (0)

#define do2e(E, J)\
	(\
		(E(0, x)) J\
		(E(1, y))\
	)

#define do2xz(W)\
	do {\
		W(0, x)\
		W(2, z)\
	} while (0)

#define do2ryx(W, B, A, J)\
	B W(1, y) A J\
	B W(0, x) A

#define do3r(W, B, A, J)\
	B W(0, x) A J\
	B W(1, y) A J\
	B W(2, z) A
#define do3rv(W, B, A, J, ...)\
	B W(0, x, __VA_ARGS__) A J\
	B W(1, y, __VA_ARGS__) A J\
	B W(2, z, __VA_ARGS__) A

#define do3(W)\
	do {\
		W(0, x)\
		W(1, y)\
		W(2, z)\
	} while (0)
#define do3v(W, ...)\
	do {\
		W(0, x, __VA_ARGS__)\
		W(1, y, __VA_ARGS__)\
		W(2, z, __VA_ARGS__)\
	} while (0)

#define do3e(E, J)\
	(\
		(E(0, x)) J\
		(E(1, y)) J\
		(E(2, z))\
	)
#define do3ev(E, J, ...)\
	(\
		(E(0, x, __VA_ARGS__)) J\
		(E(1, y, __VA_ARGS__)) J\
		(E(2, z, __VA_ARGS__))\
	)

#define do4(W)\
	do {\
		W(0, x)\
		W(1, y)\
		W(2, z)\
		W(3, w)\
	} while (0)
#define do4v(W, ...)\
	do {\
		W(0, x, __VA_ARGS__)\
		W(1, y, __VA_ARGS__)\
		W(2, z, __VA_ARGS__)\
		W(3, w, __VA_ARGS__)\
	} while (0)

#define do4r(W, B, A, J)\
	B W(0, x) A J\
	B W(1, y) A J\
	B W(2, z) A J\
	B W(3, w) A
#define do4rv(W, B, A, J, ...)\
	B W(0, x, __VA_ARGS__) A J\
	B W(1, y, __VA_ARGS__) A J\
	B W(2, z, __VA_ARGS__) A J\
	B W(3, w, __VA_ARGS__) A

#define do4e(E, J)\
	(\
		(E(0, x)) J\
		(E(1, y)) J\
		(E(2, z)) J\
		(E(3, w))\
	)
#define do4ev(E, J, ...)\
	(\
		(E(0, x, __VA_ARGS__)) J\
		(E(1, y, __VA_ARGS__)) J\
		(E(2, z, __VA_ARGS__)) J\
		(E(3, w, __VA_ARGS__))\
	)
