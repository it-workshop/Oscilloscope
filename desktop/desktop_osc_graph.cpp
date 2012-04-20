#include "desktop_osc.h"
#include "ui_desktop_osc.h"

void desktop_osc::init_graph()
{
    timer.stop();
    float one;
    unsigned int max;
    const float k = 1000000.;

    if(mode == MODE_UART)
    {
        ui->qwtPlot->setAxisTitle(QwtPlot::xBottom, "T [ms]");
        one = msecs;
        max = PLOT_SIZE;
    }
    else
    {
        max = ALL_N;
        one = k / frequency;
        ui->qwtPlot->setAxisTitle(QwtPlot::xBottom, "T [us]");
    }

    if(msecs)
    {
        timer.setInterval(msecs);
        timer.start();
        for(int i = 0; i < PLOT_SIZE; i++)
        {
            d_x[i] = one * i;
            d_y[i] = 0;
        }
    }

    position = 0;
    ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, max * one);
    ui->qwtPlot->setAxisScale(QwtPlot::yLeft, 0, 5);
}

/*void desktop_osc::clear_graph()
{
    position = 0;
    unsigned int i;
    for(i = PLOT_SIZE - 1; i > 0; i--)
        d_y[i] = 0;
}*/

void desktop_osc::to_graph(double y, bool replot)
{
    d_y[position++] = y;
    if(position == PLOT_SIZE) position = 0;
    if(replot) ui->qwtPlot->replot();
}

