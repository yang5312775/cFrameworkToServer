#include"random.h"

#define N               (16)
#define MASK            ((1 << N) - 1)
#define LOW(x)          ((x) & MASK)
#define HIGH(x)         LOW((x) >> N)
#define CARRY(x, y)     ((x + y) > MASK) // ��������Ƿ��λ, ����16λ
#define ADDEQU(x, y, z) z = CARRY(x, y); x = LOW(x + y)

#define MUL(x, y, z)	l = (x) * (y); (z)[0] = LOW(l); (z)[1] = HIGH(l)

#define X0              (0x330E)
#define X1              (0xABCD)
#define X2              (0x1234)
#define A0              (0xE66D)
#define A1              (0xDEEC)
#define A2              (0x0005)
#define C               (0x000B)

static uint32_t _x[] = { X0, X1, X2 }, _a[] = { A0, A1, A2 }, _c = C;

static void srand_next(void) {
	uint32_t p[2], q[2], r[2], c0, c1, l;

	MUL(_a[0], _x[0], p);
	ADDEQU(p[0], _c, c0);
	ADDEQU(p[1], c0, c1);
	MUL(_a[0], _x[1], q);
	ADDEQU(p[1], q[0], c0);
	MUL(_a[1], _x[0], r);

	l = c0 + c1 + CARRY(p[1], r[0]) + q[1] + r[1]
		+ _a[0] * _x[2] + _a[1] * _x[1] + _a[2] * _x[0];
	_x[2] = LOW(l);
	_x[1] = LOW(p[1] + r[0]);
	_x[0] = LOW(p[0]);
}

//
// sh_srand - ��ʼ�����������, (int32_t)time(NULL)
// seed		: ������
// return	: void
//
void
srand_init(int32_t seed) {
	_x[0] = X0; _x[1] = LOW(seed); _x[2] = HIGH(seed);
	_a[0] = A0; _a[1] = A1; _a[2] = A2;
	_c = C;
}

// sh_rand  - �õ�[0, INT32_MAX]�����
int32_t
srand_get_int32(void) {
	srand_next();
	return (_x[2] << (N - 1)) + (_x[1] >> 1);
}
// sh_rands - �õ�[min, max] ��Χ�������
int32_t
srand_get_limit_int32(int32_t min, int32_t max) {
	assert(max > min);
	return srand_get_int32() % (max - min + 1) + min;
}
// sh_randk - �õ�һ��64λ��key
int64_t
srand_get_int64(void) {
	uint64_t x = ((srand_get_int32() << N) ^ srand_get_int32()) & INT32_MAX;
	uint64_t y = ((srand_get_int32() << N) ^ srand_get_int32()) & INT32_MAX;
	return ((x << 2 * N) | y) & INT64_MAX;
}

//��ȡһ��αUUID
void srand_get_uuid(unsigned char UUID[16])
{
	char * password = "qewe$%^&*()16gvp";
	int timeee = srand_get_int32();
	unsigned char key[16];
	memcpy(UUID, password, 16);
	memcpy(key, &timeee, 4);
	DbsAesCbcEncrypt(key, key, UUID, 16);
}