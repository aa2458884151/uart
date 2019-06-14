/*******************************************************************************
Copyright: 2019, Innovem Tech. Co., Ltd.
FileName: LTEttyUart.cpp
*******************************************************************************/
#include "LTEttyUart.h"
LTEttyUart::LTEttyUart()
{
    m_ttyfd = -1;
}
LTEttyUart::~LTEttyUart()
{
    close(m_ttyfd);
}
int LTEttyUart::LTEttyUartInit(int speed, int databits, int stopbits, int parity , const char *path)
{
    m_ttyfd = open(path, O_RDWR | O_NOCTTY);

    if(-1 == m_ttyfd) {
        /* 不能打开串口一*/
        perror(" 提示错误！");
    }
    printf("%d\n", m_ttyfd);
    struct termios Opt;
    int status;
    if(tcgetattr(m_ttyfd, &Opt) != 0) {
        perror("SetupSerial 123");
    }
    tcflush(m_ttyfd, TCIOFLUSH);                //清空原始缓冲区  因为接下来要设置波特率 缓冲区内有可能存储错误波特率的数据
    cfsetispeed(&Opt, speed);              //设置发送缓冲区的波特率
    cfsetospeed(&Opt, speed);              //设置接受缓冲区的波特率

    Opt.c_cflag &= ~CSIZE;
    Opt.c_cflag |= CREAD;
    Opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //Opt.c_lflag = 0;
    switch(databits) {
        /*设置数据位数*/
    case 7:
        Opt.c_cflag |= CS7;
        break;
    case 8:
        Opt.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr, "Unsupported data size\n");
        return -1;
    }
    /*设置校验位*/
    switch(parity) {
    case 'n':
    case 'N':
        Opt.c_cflag &= ~PARENB; /* PARENB：产生奇偶位，执行奇偶校验 */
        Opt.c_iflag &= ~INPCK;  /* INPCK：使奇偶校验起作用 */
        break;
    case 'o':
    case 'O':
        Opt.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
        Opt.c_iflag |= INPCK;             /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        Opt.c_cflag |= PARENB;  /* Enable parity */
        Opt.c_cflag &= ~PARODD; /* 转换为偶效验*/
        Opt.c_iflag |= INPCK;   /* Disnable parity checking */
        break;
    case 'S':
    case 's':
        Opt.c_cflag &= ~PARENB;
        Opt.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported parity\n");
        return -1;
    }
    /* 设置停止位*/
    switch(stopbits) {
    case 1:
        Opt.c_cflag &= ~CSTOPB;
        break;
    case 2:
        Opt.c_cflag |= CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported stop bits\n");
        return -1;
    }

    Opt.c_cc[VTIME] = 0; /* 设置超时15 seconds*/
    Opt.c_cc[VMIN] = 1;  /* Update the Opt and do it NOW */
    Opt.c_iflag = 0;
    Opt.c_oflag = 0;

    status = tcsetattr(m_ttyfd, TCSANOW, &Opt); //将串口参数设置回去
    if(status != 0) {
        perror("设置失败");
        return -1;
    }
    return 0;
}
int LTEttyUart::SelectRead(int timeoutSec, int timeoutUsec, char *rcvData, int bufSize)
{
    struct timeval tv;
    fd_set rfds;
    int retval;
    tv.tv_sec = timeoutSec;
    tv.tv_usec = timeoutUsec;
    FD_ZERO(&rfds);
    FD_SET(m_ttyfd, &rfds);
    retval = select(m_ttyfd + 1, &rfds, NULL, NULL, &tv);
    if(retval == -1) {
        printf("select error\r\n");
        return -1;
    } else if(retval) {
        return read(m_ttyfd, rcvData, bufSize);
    } else { //超时
        return 0;
    }
}
int LTEttyUart::ReadOneData(char *rcvbuf, int sec, int usec)
{
    if(m_ttyfd == -1) {
        perror("you should init LTEttyUart before need it!\n");
        return 0;
    }
    int bufHead = 0, bufsize = 0, ret = 0, rcvstat = 0;
    int rcvsize = 0;
    char cur_buf[TTYLENGTH] = {0};
    while(1) { //筛选包头
        if(SelectRead(sec, usec, (char *)&bufHead, 1) > 0) {
#ifdef DEBUGTTY
            printf("buf head is %d\n", bufHead);
#endif
            if(bufHead != 0xff) {
                continue;
            } else {
                break;
            }
        } else {
            printf("接收失败！！\n");
            continue;
        }
    }
    memset(rcvbuf, 0, TTYLENGTH);
    memset(cur_buf, 0, TTYLENGTH);
    if(SelectRead(sec, usec, (char *)&bufsize, 1) > 0) {
        rcvsize = bufsize;
#ifdef DEBUGTTY
        printf("%d\n", bufsize);
#endif
        while(1) {
            ret = SelectRead(sec, usec, cur_buf, bufsize);
            strcat(rcvbuf, cur_buf);
            if((ret != bufsize) && (ret != 0)) {

                memset(cur_buf, 0, TTYLENGTH);
                bufsize -= ret;
            } else if(ret == 0) {
                rcvstat = 0;
                break;
            } else {
                rcvstat = 1;
                break;
            }
        }
        if(rcvstat == 0) {
            //认为断开链接了   需要清空接收缓冲区 等待下一次接收
            tcflush(m_ttyfd, TCIFLUSH);
            printf("断开链接\n");
            return 0;
        } else {
#ifdef DEBUGTTY
            printf("rcv count is %d\n", rcvsize);
#endif
            return rcvsize;
        }
    }
	return 0;
}
int LTEttyUart::SendOneData(char *buf, int len)
{
    if(m_ttyfd == -1) {
        perror("you should init LTEttyUart before need it!\n");
        return 0;
    }
#ifdef DEBUGTTY
    if(len > TTYLENGTH || len <= 0) {
        return -1;
    } else {
        for(int i = 0; i < len; i++) {
            if(buf[i] == 0xff) {
                return -2;
            }
        }
    }
#endif
    char sendbuf[TTYLENGTH];
    memset(sendbuf, 0, TTYLENGTH);
    sendbuf[0] = 0xff;
    sendbuf[1] = len;
    memcpy(&sendbuf[2], buf, len);
    return write(m_ttyfd, sendbuf, len + 2);
}