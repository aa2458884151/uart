#include "LTEttyUart.h"
#define DEBUF
int testarm()
{
    char rcvbuf[TTYLENGTH] = {0};
    LTEttyUart *ttyuart = new LTEttyUart();
    ttyuart->LTEttyUartInit(B115200, 8, 1, 'N', "/dev/ttyPS0");
    while(1) {
        if(ttyuart->ReadOneData(rcvbuf, 2, 0) > 0) {
            printf("%s\n", rcvbuf);
        }
    }
}
int testpc()
{
    char sndbuf[TTYLENGTH] = {0};
    LTEttyUart *ttyuart = new LTEttyUart();
    ttyuart->LTEttyUartInit(B115200, 8, 1, 'N', "/dev/ttyUSB0");
    while(1) {
        scanf("%s", sndbuf);
        ttyuart->SendOneData(sndbuf, strlen(sndbuf));
    }
}
int main()
{
    testarm();
    //testpc();
}