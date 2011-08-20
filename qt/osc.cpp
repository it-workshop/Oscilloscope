#include <math.h>
#include <stdlib.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "osc.h"
#include "ui_osc.h"
enum{ADC_NORMAL,ADC_CLOSE,ADC_SEND};
enum{ADC_BUFFERED,ADC_NONBUFFERED};
unsigned int msecs=0,show_errors=0,adc_mode=ADC_NONBUFFERED;
const double TwoPi = M_PI*2;
void Osc::init_graph() {
    timer.stop();
    if(msecs!=0) {
        timer.setInterval(msecs);
        timer.start();
        for (int i = 0; i< PLOT_SIZE; i++) {
            d_x[i] = (msecs?msecs:1)*i;
            d_y[i] = 0;
            d_z[i] = 0;
        }
    }
    ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, PLOT_SIZE*(msecs?msecs:1));
    ui->qwtPlot->setAxisTitle(QwtPlot::xBottom, "T/ms");
    ui->qwtPlot->setAxisScale(QwtPlot::yLeft, 0, 5);
    ui->qwtPlot->setAxisTitle(QwtPlot::yLeft, "U/V");
}

int adc(int todo=ADC_NORMAL,char arg=0) {
    static const int IMAX=2000;
    static struct termios tio;
    static int tty_fd=-1;
    unsigned char c=0;
    bool changed=false;
    static unsigned int i=0;
    if((todo==ADC_CLOSE)&&tty_fd!=-1) {
        close(tty_fd);
        tty_fd=-1;
        return(0);
    }
    if(tty_fd==-1) {
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
        memset(&tio,0,sizeof(tio));
        tio.c_iflag=0;
        tio.c_oflag=0;
        tio.c_cflag=CS8|CREAD|CLOCAL;
        tio.c_lflag=0;
        tio.c_cc[VMIN]=1;
        tio.c_cc[VTIME]=5;

        tty_fd=open("/dev/ttyUSB0", O_RDWR | O_NONBLOCK);
        cfsetospeed(&tio,B9600);
        cfsetispeed(&tio,B9600);
        tcsetattr(tty_fd,TCSANOW,&tio);
    }
    if(todo==ADC_SEND) {
        write(tty_fd,&arg,1);
        return(0);
    }
    write(tty_fd,"g",1);
    i=0;
    while(!changed&&i<IMAX) {
        changed=(read(tty_fd,&c,1)>0);
        i++;
    }
    if(i>=IMAX) {
        adc(ADC_CLOSE);
        return(-1);
    }
    return(c);
}

Osc::Osc(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::Osc)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    ui->setupUi(this);
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
    init_graph();
}


void Osc::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

double sin_value() {
    static unsigned int i=0;
    static const double step=M_PI/100;
    if(step*i>=2*M_PI) {
        i=0;
        return(0);
    }
    i++;
    return(sin(i*step));
}

void Osc::to_graph(double y) {
    int i;
    for (i=PLOT_SIZE-1;i>0;i--)
        d_y[i]=d_y[i-1];
    d_y[0]=y;
    ui->qwtPlot->replot();
}

Osc::~Osc() {
    delete ui;
    adc(ADC_CLOSE);
}
double interval(double value,double* res) {
    static bool min_reached=false,max_reached=false;
    static int i=0,si=0;
    static double min=10000000,max=-1,delta=0.05;
    if(min>value) min=value;
    if(max<value) max=value;
    if(*res&&si>=3*(*res)) {
        si=0;
        min=100000000;
        max=-1;
    }
    if(((value-min)<delta*(max-min))&&max_reached) min_reached=true;
    if((max-value)<delta*(max-min)) {
        if(!max_reached) {
            max_reached=true;
            i=0;
        }
        else if(min_reached) {
            if((max-min)>5*delta) *res=i+1;
            i=0;
            min_reached=false;
            max_reached=false;
        }
    }
    if(max_reached) i++;
    si++;

}

void Osc::update() {
    static double interval1=0;
    static char* a=new char[10];
    double value=(1.0*adc())/51.0;
    //double value=(sin_value()+1)/2.*5;
    if(!(show_errors)&&value<0) return;
    interval(value,&interval1);
    sprintf(a,"%f Hz",1000/(interval1*msecs));
    ui->label->setText(QString(a));
    to_graph(value);
}

void Osc::on_spinBox_2_valueChanged(int x) {
/*    if(x>1) {
        adc_mode=ADC_BUFFERED;
        adc(ADC_SEND,'b');
        adc(ADC_SEND,char(x));
    }
    else {
        adc_mode=ADC_NONBUFFERED;
        adc(ADC_SEND,'n');
    }*/

}

void Osc::on_spinBox_valueChanged(int x) {
    msecs=ui->spinBox->value();
    init_graph();
}

void Osc::on_checkBox_stateChanged(int x) {
    show_errors=x;
}
