#ifndef DESKTOP_OSC_H
#define DESKTOP_OSC_H

#include <QMainWindow>
#include <QTimer>
#include "uart.h"

#define PLOT_SIZE 500
enum {MODE_UART_BUF, MODE_UART};

namespace Ui {
    class desktop_osc;
}

class desktop_osc : public QMainWindow {
    Q_OBJECT
public:
    desktop_osc(QWidget *parent = 0);
    ~desktop_osc();

protected:
    void changeEvent(QEvent *e);

private:
    QTimer timer;

    int mk_frequency;
    unsigned int ALL_N;
    unsigned int trigger_reset_max;

    unsigned int msecs;
    unsigned int frequency;
    unsigned int current;
    unsigned int position;
    unsigned short int mode;
    bool answered;
    uart uartobj;
    double d_x[PLOT_SIZE];
    double d_y[PLOT_SIZE];
    Ui::desktop_osc *ui;

    void init_graph();
    void to_graph(double,bool replot=true);
    //void clear_graph();

public:
    void device_return_control();
    void device_mode_buffer();
    void device_mode_non_buffered();
    void device_update();
    void parse_settings();

private slots:
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void update();
    void on_frequency_valueChanged();
    void on_frequency_slider_sliderMoved(int position);
    void on_trigger_reset_valueChanged();
    void on_trigger_check_valueChanged();
};

#endif // DESKTOP_OSC_H
