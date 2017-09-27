#ifndef _H_RANDOM
#define _H_RANDOM
#include<basic.h>
#include"en_de_crypt.h"

//初始化随机数种子, (int32_t)time(NULL)
inline void srand_init(int32_t seed);

// sh_rand  - 得到[0, INT32_MAX]随机数
inline int32_t srand_get_int32(void);

// sh_rands - 得到[min, max] 范围内随机数
inline int32_t srand_get_limit_int32(int32_t min, int32_t max);

// sh_randk - 得到一个64位的key
inline int64_t srand_get_int64(void);

//获取一个伪UUID
void srand_get_uuid(unsigned char UUID[16]);


#endif