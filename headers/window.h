#ifndef WINDOW_H
#define WINDOW_H

#include <sll/database.h>
#include <sll/base.h>
#include <sll/input.h>
#include <changeuidsalingdialog.h>
#include <initaccountsdialog.h>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QThread>
#include <QMutex>

#define DEFAULT_DATABASE_PATH "./SL_Stor.sqlite"

QT_BEGIN_NAMESPACE
namespace Ui
{
	class window;

	namespace columns {
		enum TREE_WIDGET_COLUMNS {
			UID,
			BOT_STATUS,
			RATING,
			LEAGUE,
			DEC,
		};
	};
}
QT_END_NAMESPACE

class twMenus : public QObject
{
	Q_OBJECT

	public:
		QMenu *headerMenu;
		QMenu *itemMenu;

		twMenus(QTreeWidget *tw);
		~twMenus();

	private:
		QTreeWidget *__tw;
		QAction *aToggleID;
		QAction *aToggleBotStatus;
		QAction *aToggleRating;
		QAction *aToggleLeague;
		QAction *aToggleDec;

		void doToggle(int col, bool visible);

	public slots:
		void show_headerMenu(const QPoint& p);
		void show_itemMenu(const QPoint& p);

	private slots:
		void on_actionToggleID(bool c)         { doToggle(Ui::columns::UID, c); }
		void on_actionToggleBotStatus(bool c)  { doToggle(Ui::columns::BOT_STATUS, c); }
		void on_actionToggleRating(bool c)     { doToggle(Ui::columns::RATING, c); }
		void on_actionToggleLeague(bool c)     { doToggle(Ui::columns::LEAGUE, c); }
		void on_actionToggleDec(bool c)        { doToggle(Ui::columns::DEC, c); }
};

class window : public QMainWindow
{
    Q_OBJECT
	QThread sl_db_Thread;

public:
    window(QWidget *parent = nullptr);
    ~window();

/* signals: */
/* 	void _itemChanged(Item& changed); */

private slots:
    void on_actionExit_triggered();
    void on_actionUse_dock_triggered(bool checked);
    void on_actionRead_data_from_file_triggered();

    void on_newitemInSLStor(SL::Item *newItem);
    void on_itemInSLStorchanged(SL::Item *changed);
	void on_itemsUIDsAlignReorganized(void);

    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

	void on_rightClickTWItem(const QPoint &);
	void on_rightClickTWHeader(const QPoint &);

    void on_treeWidget_destroyed();

    void on_pushButton_saveAccInfo_clicked();

    void on_lineEdit_username_textEdited(const QString &arg1);
    void on_lineEdit_posiginKey_textEdited(const QString &arg1);
    void on_lineEdit_email_textEdited(const QString &arg1);
    void on_lineEdit_password_textEdited(const QString &arg1);

    void on_progressBar_questProgress_valueChanged(int value);

    void on_lineEdit_username_returnPressed();

    void on_lineEdit_posiginKey_returnPressed();

    void on_lineEdit_email_returnPressed();

    void on_lineEdit_password_returnPressed();

    void on_actionEdit_UIDs_aligment_triggered();

	void dummy(void) { }

private:
    Ui::window *ui;
	twMenus *__twMenus;

	//mutexes
	QMutex canAccessdb;
	//data base
	SL::DataBase sl_db;

	//dialogs
    InitAccountsDialog    *IADialog;
    ChangeUIDsAlingDialog *CUADialog;

	SL::Stor __iStor;

	/* void updateDockValues(void); */
	/* void updateTableValues(void); */

    void enable_save_acc_info_button();
    void addItemToTreeWidget(SL::Item& item);
	QTreeWidgetItem *getItemYbyIDColumn(QTreeWidget *tw, QString id);

	void setValuesToDock(SL::Item *item);
	void setValuesToTable(SL::Item *item, QTreeWidgetItem *row);

};
#endif // WINDOW_H
