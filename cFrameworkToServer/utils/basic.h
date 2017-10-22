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


//大小端转换
#define ENDIAN_WORD(w) ( ( (w) & 0x00FFu) << 8 | ( (w) & 0xFF00u) >> 8 ) 
#define ENDIAN_DWRD(d) ( ( (d) & 0x000000FFu) << 24 | ( (d) & 0x0000FF00u) << 8 | ( (d) & 0x00FF0000u) >> 8 | ( (d) & 0xFF000000u) >> 24 ) 

void SLEEP(int Seconds);


//将16进制字符串转化为16进制数据
void StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen);

// 将16进制数转化为字符串
void HexToStr(unsigned char *pbDest, unsigned char *pbSrc, int nLen);

//获取当前时间字符串
void getLocalTimeStr(char * timeStr, int buffLen);

//获取指定时间字符串
void getTimeStr(unsigned int time, char * timeStr, int buffLen);

//根据字符串产生hash值
unsigned int calc_hash(const char *key);
unsigned
tstr_hash(const char * str);
//去除字符串中的空格，返回一个malloc的字符串
char *Trim(const char *str);

#ifdef __GNUC__
//创建目录路径，不存在就创建
int mkdirR(char *strPath);

//打开指定的文件，文件或路径不存在就直接创建
int TouchR(char *strFileName, const char *mode, FILE *&pF)

//linux环境下   创建守护进程
int daemon(void)
#endif

// 
// 控制台输出完整的消息提示信息, 其中fmt必须是 "" 包裹的字符串
// CERR         -> 简单的消息打印
// CERR_EXIT    -> 输出错误信息, 并推出当前进程
// CERR_IF      -> if语句检查, 如果符合标准错误直接退出
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
// RETURN - 打印错误信息, 并return 返回指定结果
// val		: return的东西, 当需要 return void; 时候填 ',' 就过 or NIL
// fmt		: 双引号包裹的格式化字符串
// ...		: fmt中对应的参数
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
* 屏幕清除宏, 依赖系统脚本
*  return	: void
*/
#define sh_cls() \
        printf("\ec")

//
// getch - 立即得到用户输入的一个字符, linux实现
// return	: 返回得到字符
//
extern int getch(void);

//
// sh_mkdir - 通用的单层目录创建宏 等同于 shell> mkdir path
// path		: 目录路径加名称
// return	: 0表示成功, -1表示失败, 失败原因都在 errno
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

// 添加双引号的宏 
#define _STR(v) #v
#define CSTR(v)	_STR(v)

// 获取数组长度,只能是数组类型或""字符串常量,后者包含'\0'
#define LEN(a) (sizeof(a) / sizeof(*(a)))

// 置空操作, v必须是个变量
#define BZERO(v) \
        memset(&(v), 0, sizeof(v))

/*
* 比较两个结构体栈上内容是否相等,相等返回true,不等返回false
* a	: 第一个结构体值
* b	: 第二个结构体值
*		: 相等返回true, 否则false
*/
#define STRUCTCMP(a, b) (!memcmp(&a, &b, sizeof(a)))

//
// EXTERN_RUN - 简单的声明, 并立即使用的宏
// test		: 需要执行的函数名称
//
#define EXTERN_RUN(test, ...) \
    do { \
	    extern void test(); \
	    test (__VA_ARGS__); \
    } while(0)

// 简单的time时间记录宏
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
// sh_pause - 等待的宏 是个单线程没有加锁 | "请按任意键继续. . ."
// return	: void
//
extern void sh_pause(void);

#ifndef SH_PAUSE

#   ifdef _DEBUG
#       define SH_PAUSE() atexit(sh_pause)
#   else
#       define SH_PAUSE() /* 别说了, 都重新开始吧 */
#   endif

#endif // !INIT_PAUSE
#endif

// 定义一些通用的函数指针帮助, 主要用于基库的封装.
// 有构造函数, 析构函数, 比较函数, 轮询函数 ... 
// icmp_f   - int icmp(const void * ln, const void * rn); 标准结构
// each_f   - int <-> int, each循环操作, arg 外部参数, node 内部节点
// start_f  - pthread 线程启动的辅助函数宏, 方便优化
//
typedef int(*icmp_f)();
typedef void *  (*vnew_f)();
typedef void(*node_f)(void * node);
typedef int(*each_f)(void * node, void * arg);
typedef void *  (*start_f)(void * arg);

#endif