#ifndef UART_H
#define UART_H

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

class uart
{
private:
    struct termios tio;
    int tty_fd;
    unsigned int rate;
    char* device;
    unsigned int maxwait;
    bool uerror;
public:
    uart();
    bool uread_error();
    void uread_error_reset();
    void uopen();
    void uclose();
    void ureopen();
    void set_rate(unsigned int);
    void set_device(char*);
    void uwrite(short unsigned int);
    short unsigned int uread();
    void flush();
};

#endif // UART_H
