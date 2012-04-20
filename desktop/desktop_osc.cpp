#include "desktop_osc.h"
#include "ui_desktop_osc.h"
#include <qwt/qwt_painter.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_scale_widget.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_math.h>
#include <iostream>

using std::cerr;
using std::endl;

desktop_osc::desktop_osc(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::desktop_osc)
{
    mk_frequency = 20000000;
    msecs = 0;
    answered = true;
    ALL_N = 128;
    current = 0;
    mode = MODE_UART_BUF;
    trigger_reset_max = 20;

    uartobj.set_device((char*)"/dev/ttyUSB0");
    uartobj.set_rate(B57600);
    uartobj.uopen();

    ui->setupUi(this);


    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    //QwtPainter::setDeviceClipping(false);
    //QwtPainter::

    //ui->qwtPlot->canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    //ui->qwtPlot->canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);


#if QT_VERSION >= 0x040000
#ifdef Q_WS_X11
    ui->qwtPlot->canvas()->setAttribute(Qt::WA_PaintOnScreen, true);
#endif
#endif
    ui->qwtPlot->insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
    QwtPlotCurve *cRight = new QwtPlotCurve("signal");
    cRight->attach(ui->qwtPlot);
    cRight->setPen(QPen(Qt::red));
    cRight->setRawSamples(d_x, d_y, PLOT_SIZE);

    ui->qwtPlot->setAxisTitle(QwtPlot::xBottom, "T [ms]");
    ui->qwtPlot->setAxisTitle(QwtPlot::yLeft, "U [V]");
    init_graph();
    msecs = 1;
}

desktop_osc::~desktop_osc()
{
    uartobj.uclose();
    delete ui;
}

void desktop_osc::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void desktop_osc::update()
{
    device_update();
}

void desktop_osc::parse_settings()
{
    uartobj.uclose();
    uartobj.set_device((char*)ui->device->text().toStdString().c_str());
    uartobj.uopen();

    frequency = ui->frequency->value();
}

//stop
void desktop_osc::on_pushButton_2_clicked()
{
    timer.stop();
}

//return control
void desktop_osc::on_pushButton_3_clicked()
{
    parse_settings();
    device_return_control();
}

//start_buffered
void desktop_osc::on_pushButton_4_clicked()
{
    parse_settings();
    device_mode_buffer();
}

//start_non_buffered
void desktop_osc::on_pushButton_clicked()
{
    parse_settings();
    device_mode_non_buffered();
}

void desktop_osc::on_frequency_valueChanged()
{
    ui->frequency_slider->setValue(ui->frequency->value());
    frequency = ui->frequency->value();
}

void desktop_osc::on_frequency_slider_sliderMoved(int position)
{
    ui->frequency->setValue(position);
    frequency = ui->frequency->value();
}

void desktop_osc::on_trigger_reset_valueChanged()
{
    if((unsigned int) ui->trigger_reset->value() == trigger_reset_max)
        ui->trigger_reset->setSuffix(" (never)");
    else ui->trigger_reset->setSuffix("");
}

void desktop_osc::on_trigger_check_valueChanged()
{
    if(ui->trigger_check->value() == 0) ui->trigger_check->setSuffix(" (off)");
    else ui->trigger_check->setSuffix("");
}
