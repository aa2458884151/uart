#include "LTEttyUart.h"
#define DEBUF
int testarm()
{
    char rcvbuf[TTYLENGTH] = {0};
    LTEttyUart *ttyuart = new LTEttyUart();
    ttyuart->LTEttyUartInit(B115200, 8, 1, 'N', "/dev/ttyS1");
    while(1) {
		memset(rcvbuf,0,TTYLENGTH);
        if(ttyuart->ReadOneData(rcvbuf, 2, 0) > 0) {
            printf("%s\n", rcvbuf);
        }
    }
}

int testpc()
{
    char sndbuf[TTYLENGTH] = {0};
	int ret=0;
    LTEttyUart *ttyuart = new LTEttyUart();
    ttyuart->LTEttyUartInit(B115200, 8, 1, 'N', "/dev/ttyUSB0");
    while(1) {
        ret=scanf("%s", sndbuf);
		printf("%d\n",ret);
        ttyuart->SendOneData(sndbuf, strlen(sndbuf));
    }
}
int main()
{
    unsigned char sndbuf[TTYLENGTH] = {0xBA ,0xCE ,0x10 ,0x00 ,0x06 ,0x03 ,0xE8 ,0x03 ,0x00 ,0x00 ,0x14 ,0x00 ,0x00 ,0x00 ,0x02 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x0E ,0x04 ,0x06 ,0x03};
    LTEttyUart *ttyuart = new LTEttyUart();
    ttyuart->LTEttyUartInit(B115200, 8, 1, 'N', "/dev/ttyS1");
    //memset(sndbuf,0xff,200);
    int i=0;
	i=ttyuart->SendOneData((char *)sndbuf, strlen((char *)sndbuf));
	printf("length = %d\n",i);
    testarm();
    //testpc();
}