#include"en_de_crypt.h"
//aes加密 加密数据必须是16整数倍
int DbsAesEncrypt(unsigned char * key, unsigned char * dataInputOutput, int len)
{
	AES_KEY     aes;
	AES_set_encrypt_key((unsigned char *)key, 128, &aes);
	int count = 0;
	if (len % 16 == 0)
		count = len / 16;
	else
		count = len / 16 + 1;
	for (int i = 0; i < count; i++)
		AES_encrypt((unsigned char *)dataInputOutput + (i * 16), (unsigned char *)dataInputOutput + (i * 16), &aes);

	return 0;
}

//aes解密 数据必须是16整数倍
int DbsAesDecrypt(unsigned char * key, unsigned char * dataInputOutput, int len)
{
	AES_KEY     aes;
	AES_set_decrypt_key((unsigned char *)key, 128, &aes);
	int count = 0;
	if (len % 16 == 0)
		count = len / 16;
	else
		count = len / 16 + 1;
	for (int i = 0; i < count; i++)
		AES_decrypt((unsigned char *)dataInputOutput + (i * 16), (unsigned char *)dataInputOutput + (i * 16), &aes);

	return 0;
}

//aes-cbc加密 
int DbsAesCbcEncrypt(unsigned char * key, unsigned char * iv, unsigned char * dataInputOutput, int len)
{
	AES_KEY     aes;
	AES_set_encrypt_key((unsigned char *)key, 128, &aes);
	AES_cbc_encrypt((unsigned char *)dataInputOutput, (unsigned char *)dataInputOutput, len, &aes, iv, AES_ENCRYPT);
	return 0;
}

//aes-cbc解密
int DbsAesCbcDecrypt(unsigned char * key, unsigned char * iv, unsigned char * dataInputOutput, int len)
{
	AES_KEY     aes;
	AES_set_decrypt_key((unsigned char *)key, 128, &aes);
	AES_cbc_encrypt((unsigned char *)dataInputOutput, (unsigned char *)dataInputOutput, len, &aes, iv, AES_DECRYPT);
	return 0;
}

//获取MD5值
int GetMD5Code(unsigned char * dataInput, int len, unsigned char * MD5)
{
	MD5_CTX md5;
	MD5_Init(&md5);
	MD5_Update(&md5, (unsigned char *)dataInput, len);
	MD5_Final(MD5, &md5);
	return 0;
}

//获取SHA1值
int GetSHA1Code(unsigned char * dataInput, int len, unsigned char * SHA1)
{
	SHA_CTX sha1;
	SHA1_Init(&sha1);
	SHA1_Update(&sha1, (unsigned char*)dataInput, len);
	SHA1_Final(SHA1, &sha1);
	return 0;
}
//ROT13 加解密
char rot13_char(char c)
{
	/* We don't want to use isalpha here; setting the locale would change
	* which characters are considered alphabetical. */
	if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
		return c + 13;
	else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
		return c - 13;
	else
		return c;
}

int DBSBase64Encode(unsigned char *in_str, int in_len, char *out_str)
{
	BIO *b64, *bio;
	BUF_MEM *bptr = NULL;
	size_t size = 0;

	if (in_str == NULL || out_str == NULL)
		return -1;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_write(bio, in_str, in_len);
	BIO_flush(bio);

	BIO_get_mem_ptr(bio, &bptr);
	memcpy(out_str, bptr->data, bptr->length);
	out_str[bptr->length] = '\0';
	size = bptr->length;

	BIO_free_all(bio);
	return size;
}

int DBSBase64Decode(char *in_str, int in_len, unsigned char *out_str)
{
	BIO *b64, *bio;
	BUF_MEM *bptr = NULL;
	int counts;
	int size = 0;

	if (in_str == NULL || out_str == NULL)
		return -1;

	b64 = BIO_new(BIO_f_base64());
	//	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

	bio = BIO_new_mem_buf(in_str, in_len);
	bio = BIO_push(b64, bio);

	size = BIO_read(bio, out_str, in_len);
	out_str[size] = '\0';

	BIO_free_all(bio);
	return size;
}

