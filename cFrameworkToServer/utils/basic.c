#include<basic.h>
#include"en_de_crypt.h"
void SLEEP(int Seconds)
{
#ifdef WIN32
	Sleep(Seconds * 1000);
#else
	sleep(Seconds);
#endif
}


/*
// C prototype : void StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - ���������
//	[IN] pbSrc - �ַ���
//	[IN] nLen - 16���������ֽ���(�ַ����ĳ���/2)
// return value:
// remarks : ���ַ���ת��Ϊ16������
*/
void StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen)
{
	char h1, h2;
	unsigned char s1, s2;
	int i;

	for (i = 0; i<nLen; i++)
	{
		h1 = pbSrc[2 * i];
		h2 = pbSrc[2 * i + 1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9)
			s1 -= 7;

		s2 = toupper(h2) - 0x30;
		if (s2 > 9)
			s2 -= 7;

		pbDest[i] = s1 * 16 + s2;
	}
}

/*
// C prototype : void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - ���Ŀ���ַ���
//	[IN] pbSrc - ����16����������ʼ��ַ
//	[IN] nLen - 16���������ֽ���
// return value:
// remarks : ��16������ת��Ϊ�ַ���
*/
void HexToStr(unsigned char *pbDest, unsigned char *pbSrc, int nLen)
{
	char	ddl, ddh;
	int i;

	for (i = 0; i<nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pbDest[i * 2] = ddh;
		pbDest[i * 2 + 1] = ddl;
	}

	pbDest[nLen * 2] = '\0';
}

//��ȡ��ǰʱ���ַ���
void getLocalTimeStr(char * timeStr, int buffLen)
{
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timeStr, buffLen, "%Y:%m:%d %H:%M:%S", timeinfo);
}

//��ȡָ��ʱ���ַ���
void getTimeStr(unsigned int time, char * timeStr, int buffLen)
{
	time_t timer = time;//time_t����long int ����
	struct tm * timeinfo;
	timeinfo = localtime(&timer);
	strftime(timeStr, buffLen, "%Y:%m:%d %H:%M:%S", timeinfo);
}


//�����ַ�������hashֵ
unsigned int calc_hash(const char *key)
{
	const char *end = key + strlen(key);
	unsigned int hash;
	for (hash = 0; key < end; key++)
	{
		hash *= 16777619;
		hash ^= (unsigned int)(unsigned char)*key;
	}
	return hash;
}

//
// Brian Kernighan�� Dennis Ritchie ����ݵ� hash�㷨
// str		: �ַ�������
// return	: ���ؼ�����hashֵ
//
unsigned
tstr_hash(const char * str) {
	register unsigned h = 0;
	if (str) {
		register unsigned c;
		while ((c = *str++))
			h = h * 131 + c;
	}
	return h;
}

/***************************************************************
/*  �� �� ����Trim
/*  �������ܣ�C���԰�Trim()������ȥ���ַ����еĿ��ַ�
/*  ��    ����
/*            str	��Դ�ַ���
/*  �� �� ֵ��
/*            ����ȥ�����ַ�����ַ���
/*
/*	������ͷ�ļ���
/*					#include <stdlib.h>
/*					#include <string.h>/*
/*
/*  ��    �ߣ�X����ʨ
/*  ��    �ڣ�2015��11��7��
/***************************************************************/
char *Trim(const char *str)
{
	unsigned int uLen = strlen(str);

	if (0 == uLen)
	{
		return '\0';
	}

	char *strRet = (char *)malloc(uLen + 1);
	memset(strRet, 0, uLen + 1);

	unsigned int i = 0, j = 0;
	for (i = 0; i<uLen + 1; i++)
	{
		if (str[i] != ' ')
		{
			strRet[j++] = str[i];
		}
	}
	strRet[j] = '\0';

	return strRet;
}


/***************************************************************************************************
*     Name: mkdirR
* Describe: ����Ŀ¼;
*    Input: char *strPath: Ҫ������Ŀ¼;
*   Output: void
*   Return: 0 : �ɹ�
*           -1: ʧ��
*   Notice:
***************************************************************************************************/
#ifdef __GNUC__
int mkdirR(char *strPath)
{
	int     i = 1;
	char    strTemp[256];

	if (NULL == strPath)
	{
		return 0;
	}

	memset(strTemp, 0, sizeof(strTemp));
	sprintf(strTemp, "%s", strPath);

	while (strTemp[i])
	{
		if (strTemp[i] == '/')
		{
			strTemp[i] = '\0';
			if (access(strTemp, F_OK) != 0)
			{
				if (mkdir(strTemp, 0777) == -1)
				{
					printf("Create path:[%s] fail.\n", strTemp);
					return -1;
				}
			}

			strTemp[i] = '/';
		}

		i++;
	}

	return 0;
}

/***************************************************************************************************
*     Name: TouchR
* Describe: ��һ��ָ�����ļ�;
*    Input: char *strFileName: Ҫ�򿪵��ļ���;
*           const char *mode : Ҫ�򿪵�ģʽ;
*   Output: FILE *&pF: ���ļ��ľ��;
*   Return: 0 : �ɹ�
*           -1: ʧ��
*   Notice:
***************************************************************************************************/
int TouchR(char *strFileName, const char *mode, FILE *&pF)
{
	if (pF)
	{
		fclose(pF);
		pF = NULL;
	}

	pF = fopen(strFileName, mode);
	if (!pF)
	{
		mkdirR(strFileName);
		pF = fopen(strFileName, mode);
	}

	if (!pF)
	{
		printf("Open file:[%s] fail.\n", strFileName);
	}

	return pF ? 0 : -1;
}

//linux������   �����ػ�����
int daemon(void)
{
	pid_t pid = fork();

	if (pid != 0) exit(0);//parent

						  //first children
	if (setsid() == -1)
	{
		printf("setsid failed\n");
		assert(0);
		exit(-1);
	}

	umask(0);


	pid = fork();

	if (pid != 0) exit(0);

	//second children 
	chdir("/");

	for (int i = 0; i < 3; i++)
	{
		close(i);
	}


	int stdfd = open("/dev/null", O_RDWR);
	dup2(stdfd, STDOUT_FILENO);
	dup2(stdfd, STDERR_FILENO);

	return 0;
}

#endif