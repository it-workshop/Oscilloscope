#ifndef UART_H
#define UART_H

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

class uart {
private:
    struct termios tio;
    int tty_fd;
    unsigned int rate;
    char* device;
    int maxwait;
    bool writing;
public:
    uart();
    bool active();
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
