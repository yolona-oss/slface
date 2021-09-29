#include "initaccountsdialog.h"
#include "ui_initaccountsdialog.h"
#include <window.h>
#include <QFileDialog>
#include <QMessageBox>

#include <iostream>

InitAccountsDialog::InitAccountsDialog(QWidget *parent) :
    QDialog(parent),
		ui(new Ui::InitAccountsDialog),
		__fmt("ukep")
{
    ui->setupUi(this);
}

InitAccountsDialog::~InitAccountsDialog()
{
    delete ui;
}

QString          InitAccountsDialog::getFileToReadFrom(void) { return __filename; }
SL::ReadAccsFlag InitAccountsDialog::getRAF(void) { return __rafFlags; }
std::string      InitAccountsDialog::getFormat(void) { return __fmt; }

void
InitAccountsDialog::on_pushButton_choose_file_clicked()
{
    ui->pushButton_choose_file->setStyleSheet("");

    __filename = QFileDialog::getOpenFileName(this, tr("Choose"), "", "All files (*);;Text file (*.txt)");
}

char
InitAccountsDialog::ind2fmt(FMT_FIELDS val)
{
    switch (val) {
        case USERNAME_FIELD:
            return 'u';
            break;

        case KEY_FIELD:
            return 'k';
            break;

        case EMAIL_FIELD:
            return 'e';
            break;

        case PASS_FIELD:
            return 'p';
            break;
        default:
            return ' ';
            break;
    }
}

void InitAccountsDialog::on_buttonBox_accepted()
{
    if (ui->checkBox_skip_empty_lines->isChecked()) {
        __rafFlags |= SL::RAF_SKIPNL;
    }
    if (ui->radioButton_column->isChecked()) {
        __rafFlags |= SL::RAF_COLUMN;
    } else {
        __rafFlags |= SL::RAF_ROW;
    }

    if (!__filename.isEmpty()) {
        __fmt.erase(std::remove_if(__fmt.begin(), __fmt.end(), isspace), __fmt.end()); //deleting spaces
		this->accept();
    } else {
        ui->pushButton_choose_file->setStyleSheet("background-color: red;");
        QMessageBox::warning(this, "File not selected", "File to read from not selected!");
    }
}

void InitAccountsDialog::on_buttonBox_rejected()
{
    this->reject();
}

void InitAccountsDialog::on_comboBox_field1_activated(int index)
{
    __fmt[0] = ind2fmt(static_cast<FMT_FIELDS>(index));
}

void InitAccountsDialog::on_comboBox_field2_activated(int index)
{
    __fmt[1] = ind2fmt(static_cast<FMT_FIELDS>(index));
}


void InitAccountsDialog::on_comboBox_field3_activated(int index)
{
    __fmt[2] = ind2fmt(static_cast<FMT_FIELDS>(index));
}


void InitAccountsDialog::on_comboBox_field4_activated(int index)
{
    __fmt[3] = ind2fmt(static_cast<FMT_FIELDS>(index));
}
