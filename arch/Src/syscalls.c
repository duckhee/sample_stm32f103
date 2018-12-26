#include "Hw_Config.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "stm32f10x.h"

//retargetting _write
__attribute__ ((used)) int _write(int fd, char *ptr, int len)
{
    size_t i;
    for(i = 0; i < len; i++)
    {
        USART_SendData(USART1, ptr[i]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
    return len;
}

//retargetting _read
/*
__attribute__ ((used)) int _read(char *buf, int len)
{
    size_t i;
    for(i = 0; i < len; ++i)
    {
        buf[i] = USART_ReceiveData(USART1);
        while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    }
    return i;
}
*/
/*

//retargetting _read
__attribute__ ((used)) int _read(int __fd, void *__buf, size_t _nbyte)
{
    return 0;
}

*/


/*
//need to testing 
__attribute__ ((used)) int _write(int fd, char *ptr, int len)
{
    size_t i;
    for(i = 0; i < len; i++)
    {
        fputc(USART1, (uint8_t *)&ch);
    }
    return len;
}

*/