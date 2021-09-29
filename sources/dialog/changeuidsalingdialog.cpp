#include "changeuidsalingdialog.h"
#include "ui_changeuidsalingdialog.h"

#include <QMessageBox>

ChangeUIDsAlingDialog::ChangeUIDsAlingDialog(SL::Stor& stor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeUIDsAlingDialog)
{
    ui->setupUi(this);

	__idalignDefault = (*stor.begin())->id();

	ui->spinBox->setPrefix("New UID alingment: ");
	ui->spinBox->setValue(__idalignDefault);
}

ChangeUIDsAlingDialog::~ChangeUIDsAlingDialog()
{
    delete ui;
}

long
ChangeUIDsAlingDialog::IDAlign(void)
{
	return __idalign;
}

bool
ChangeUIDsAlingDialog::doSave()
{
    return __save;
}

void ChangeUIDsAlingDialog::on_buttonBox_accepted()
{
    //check for empty spinbox
	__idalign = ui->spinBox->value();
    __save = ui->checkBox->isChecked();

	if (__idalign == __idalignDefault) {
		QMessageBox::warning(this, "Invalid UID", "New seleted value is equal to old", "Ok");
	} else {
		this->accept();
	}
}

void ChangeUIDsAlingDialog::on_buttonBox_rejected()
{
    this->reject();
}

