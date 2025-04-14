#ifndef __UARTTOOL__H
#define __UARTTOOL__H


int mySerialOpen(const char *device, const int baud);

void serialSendMsg(const int fd, const unsigned char *s, int len);


int serialRecvMsg(const int fd, unsigned char *buf);

#define SERIAL_DEV "/dev/ttyS5"
#define BAUD 115200
#endif