#include <stdio.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::update() {
    static short unsigned int s1,s2,s3,s4,s5;
    s1=ui->checkBox->isChecked();
    s1|=ui->checkBox_2->isChecked()<<1;
    s1|=ui->checkBox_3->isChecked()<<2;
    s1|=ui->checkBox_4->isChecked()<<3;
    s1|=ui->checkBox_5->isChecked()<<4;
    s1|=ui->checkBox_6->isChecked()<<5;
    s1|=ui->checkBox_7->isChecked()<<6;
    s1|=ui->checkBox_8->isChecked()<<7;

    s2=ui->checkBox_9->isChecked();
    s2|=ui->checkBox_10->isChecked()<<1;
    s2|=ui->checkBox_11->isChecked()<<2;
    s2|=ui->checkBox_12->isChecked()<<3;
    s2|=ui->checkBox_13->isChecked()<<4;
    s2|=ui->checkBox_14->isChecked()<<5;
    s2|=ui->checkBox_15->isChecked()<<6;
    s2|=ui->checkBox_16->isChecked()<<7;

    s3=ui->checkBox_17->isChecked();
    s3|=ui->checkBox_18->isChecked()<<1;
    s3|=ui->checkBox_19->isChecked()<<2;
    s3|=ui->checkBox_20->isChecked()<<3;
    s3|=ui->checkBox_21->isChecked()<<4;
    s3|=ui->checkBox_22->isChecked()<<5;
    s3|=ui->checkBox_23->isChecked()<<6;
    s3|=ui->checkBox_24->isChecked()<<7;

    s4=ui->checkBox_25->isChecked();
    s4|=ui->checkBox_26->isChecked()<<1;
    s4|=ui->checkBox_27->isChecked()<<2;
    s4|=ui->checkBox_28->isChecked()<<3;
    s4|=ui->checkBox_29->isChecked()<<4;
    s4|=ui->checkBox_30->isChecked()<<5;
    s4|=ui->checkBox_31->isChecked()<<6;
    s4|=ui->checkBox_32->isChecked()<<7;

    s5=ui->checkBox_33->isChecked();
    s5|=ui->checkBox_34->isChecked()<<1;
    s5|=ui->checkBox_35->isChecked()<<2;
    s5|=ui->checkBox_36->isChecked()<<3;
    s5|=ui->checkBox_37->isChecked()<<4;
    s5|=ui->checkBox_38->isChecked()<<5;
    s5|=ui->checkBox_39->isChecked()<<6;
    s5|=ui->checkBox_40->isChecked()<<7;
    sprintf(s,"else if(sym=='') {\n\tbuf[0]=%u;\n\tbuf[1]=%u;\n\tbuf[2]=%u;\n\tbuf[3]=%u;\n\tbuf[4]=%u;\n}",s1,s2,s3,s4,s5);
    ui->plainTextEdit->clear();
    ui->plainTextEdit->insertPlainText(QString(s));
}

void MainWindow::on_pushButton_clicked()
{
    update();
}

void MainWindow::on_pushButton_2_clicked()
{
 ui->checkBox->setChecked(0);
 ui->checkBox_2->setChecked(0);
 ui->checkBox_3->setChecked(0);
 ui->checkBox_4->setChecked(0);
 ui->checkBox_5->setChecked(0);
 ui->checkBox_6->setChecked(0);
 ui->checkBox_7->setChecked(0);
 ui->checkBox_8->setChecked(0);
 ui->checkBox_9->setChecked(0);
 ui->checkBox_10->setChecked(0);
 ui->checkBox_11->setChecked(0);
 ui->checkBox_12->setChecked(0);
 ui->checkBox_13->setChecked(0);
 ui->checkBox_14->setChecked(0);
 ui->checkBox_15->setChecked(0);
 ui->checkBox_16->setChecked(0);
 ui->checkBox_17->setChecked(0);
 ui->checkBox_18->setChecked(0);
 ui->checkBox_19->setChecked(0);
 ui->checkBox_20->setChecked(0);
 ui->checkBox_21->setChecked(0);
 ui->checkBox_22->setChecked(0);
 ui->checkBox_23->setChecked(0);
 ui->checkBox_24->setChecked(0);
 ui->checkBox_25->setChecked(0);
 ui->checkBox_26->setChecked(0);
 ui->checkBox_27->setChecked(0);
 ui->checkBox_28->setChecked(0);
 ui->checkBox_29->setChecked(0);
 ui->checkBox_30->setChecked(0);
 ui->checkBox_31->setChecked(0);
 ui->checkBox_32->setChecked(0);
 ui->checkBox_33->setChecked(0);
 ui->checkBox_34->setChecked(0);
 ui->checkBox_35->setChecked(0);
 ui->checkBox_36->setChecked(0);
 ui->checkBox_37->setChecked(0);
 ui->checkBox_38->setChecked(0);
 ui->checkBox_39->setChecked(0);
 ui->checkBox_40->setChecked(0);
}
