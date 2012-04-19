#include "desktop_osc.h"
#include "ui_desktop_osc.h"

void desktop_osc::device_mode_buffer()
{
    mode = MODE_UART_BUF;
    timer.stop();
    period = mk_frequency / ui->spinBox_2->value();
    uartobj.flush();

    uartobj.uwrite('i'); //input
    uartobj.uwrite(ui->input->currentIndex());

    uartobj.uwrite('t'); //trigger_check
    uartobj.uwrite(ui->trigger_check->value());

    uartobj.uwrite('r'); //trigger_reset
    uartobj.uwrite(ui->trigger_reset->value());

    uartobj.uwrite('e'); //trigger_error
    uartobj.uwrite(ui->trigger_error->value());

    uartobj.uwrite('b');
    uartobj.uwrite(period & 0xff);
    uartobj.uwrite(period >> 8);

    current = 0;
    msecs = 1;
    answered = true;
    init_graph();
}

void desktop_osc::device_return_control()
{
    uartobj.uwrite('c');
    timer.stop();
}


void desktop_osc::device_update()
{
    const unsigned int s = 6553;
    if(answered || mode == MODE_UART)
    {
        answered = false;
        uartobj.uwrite('g');
        uartobj.uread_error_reset();
        unsigned int r = uartobj.uread() | (uartobj.uread() << 8);
        //cerr << "update " << r << endl;
        to_graph(double(r) / s, mode == MODE_UART || current == ALL_N);
        if(current == ALL_N)
        {
            current = 0;
            if(mode == MODE_UART_BUF)
            {
                position = 0;
                uartobj.flush();
                uartobj.uwrite('b');
                uartobj.uwrite(period & 0xff);
                uartobj.uwrite(period >> 8);
                ui->qwtPlot->replot();
                //clear_graph();
            }
        }
        else current++;
        answered = true;
    }
}

void desktop_osc::device_mode_non_buffered()
{
    timer.stop();
    mode = MODE_UART;
    uartobj.uwrite('n'); //non-buffered

    uartobj.uwrite('i'); //input
    uartobj.uwrite(ui->input->currentIndex());

    msecs = ui->spinBox->value();
    answered = true;
    init_graph();
}
