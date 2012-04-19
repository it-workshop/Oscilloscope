#include <stdio.h>
#include "symbols.h"
#include "ui_symbols.h"
#include <iostream>
#include <stdlib.h>
#include <fstream>

void Symbols::write_memory()
{
    values[current][0] = ui->checkBox->isChecked();
    values[current][0] |= ui->checkBox_2->isChecked() << 1;
    values[current][0] |= ui->checkBox_3->isChecked() << 2;
    values[current][0] |= ui->checkBox_4->isChecked() << 3;
    values[current][0] |= ui->checkBox_5->isChecked() << 4;
    values[current][0] |= ui->checkBox_6->isChecked() << 5;
    values[current][0] |= ui->checkBox_7->isChecked() << 6;
    values[current][0] |= ui->checkBox_8->isChecked() << 7;

    values[current][1] = ui->checkBox_9->isChecked();
    values[current][1] |= ui->checkBox_10->isChecked() << 1;
    values[current][1] |= ui->checkBox_11->isChecked() << 2;
    values[current][1] |= ui->checkBox_12->isChecked() << 3;
    values[current][1] |= ui->checkBox_13->isChecked() << 4;
    values[current][1] |= ui->checkBox_14->isChecked() << 5;
    values[current][1] |= ui->checkBox_15->isChecked() << 6;
    values[current][1] |= ui->checkBox_16->isChecked() << 7;

    values[current][2] = ui->checkBox_17->isChecked();
    values[current][2] |= ui->checkBox_18->isChecked() << 1;
    values[current][2] |= ui->checkBox_19->isChecked() << 2;
    values[current][2] |= ui->checkBox_20->isChecked() << 3;
    values[current][2] |= ui->checkBox_21->isChecked() << 4;
    values[current][2] |= ui->checkBox_22->isChecked() << 5;
    values[current][2] |= ui->checkBox_23->isChecked() << 6;
    values[current][2] |= ui->checkBox_24->isChecked() << 7;

    values[current][3] = ui->checkBox_25->isChecked();
    values[current][3] |= ui->checkBox_26->isChecked() << 1;
    values[current][3] |= ui->checkBox_27->isChecked() << 2;
    values[current][3] |= ui->checkBox_28->isChecked() << 3;
    values[current][3] |= ui->checkBox_29->isChecked() << 4;
    values[current][3] |= ui->checkBox_30->isChecked() << 5;
    values[current][3] |= ui->checkBox_31->isChecked() << 6;
    values[current][3] |= ui->checkBox_32->isChecked() << 7;

    values[current][4] = ui->checkBox_33->isChecked();
    values[current][4] |= ui->checkBox_34->isChecked() << 1;
    values[current][4] |= ui->checkBox_35->isChecked() << 2;
    values[current][4] |= ui->checkBox_36->isChecked() << 3;
    values[current][4] |= ui->checkBox_37->isChecked() << 4;
    values[current][4] |= ui->checkBox_38->isChecked() << 5;
    values[current][4] |= ui->checkBox_39->isChecked() << 6;
    values[current][4] |= ui->checkBox_40->isChecked() << 7;
}

void Symbols::reset_table()
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

void Symbols::read_memory()
{
    ui->checkBox->setChecked((values[current][0] & 1));
    ui->checkBox_2->setChecked((values[current][0] & (1 << 1)));
    ui->checkBox_3->setChecked((values[current][0] & (1 << 2)));
    ui->checkBox_4->setChecked((values[current][0] & (1 << 3)));
    ui->checkBox_5->setChecked((values[current][0] & (1 << 4)));
    ui->checkBox_6->setChecked((values[current][0] & (1 << 5)));
    ui->checkBox_7->setChecked((values[current][0] & (1 << 6)));
    ui->checkBox_8->setChecked((values[current][0] & (1 << 7)));

    ui->checkBox_9->setChecked(values[current][1] & 1);
    ui->checkBox_10->setChecked(values[current][1] & (1 << 1));
    ui->checkBox_11->setChecked(values[current][1] & (1 << 2));
    ui->checkBox_12->setChecked(values[current][1] & (1 << 3));
    ui->checkBox_13->setChecked(values[current][1] & (1 << 4));
    ui->checkBox_14->setChecked(values[current][1] & (1 << 5));
    ui->checkBox_15->setChecked(values[current][1] & (1 << 6));
    ui->checkBox_16->setChecked(values[current][1] & (1 << 7));

    ui->checkBox_17->setChecked(values[current][2] & 1);
    ui->checkBox_18->setChecked(values[current][2] & (1 << 1));
    ui->checkBox_19->setChecked(values[current][2] & (1 << 2));
    ui->checkBox_20->setChecked(values[current][2] & (1 << 3));
    ui->checkBox_21->setChecked(values[current][2] & (1 << 4));
    ui->checkBox_22->setChecked(values[current][2] & (1 << 5));
    ui->checkBox_23->setChecked(values[current][2] & (1 << 6));
    ui->checkBox_24->setChecked(values[current][2] & (1 << 7));

    ui->checkBox_25->setChecked(values[current][3] & 1);
    ui->checkBox_26->setChecked(values[current][3] & (1 << 1));
    ui->checkBox_27->setChecked(values[current][3] & (1 << 2));
    ui->checkBox_28->setChecked(values[current][3] & (1 << 3));
    ui->checkBox_29->setChecked(values[current][3] & (1 << 4));
    ui->checkBox_30->setChecked(values[current][3] & (1 << 5));
    ui->checkBox_31->setChecked(values[current][3] & (1 << 6));
    ui->checkBox_32->setChecked(values[current][3] & (1 << 7));

    ui->checkBox_33->setChecked(values[current][4] & 1);
    ui->checkBox_34->setChecked(values[current][4] & (1 << 1));
    ui->checkBox_35->setChecked(values[current][4] & (1 << 2));
    ui->checkBox_36->setChecked(values[current][4] & (1 << 3));
    ui->checkBox_37->setChecked(values[current][4] & (1 << 4));
    ui->checkBox_38->setChecked(values[current][4] & (1 << 5));
    ui->checkBox_39->setChecked(values[current][4] & (1 << 6));
    ui->checkBox_40->setChecked(values[current][4] & (1 << 7));
}
