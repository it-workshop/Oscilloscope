#include <stdio.h>
#include "symbols.h"
#include "ui_symbols.h"
#include <iostream>
#include <stdlib.h>
#include <fstream>

using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::stringstream;

Symbols::Symbols(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::Symbols)
{

    ui->setupUi(this);
    filename = "./eeprom_fonts";
    VAL = 128;
    SYM = 5;
    values = new value_t*[VAL];
    current = 0;
    short unsigned int i;
    for(i = 0; i < VAL; i++)
        values[i] = new value_t[SYM];
    read_file();
}

Symbols::~Symbols()
{
    delete ui;
    unsigned int i;
    for(i = 0; i < VAL; i++)
        delete[] values[i];
    delete[] values;
}

void Symbols::changeEvent(QEvent *e)
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



void Symbols::on_pushButton_clicked()
{
    write_memory();
    write_file();
}

void Symbols::on_pushButton_2_clicked()
{
    reset_table();
}


void Symbols::read_file()
{
    FILE* file = fopen(filename.c_str(), "r");

    unsigned int i, si;    
    for(i = 0; i < VAL; i++)
    {
        for(si = 0; si < SYM; si++)
        {
            values[i][si] = fgetc(file);
        }
    }
    
    fclose(file);
}

void Symbols::write_file()
{
    FILE* file = fopen(filename.c_str(), "w");
    
    unsigned int i, si;
    for(i = 0; i < VAL; i++)
        for(si = 0; si < SYM; si++)
            fputc(values[i][si], file);
    
    fclose(file);
}

void Symbols::on_spinBox_valueChanged(int x)
{
    write_memory();

    char* buf = new char[10];
    sprintf(buf,"%c",x);
    ui->lineEdit->setText(QString(buf));
    current = x;
    delete buf;

    read_memory();
}

void Symbols::on_lineEdit_editingFinished()
{
    write_memory();

    if(ui->lineEdit->text().toAscii().data()[0])
    {
        ui->spinBox->setValue(ui->lineEdit->text().toAscii().data()[0]);
        reset_table();
        current = ui->spinBox->value();

        read_memory();
    }
    else ui->spinBox->setValue(0);

}

void Symbols::on_pushButton_3_clicked()
{
    read_file();
}
