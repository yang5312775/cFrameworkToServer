#ifndef _H_EN_DE_CRYPT
#define _H_EN_DE_CRYPT
#include <basic.h>


//aes加密
int DbsAesEncrypt(unsigned char * key, unsigned char * dataInputOutput, int len);

//aes解密
int DbsAesDecrypt(unsigned char * key, unsigned char * dataInputOutput, int len);

//aes-cbc加密
int DbsAesCbcEncrypt(unsigned char * key, unsigned char * iv, unsigned char * dataInputOutput, int len);

//aes-cbc解密
int DbsAesCbcDecrypt(unsigned char * key, unsigned char * iv, unsigned char * dataInputOutput, int len);

//获取MD5值
int GetMD5Code(unsigned char * dataInput, int len, unsigned char * MD5);

//获取SHA1值
int GetSHA1Code(unsigned char * dataInput, int len, unsigned char * SHA1);

//ROT13加解密
char rot13_char(char c);

//base64 加密
int DBSBase64Encode(unsigned char *in_str, int in_len, char *out_str);

//base64 解密
int DBSBase64Decode(char *in_str, int in_len, unsigned char *out_str);

//获取CRC32值
int GetCRC32Code(unsigned char * dataInput, int len, unsigned char * CRC32);

//RSA公钥密钥对生成
int DBSRsaPublicPrivateKeyGenerate(unsigned char * PUB, unsigned char * PRI, int bitsLen);

//RSA公钥加密
int DBSRsaPublicEncrypt(unsigned char * MOD, unsigned char * DataInputOutput, int dataLen);

//RSA公钥解密
int DBSRsaPublicDecrypt(unsigned char * MOD, unsigned char * DataInputOutput, int dataLen);

//RSA私钥加密
int DBSRsaPrivateEncrypt(unsigned char * MOD, unsigned char * PrivExp, unsigned char * DataInputOutput, int dataLen);

//RSA私钥解密
int DBSRsaPrivateDecrypt(unsigned char * MOD, unsigned char * PrivExp, unsigned char * DataInputOutput, int dataLen);

#endif