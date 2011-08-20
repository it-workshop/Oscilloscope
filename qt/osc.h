#ifndef OSC_H
#define OSC_H
#include <QTimer>
#include <QMainWindow>
const int PLOT_SIZE = 501;      // 0 to 500
namespace Ui {
    class Osc;
}

class Osc : public QMainWindow {
    Q_OBJECT
public:
    void to_graph(double);
    Osc(QWidget *parent = 0);
    ~Osc();

protected:
    void changeEvent(QEvent *e);

private:
    QTimer timer;
    void init_graph();
    Ui::Osc *ui;
    double d_x[PLOT_SIZE];
    double d_y[PLOT_SIZE];
    double d_z[PLOT_SIZE];

private slots:
    void on_checkBox_stateChanged(int );
    void on_spinBox_valueChanged(int );
    void on_spinBox_2_valueChanged(int );
    void update();
};

#endif // OSC_H
