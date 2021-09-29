#ifndef INITACCOUNTSDIALOG_H
#define INITACCOUNTSDIALOG_H

#include <QDialog>
#include <string>
#include <sll/input.h>
#include <sll/base.h>
#include <sll/database.h>

namespace Ui {
	class InitAccountsDialog;
}

class InitAccountsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InitAccountsDialog(QWidget *parent = nullptr);
    ~InitAccountsDialog();

	QString getFileToReadFrom(void);
	std::string getFormat(void);
	SL::ReadAccsFlag getRAF(void);

private slots:
    void on_pushButton_choose_file_clicked();

    void on_buttonBox_accepted();

    void on_comboBox_field1_activated(int index);
    void on_comboBox_field2_activated(int index);
    void on_comboBox_field3_activated(int index);
    void on_comboBox_field4_activated(int index);

    void on_buttonBox_rejected();

private:
    enum FMT_FIELDS {
        USERNAME_FIELD = 0,
        KEY_FIELD,
        EMAIL_FIELD,
        PASS_FIELD,
    };

private:
    Ui::InitAccountsDialog *ui;

    QString __filename;
	SL::ReadAccsFlag __rafFlags = 0;
	std::string __fmt;

	SL::ReadAccsFlag read_vector = SL::RAF_COLUMN;
	SL::ReadAccsFlag skipnl = 0;
    char ind2fmt(FMT_FIELDS val);
};

#endif // INITACCOUNTSDIALOG_H
