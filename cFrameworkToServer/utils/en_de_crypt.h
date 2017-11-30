#ifndef _H_EN_DE_CRYPT
#define _H_EN_DE_CRYPT
#include <basic.h>

#define ENC_TYPE_NONE	0
#define ENC_TYPE_AES	1
#define ENC_TYPE_RSA	2

typedef struct {
	unsigned char   enc_type;   //��������
	unsigned char	length[4];	//���ݱ����ܺ���ܳ���
	unsigned char	padding;    //����������ĩβ���볤��
	unsigned char	random[4];  //�������ֵ
}enc_head;

//aes����
int DbsAesEncrypt(unsigned char * key, unsigned char * dataInputOutput, int len);

//aes����
int DbsAesDecrypt(unsigned char * key, unsigned char * dataInputOutput, int len);

//aes-cbc����
int DbsAesCbcEncrypt(unsigned char * key, unsigned char * iv, unsigned char * dataInputOutput, int len);

//aes-cbc����
int DbsAesCbcDecrypt(unsigned char * key, unsigned char * iv, unsigned char * dataInputOutput, int len);

//��ȡMD5ֵ
int GetMD5Code(unsigned char * dataInput, int len, unsigned char * MD5);

//��ȡSHA1ֵ
int GetSHA1Code(unsigned char * dataInput, int len, unsigned char * SHA1);

//ROT13�ӽ���
char rot13_char(char c);

//base64 ����
int DBSBase64Encode(unsigned char *in_str, int in_len, char *out_str);

//base64 ����
int DBSBase64Decode(char *in_str, int in_len, unsigned char *out_str);

//��ȡCRC32ֵ
int GetCRC32Code(unsigned char * dataInput, int len, unsigned char * CRC32);

//RSA��Կ��Կ������
int DBSRsaPublicPrivateKeyGenerate(unsigned char * PUB, unsigned char * PRI, int bitsLen);

//RSA��Կ����
int DBSRsaPublicEncrypt(unsigned char * MOD, unsigned char * DataInputOutput, int dataLen);

//RSA��Կ����
int DBSRsaPublicDecrypt(unsigned char * MOD, unsigned char * DataInputOutput, int dataLen);

//RSA˽Կ����
int DBSRsaPrivateEncrypt(unsigned char * MOD, unsigned char * PrivExp, unsigned char * DataInputOutput, int dataLen);

//RSA˽Կ����
int DBSRsaPrivateDecrypt(unsigned char * MOD, unsigned char * PrivExp, unsigned char * DataInputOutput, int dataLen);

//��ĳ�ּ����㷨������������
int encryptData(unsigned char *dataInput, int inputLen, int maxLen, char encryptType);

//��ĳ�ּ����㷨���ܱ����ܵ�����
int decryptData(unsigned char *dataInput, char encryptType);
#endif