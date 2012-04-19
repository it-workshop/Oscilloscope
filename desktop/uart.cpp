#include "uart.h"
#include <sys/timeb.h>

unsigned int mtime(bool reset = false)
{
    static timeb* tp = new timeb;
    static unsigned long long int old = 0;
    ftime(tp);
    if(reset)
    {
        old = tp->time * 1000 + tp->millitm;
        return(0);
    }
    else
    {
        return(tp->time * 1000 + tp->millitm - old);
    }
}


uart::uart()
{
    tty_fd = -1;
    device = (char*)"/dev/ttyUSB0";
    rate = 9600;
    maxwait = 10;
}

void uart::uopen()
{
    if(tty_fd == -1)
    {
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
        memset(&tio, 0, sizeof(tio));
        tio.c_iflag = 0;
        tio.c_oflag = 0;
        tio.c_cflag = CS8 | CREAD | CLOCAL;
        tio.c_lflag = 0;
        tio.c_cc[VMIN] = 1;
        tio.c_cc[VTIME] = 5;

        tty_fd=open(device, O_RDWR | O_NONBLOCK);
        cfsetospeed(&tio, rate);
        cfsetispeed(&tio, rate);
        tcsetattr(tty_fd, TCSANOW, &tio);
        flush();
    }
}

void uart::uclose()
{
    if(tty_fd != -1)
    {
        close(tty_fd);
        tty_fd = -1;
    }
}

bool uart::uread_error()
{
    return(uerror);
}

void uart::ureopen()
{
    uclose();
    uopen();
}

void uart::uread_error_reset()
{
    uerror = false;
}

void uart::set_rate(unsigned int newrate)
{
    rate=newrate;
}

void uart::set_device(char* newdevice)
{
    device = newdevice;
}

void uart::uwrite(short unsigned int buf)
{
    write(tty_fd, &buf, 1);
}

void uart::flush()
{
    static unsigned char x;
    while(read(tty_fd, &x, 1) > 0);
}

short unsigned int uart::uread()
{
    short unsigned int a = 0;
    mtime(true);
    while((read(tty_fd, &a, 1) <= 0) && ((mtime(false) < maxwait)));
    if(mtime(false) >= maxwait) uerror = true;
    return(a);
}
