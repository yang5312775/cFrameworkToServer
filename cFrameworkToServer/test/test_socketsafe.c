#include<basic.h>

void test_socketsafe(void)
{
	unsigned char buffer[129+64];
	for (int i = 0; i < 129; i++)
		buffer[i] = i;

//	int ret = encryptData(buffer , 129 , 129+32 , ENC_TYPE_AES);
//	ret = decryptData(buffer , 129 + 32, ENC_TYPE_AES);

//	int ret = getSocketSendBuff(buffer , 129 , 161+32 , 2 , 1 , 1);
//	sock_head s_h;
//	ret = parseRecvBuff(buffer , ret , &s_h);


}


