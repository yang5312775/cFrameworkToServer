#ifndef _H_BASIC
#define _H_BASIC
//standard c include file
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <float.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>

//third party include
#include<pthread.h>
#include<mysql.h>
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/evp.h>  
#include <openssl/bio.h>  
#include <openssl/buffer.h>  

//internal include file
#include"define.h"
#include"config.h"
#include"log.h"
#include"memory_pool.h"
#include"random.h"
#include"basic_struct\dict.h"
#include"basic_struct\list.h"
#include"log_new.h"
//linux include file
#ifdef __GNUC__
#include <unistd.h>
#include <termio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/select.h>
#include <sys/resource.h>
//windows include file
#elif _MSC_VER
#include<windows.h>
#include <direct.h> 
#include <conio.h>
#include <ws2tcpip.h>
#include <winerror.h>
#endif


//��С��ת��
#define ENDIAN_WORD(w) ( ( (w) & 0x00FFu) << 8 | ( (w) & 0xFF00u) >> 8 ) 
#define ENDIAN_DWRD(d) ( ( (d) & 0x000000FFu) << 24 | ( (d) & 0x0000FF00u) << 8 | ( (d) & 0x00FF0000u) >> 8 | ( (d) & 0xFF000000u) >> 24 ) 

void SLEEP(int Seconds);


//��16�����ַ���ת��Ϊ16��������
void StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen);

// ��16������ת��Ϊ�ַ���
void HexToStr(unsigned char *pbDest, unsigned char *pbSrc, int nLen);

//��ȡ��ǰʱ���ַ���
void getLocalTimeStr(char * timeStr, int buffLen);

//��ȡָ��ʱ���ַ���
void getTimeStr(unsigned int time, char * timeStr, int buffLen);

//�����ַ�������hashֵ
unsigned int calc_hash(const char *key);
unsigned
tstr_hash(const char * str);
//ȥ���ַ����еĿո񣬷���һ��malloc���ַ���
char *Trim(const char *str);

#ifdef __GNUC__
//����Ŀ¼·���������ھʹ���
int mkdirR(char *strPath);

//��ָ�����ļ����ļ���·�������ھ�ֱ�Ӵ���
int TouchR(char *strFileName, const char *mode, FILE *&pF)

//linux������   �����ػ�����
int daemon(void)
#endif

// 
// ����̨�����������Ϣ��ʾ��Ϣ, ����fmt������ "" �������ַ���
// CERR         -> �򵥵���Ϣ��ӡ
// CERR_EXIT    -> ���������Ϣ, ���Ƴ���ǰ����
// CERR_IF      -> if�����, ������ϱ�׼����ֱ���˳�
// 
#ifndef _H_CERR
#define _H_CERR

#undef	CERR
#define CERR(fmt, ...) \
    fprintf(stderr, "[%s:%s:%d][%d:%s]" fmt "\n", __FILE__, __func__, __LINE__, errno, strerror(errno), ##__VA_ARGS__)

#define CERR_EXIT(fmt, ...) \
    CERR(fmt, ##__VA_ARGS__), exit(EXIT_FAILURE)

#define CERR_IF(code) \
    if((code) < 0) \
        CERR_EXIT(#code)

//
// RETURN - ��ӡ������Ϣ, ��return ����ָ�����
// val		: return�Ķ���, ����Ҫ return void; ʱ���� ',' �͹� or NIL
// fmt		: ˫���Ű����ĸ�ʽ���ַ���
// ...		: fmt�ж�Ӧ�Ĳ���
// return	: val
// 
#define NIL
#define RETURN(val, fmt, ...) \
    do { \
        CERR(fmt, ##__VA_ARGS__); \
        return val; \
    } while(0)


#endif



#ifdef __GNUC__
/*
* ��Ļ�����, ����ϵͳ�ű�
*  return	: void
*/
#define sh_cls() \
        printf("\ec")

//
// getch - �����õ��û������һ���ַ�, linuxʵ��
// return	: ���صõ��ַ�
//
extern int getch(void);

//
// sh_mkdir - ͨ�õĵ���Ŀ¼������ ��ͬ�� shell> mkdir path
// path		: Ŀ¼·��������
// return	: 0��ʾ�ɹ�, -1��ʾʧ��, ʧ��ԭ���� errno
// 
#define sh_mkdir(path) \
        mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)

#elif _MSC_VER


#define sh_cls() \
        system("cls")

#define sh_mkdir(path) \
        mkdir(path)

#else
#	error "error : Currently only supports the Best New CL and GCC!"
#endif

// ���˫���ŵĺ� 
#define _STR(v) #v
#define CSTR(v)	_STR(v)

// ��ȡ���鳤��,ֻ�����������ͻ�""�ַ�������,���߰���'\0'
#define LEN(a) (sizeof(a) / sizeof(*(a)))

// �ÿղ���, v�����Ǹ�����
#define BZERO(v) \
        memset(&(v), 0, sizeof(v))

/*
* �Ƚ������ṹ��ջ�������Ƿ����,��ȷ���true,���ȷ���false
* a	: ��һ���ṹ��ֵ
* b	: �ڶ����ṹ��ֵ
*		: ��ȷ���true, ����false
*/
#define STRUCTCMP(a, b) (!memcmp(&a, &b, sizeof(a)))

//
// EXTERN_RUN - �򵥵�����, ������ʹ�õĺ�
// test		: ��Ҫִ�еĺ�������
//
#define EXTERN_RUN(test, ...) \
    do { \
	    extern void test(); \
	    test (__VA_ARGS__); \
    } while(0)

// �򵥵�timeʱ���¼��
#define TIME_PRINT(code) \
    do { \
	    clock_t $s, $e; \
	    $s = clock(); \
	    code \
	    $e = clock(); \
	    printf("Now code run time:%lfs.\n", ((double)$e - $s) / CLOCKS_PER_SEC); \
    } while (0)

#if 0
//
// sh_pause - �ȴ��ĺ� �Ǹ����߳�û�м��� | "�밴���������. . ."
// return	: void
//
extern void sh_pause(void);

#ifndef SH_PAUSE

#   ifdef _DEBUG
#       define SH_PAUSE() atexit(sh_pause)
#   else
#       define SH_PAUSE() /* ��˵��, �����¿�ʼ�� */
#   endif

#endif // !INIT_PAUSE
#endif

// ����һЩͨ�õĺ���ָ�����, ��Ҫ���ڻ���ķ�װ.
// �й��캯��, ��������, �ȽϺ���, ��ѯ���� ... 
// icmp_f   - int icmp(const void * ln, const void * rn); ��׼�ṹ
// each_f   - int <-> int, eachѭ������, arg �ⲿ����, node �ڲ��ڵ�
// start_f  - pthread �߳������ĸ���������, �����Ż�
//
typedef int(*icmp_f)();
typedef void *  (*vnew_f)();
typedef void(*node_f)(void * node);
typedef int(*each_f)(void * node, void * arg);
typedef void *  (*start_f)(void * arg);

#endif