//获取CRC32值
int GetCRC32Code(unsigned char * dataInput, int len, unsigned char * CRC32)
{
	memset(CRC32, 1, 4);   //暂不实现
	return 0;
}

//RSA公钥密钥对生成
int DBSRsaPublicPrivateKeyGenerate(unsigned char * MOD, unsigned char * PRI, int bitsLen)
{
	BIGNUM *bne = NULL;
	RSA *RsaKey = NULL;
	bne = BN_new();
	BN_set_word(bne, RSA_F4);
	RsaKey = RSA_new();
	RSA_generate_key_ex(RsaKey, bitsLen, bne, NULL);
	BN_bn2bin(RsaKey->n, MOD);
	BN_bn2bin(RsaKey->d, PRI);
	RSA_free(RsaKey);
	return 0;
}

//RSA公钥加密
int DBSRsaPublicEncrypt(unsigned char * MOD, unsigned char * DataInputOutput, int dataLen)
{
	BIGNUM *bne, *bnn;
	bne = BN_new();
	bnn = BN_new();
	RSA *RsaKey = NULL;
	RsaKey = RSA_new();

	BN_set_word(bne, RSA_F4);
	BN_bin2bn(MOD, dataLen, bnn);
	RsaKey->e = bne;
	RsaKey->n = bnn;
	RSA_public_encrypt(dataLen, (unsigned char *)DataInputOutput, (unsigned char *)DataInputOutput, RsaKey, RSA_NO_PADDING);
	RSA_free(RsaKey);
	return 0;
}

//RSA公钥解密
int DBSRsaPublicDecrypt(unsigned char * MOD, unsigned char * DataInputOutput, int dataLen)
{
	BIGNUM *bne, *bnn;
	bne = BN_new();
	bnn = BN_new();
	RSA *RsaKey = NULL;
	RsaKey = RSA_new();
	BN_set_word(bne, RSA_F4);
	BN_bin2bn(MOD, dataLen, bnn);
	RsaKey->e = bne;
	RsaKey->n = bnn;
	RSA_public_decrypt(dataLen, (unsigned char *)DataInputOutput, (unsigned char *)DataInputOutput, RsaKey, RSA_NO_PADDING);
	RSA_free(RsaKey);
	return 0;
}

//RSA私钥加密
int DBSRsaPrivateEncrypt(unsigned char * MOD, unsigned char * PrivExp, unsigned char * DataInputOutput, int dataLen)
{
	BIGNUM *bne, *bnn, *bnd;
	bne = BN_new();
	bnd = BN_new();
	bnn = BN_new();
	RSA *RsaKey = NULL;
	RsaKey = RSA_new();
	BN_set_word(bne, RSA_F4);
	BN_bin2bn(MOD, dataLen, bnn);
	BN_bin2bn(PrivExp, dataLen, bnd);
	RsaKey->e = bne;
	RsaKey->n = bnn;
	RsaKey->d = bnd;
	RSA_private_encrypt(dataLen, (unsigned char *)DataInputOutput, (unsigned char *)DataInputOutput, RsaKey, RSA_NO_PADDING);
	RSA_free(RsaKey);
	return 0;
}

//RSA私钥解密
int DBSRsaPrivateDecrypt(unsigned char * MOD, unsigned char * PrivExp, unsigned char * DataInputOutput, int dataLen)
{
	BIGNUM *bne, *bnn, *bnd;
	bne = BN_new();
	bnd = BN_new();
	bnn = BN_new();
	RSA *RsaKey = NULL;
	RsaKey = RSA_new();
	BN_set_word(bne, RSA_F4);
	BN_bin2bn(MOD, dataLen, bnn);
	BN_bin2bn(PrivExp, dataLen, bnd);
	RsaKey->e = bne;
	RsaKey->n = bnn;
	RsaKey->d = bnd;
	RSA_private_decrypt(dataLen, (unsigned char *)DataInputOutput, (unsigned char *)DataInputOutput, RsaKey, RSA_NO_PADDING);
	RSA_free(RsaKey);
	return 0;
}