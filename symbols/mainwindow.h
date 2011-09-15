#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <string>

using std::string;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    typedef unsigned char value_t;
    Ui::MainWindow *ui;

    unsigned int SYM,VAL;
    string filename;
    value_t** values;
    short unsigned int current;

    void set_file_pos();

    void reset_table();
    void read_file();

    void write_file();

    void read_memory();
    void write_memory();

private slots:
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_3_clicked();
    void on_lineEdit_editingFinished();
    void on_spinBox_valueChanged(int );
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
