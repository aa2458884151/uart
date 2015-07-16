/*******************************************************************************
Copyright: 2019, Innovem Tech. Co., Ltd.
FileName: LTEttyUart.h
*******************************************************************************/
#ifndef _LTE_TTYUART_H_
#define _LTE_TTYUART_H_
#include <stdio.h>  /*标准输入输出定义*/
#include <stdlib.h> /*标准函数库定义*/
#include <unistd.h> /*Unix 标准函数定义*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>   /*文件控制定义*/
#include <termios.h> /*PPSIX 终端控制定义*/
#include <errno.h>   /*错误号定义*/
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#define TTYLENGTH 256
//#define DEBUGTTY
class LTEttyUart
{
public:
    LTEttyUart();
    ~LTEttyUart();
    /***************************************************************************************
    功能 初始化串口配置
    参数  speed 波特率
         databits  数据位   取值 为 7 或者8
         stopbits  停止位   取值为 1 或者2
         parity    效验类型 取值为N,E,O,,S
         path      设备文件路径（全路径）
    返回值 成功返回0
          失败返回-1
    *****************************************************************************************/
    int LTEttyUartInit(int speed, int databits, int stopbits, int parity , const char *path);

    /*****************************************************************************************
    功能  读取一条符合规则的数据
    参数
        rcvbuf 读取数据存放的位置 需要开辟空间  空间不小于100
        sec 超时时间 秒级
        usec 超时时间微秒级
    返回值  成功返回指向读到字符串的长度
           失败返回0
    ****************************************************************************************/
    int ReadOneData(char *rcvbuf, int sec, int usec);
    /****************************************************************************************
    功能；发送数据
    参数；fd 文件描述符
         buf 数据包   数据包中不能包含0xff
         len 数据包的长度   长度不能超过100
    返回值;  正确返回发送的数据长度
            发送失败返回 0
            若长度大于LENGTH 返回 -1
            若数据包中包含非法数据  返回-2

    ****************************************************************************************/
    int SendOneData(char *buf, int len) ;
private:
    /*****************************************************************************************
    功能；读取制定字节数的数据，可以设置超时时间
    参数；timeoutSec   超时时间秒
         timeoutUsec  超时时间微妙
         rcvData      接收到的数据存放的位置
         bufSize      要读到的字节数
    返回值; 成功返回读到的字节数
           失败返回-1
           超时返回 0
    注意 ;  bufSize 一定要大于rcvdata的大小
    *****************************************************************************************/
    int SelectRead(int timeoutSec, int timeoutUsec, char *rcvData, int bufSize);
    int m_ttyfd;

};
#endif