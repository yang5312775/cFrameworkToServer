#ifndef _H_RANDOM
#define _H_RANDOM
#include<basic.h>
#include"en_de_crypt.h"

//��ʼ�����������, (int32_t)time(NULL)
inline void srand_init(int32_t seed);

// sh_rand  - �õ�[0, INT32_MAX]�����
inline int32_t srand_get_int32(void);

// sh_rands - �õ�[min, max] ��Χ�������
inline int32_t srand_get_limit_int32(int32_t min, int32_t max);

// sh_randk - �õ�һ��64λ��key
inline int64_t srand_get_int64(void);

//��ȡһ��αUUID
void srand_get_uuid(unsigned char UUID[16]);


#endif