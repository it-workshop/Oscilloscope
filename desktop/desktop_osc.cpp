#include "desktop_osc.h"
#include "ui_desktop_osc.h"
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>

#include <stdio.h>
enum{MODE_UART_BUF,MODE_UART};
void desktop_osc::init_graph() {
    timer.stop();
    if(msecs) {
        timer.setInterval(msecs);
        timer.start();
        for(int i=0;i<PLOT_SIZE;i++) {
            d_x[i]=(msecs?msecs:1)*i;
            d_y[i]=0;
        }
    }
    ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, PLOT_SIZE*(msecs?msecs:1));
    ui->qwtPlot->setAxisScale(QwtPlot::yLeft, 0, 5);
}

desktop_osc::desktop_osc(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::desktop_osc)
{
    msecs=0;
    ALL_N=128;
    current=0;
    mode=MODE_UART_BUF;
    ui->setupUi(this);
    uartobj.set_device("/dev/ttyUSB0");
    uartobj.set_rate(B57600);
    uartobj.uopen();
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    QwtPainter::setDeviceClipping(false);

    ui->qwtPlot->canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    ui->qwtPlot->canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);

#if QT_VERSION >= 0x040000
#ifdef Q_WS_X11
    ui->qwtPlot->canvas()->setAttribute(Qt::WA_PaintOnScreen, true);
#endif
#endif
    ui->qwtPlot->insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
    QwtPlotCurve *cRight = new QwtPlotCurve("0");
    cRight->attach(ui->qwtPlot);
    cRight->setPen(QPen(Qt::red));
    cRight->setRawData(d_x, d_y, PLOT_SIZE);

    ui->qwtPlot->setAxisTitle(QwtPlot::xBottom, "T/ms");
    ui->qwtPlot->setAxisTitle(QwtPlot::yLeft, "U/V");
    init_graph();
    msecs=1;
}

desktop_osc::~desktop_osc() {
    uartobj.uclose();
    delete ui;
}

void desktop_osc::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void desktop_osc::to_graph(double y) {
    int i;
    for(i=PLOT_SIZE-1;i>0;i--)
        d_y[i]=d_y[i-1];
    d_y[0]=y;
    ui->qwtPlot->replot();
}


void desktop_osc::on_pushButton_clicked() {
    mode=MODE_UART;
    msecs=ui->spinBox->value();
    init_graph();
}

void desktop_osc::update() {
    if((current<ALL_N)||(mode==MODE_UART)) {
        uartobj.uwrite('g');
        unsigned int r=uartobj.uread()|(uartobj.uread()<<8);
        to_graph(double(r)/13107);
        current++;
    }
    else {
        current=0;
        timer.stop();
    }
}

void desktop_osc::on_pushButton_2_clicked() {
    timer.stop();
}

void desktop_osc::on_pushButton_3_clicked() {
    uartobj.uwrite('c');
    timer.stop();
}

void desktop_osc::on_pushButton_4_clicked() {
    mode=MODE_UART_BUF;
    timer.stop();
    period=ui->spinBox_2->value();
    uartobj.uwrite('b');
    uartobj.uwrite(period^(0xff<<8));
    uartobj.uwrite(period>>8);
    current=0;
    msecs=1;
    init_graph();
}
