#ifndef CHANGEUIDSALINGDIALOG_H
#define CHANGEUIDSALINGDIALOG_H

#include <sll/base.h>

#include <QDialog>

namespace Ui {
class ChangeUIDsAlingDialog;
}

class ChangeUIDsAlingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeUIDsAlingDialog(SL::Stor& stor, QWidget *parent = nullptr);
    ~ChangeUIDsAlingDialog();

	long IDAlign(void);
    bool doSave();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ChangeUIDsAlingDialog *ui;
	long __idalign = -1;
	long __idalignDefault;
    bool __save = false;
};

#endif // CHANGEUIDSALINGDIALOG_H
