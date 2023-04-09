#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>   /* 文件控制定义*/
#include <termios.h> /* PPSIX 终端控制定义*/
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int set_serial_uart(int ser_fd, int nSpeed, int nBits, char nEvent, int nStop);

#ifdef __cplusplus
}
#endif

#endif
