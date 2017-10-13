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
// parameter(s): [OUT] pbDest - 输出缓冲区
//	[IN] pbSrc - 字符串
//	[IN] nLen - 16进制数的字节数(字符串的长度/2)
// return value:
// remarks : 将字符串转化为16进制数
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
// parameter(s): [OUT] pbDest - 存放目标字符串
//	[IN] pbSrc - 输入16进制数的起始地址
//	[IN] nLen - 16进制数的字节数
// return value:
// remarks : 将16进制数转化为字符串
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

//获取当前时间字符串
void getLocalTimeStr(char * timeStr, int buffLen)
{
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timeStr, buffLen, "%Y:%m:%d %H:%M:%S", timeinfo);
}

//获取指定时间字符串
void getTimeStr(unsigned int time, char * timeStr, int buffLen)
{
	time_t timer = time;//time_t就是long int 类型
	struct tm * timeinfo;
	timeinfo = localtime(&timer);
	strftime(timeStr, buffLen, "%Y:%m:%d %H:%M:%S", timeinfo);
}


//根据字符串产生hash值
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
// Brian Kernighan与 Dennis Ritchie 简便快捷的 hash算法
// str		: 字符串内容
// return	: 返回计算后的hash值
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
/*  函 数 名：Trim
/*  函数功能：C语言版Trim()函数，去掉字符串中的空字符
/*  参    数：
/*            str	：源字符串
/*  返 回 值：
/*            返回去掉空字符后的字符串
/*
/*	依赖的头文件：
/*					#include <stdlib.h>
/*					#include <string.h>/*
/*
/*  作    者：X攻城狮
/*  日    期：2015年11月7日
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
* Describe: 创建目录;
*    Input: char *strPath: 要创建的目录;
*   Output: void
*   Return: 0 : 成功
*           -1: 失败
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
* Describe: 打开一个指定的文件;
*    Input: char *strFileName: 要打开的文件名;
*           const char *mode : 要打开的模式;
*   Output: FILE *&pF: 打开文件的句柄;
*   Return: 0 : 成功
*           -1: 失败
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

//linux环境下   创建守护进程
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