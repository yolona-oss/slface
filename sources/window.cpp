#include <qnamespace.h>
#include <window.h>
#include <QObject>
#include "./ui_window.h"

#include <iostream>
#include <thread>
#include <chrono>

using namespace SL;

twMenus::twMenus(QTreeWidget* tw)
{
	__tw = tw;
	headerMenu = new QMenu();
	itemMenu = new QMenu();

	//TODO go to grep values from tw directly!
	aToggleID = new QAction("ID");
	aToggleBotStatus = new QAction("Bot status");
	aToggleRating = new QAction("Rating");
	aToggleLeague = new QAction("League");
	aToggleDec = new QAction("Dec");

	aToggleID->setCheckable(true);
	aToggleID->setChecked(true);
	aToggleBotStatus->setCheckable(true);
	aToggleBotStatus->setChecked(true);
	aToggleRating->setCheckable(true);
	aToggleRating->setChecked(true);
	aToggleLeague->setCheckable(true);
	aToggleLeague->setChecked(true);
	aToggleDec->setCheckable(true);
	aToggleDec->setChecked(true);

	QObject::connect(aToggleID, &QAction::toggled,
			this, &twMenus::on_actionToggleID);
	QObject::connect(aToggleBotStatus, &QAction::toggled,
			this, &twMenus::on_actionToggleBotStatus);
	QObject::connect(aToggleRating, &QAction::toggled,
			this, &twMenus::on_actionToggleRating);
	QObject::connect(aToggleLeague, &QAction::toggled,
			this, &twMenus::on_actionToggleLeague);
	QObject::connect(aToggleDec, &QAction::toggled,
			this, &twMenus::on_actionToggleDec);

	headerMenu->addAction(aToggleID);
	headerMenu->addAction(aToggleBotStatus);
	headerMenu->addAction(aToggleRating);
	headerMenu->addAction(aToggleLeague);
	headerMenu->addAction(aToggleDec);

	itemMenu->addAction("Run bot", [=] (void) { });
	/* itemMenu->addAction("", [=] (void) { }); */
	itemMenu->addAction("Delete", [=] (void) { });

	__tw->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(__tw, &QWidget::customContextMenuRequested,
			this, &twMenus::show_itemMenu);

	__tw->header()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(__tw->header(), &QWidget::customContextMenuRequested,
			this, &twMenus::show_headerMenu);

}

twMenus::~twMenus()
{
	delete aToggleID;
	delete aToggleBotStatus;
	delete aToggleRating;
	delete aToggleLeague;
	delete aToggleDec;

	delete headerMenu;
	delete itemMenu;
}

void twMenus::show_headerMenu(const QPoint& p) { this->headerMenu->exec(QCursor::pos()); }
void twMenus::show_itemMenu(const QPoint& p)   { this->itemMenu->exec(QCursor::pos()); }

void
twMenus::doToggle(int col, bool visible)
{
	if (visible) {
		__tw->header()->showSection(col);
	} else {
		__tw->header()->hideSection(col);
	}
}

//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

void runDB(Stor *to) {
	DataBase db(DEFAULT_DATABASE_PATH);
	if (db.ready()) { //add check count
		db.getAll(*to);
	}
}

window::window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::window),
		 __iStor(),
		 sl_db(DEFAULT_DATABASE_PATH)
{
    ui->setupUi(this);
    ui->actionUse_dock->setChecked(true);

	__twMenus = new twMenus(ui->treeWidget);

    QObject::connect(&__iStor, &Stor::_itemAdded, this,
					&window::on_newitemInSLStor);
	QObject::connect(&__iStor, &Stor::_itemChanged, this,
					&window::on_itemInSLStorchanged);
	QObject::connect(&__iStor, &Stor::_itemsUIDsAlignReoganized,
					this, &window::on_itemsUIDsAlignReorganized);

	ui->statusbar->showMessage("Loading accounts data...", 10000);

	/* std::thread thr(runDB, &__iStor); */
	/* thr.detach(); */
	if (sl_db.ready()) { //add check count
		sl_db.getAll(__iStor);
	}
}

window::~window()
{
	delete __twMenus;
    delete ui;
}

void
window::on_actionExit_triggered()
{
    QApplication::exit();
}

void window::on_actionUse_dock_triggered(bool checked)
{
    if (checked) {
        ui->dockWidget_2->show();
    } else {
        ui->dockWidget_2->hide();
    }
}

void
window::on_itemsUIDsAlignReorganized(void)
{
	ui->treeWidget->clear();
	for (auto item : __iStor) {
		addItemToTreeWidget(*item);
	}
}

void
window::on_newitemInSLStor(Item *newItem)
{
    addItemToTreeWidget(*newItem);
}

QTreeWidgetItem *
window::getItemYbyIDColumn(QTreeWidget *tw, QString id)
{
	QList list = tw->findItems(id, Qt::MatchCaseSensitive, Ui::columns::UID);

	if (list.empty()) {
		return nullptr;
	} else {
        return list[0];
			//i->t();
	}
}

void
window::setValuesToDock(Item *item)
{
    ui->lineEdit_username->setText(
			QString::fromStdString(item->username()));
    ui->lineEdit_posiginKey->setText(
			QString::fromStdString(item->postiginKey()));
    ui->lineEdit_email->setText(
			QString::fromStdString(item->email()));
    ui->lineEdit_password->setText(
			QString::fromStdString(item->password()));

	ui->label_rating->setText(
			QString::number(item->rating()));
	ui->label_DEC->setText(
			QString::number(item->decBalance()));
	ui->label_league->setText(
			QString::fromStdString(SL::leagues::toStr(
					item->league() )));
	//league
	ui->progressBar_questProgress->setValue(
			item->questProgress()); //TODO add activator

	ui->label_botStatus->setText(
			QString::fromStdString(item->statusStr()));
	ui->label_botPath->setText(
			QString::fromStdString(item->path()));
}

void
window::setValuesToTable(Item *item, QTreeWidgetItem *row)
{
	/* row->setText(ID_COLUMN, */
	/* 		QString::number(changed->id())); */
	row->setText(Ui::columns::RATING,
			QString::number(item->rating()));
	row->setText(Ui::columns::BOT_STATUS,
			QString::fromStdString(item->statusStr()));
	row->setText(Ui::columns::DEC,
			QString::number(item->decBalance()));
	row->setText(Ui::columns::LEAGUE,
				QString::fromStdString(SL::leagues::toStr(
						item->league() )));
}

void
window::on_itemInSLStorchanged(Item *changed)
{
	long curID = ui->treeWidget->currentItem()->text(Ui::columns::UID).toInt();
	long chId = changed->id();

	if (curID == chId) {
		this->setValuesToDock(changed);
	}

	QTreeWidgetItem *tw_item = getItemYbyIDColumn(ui->treeWidget, QString::number(changed->id()));
	if (!tw_item) {
		return;
	} else {
		setValuesToTable(changed, tw_item);
	}
}

void
window::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current == nullptr) {
		return;
	}

    int id = current->text(Ui::columns::UID).toInt();

	setValuesToTable(__iStor.getItem(id), current);
	setValuesToDock(__iStor.getItem(id));
}

void
window::addItemToTreeWidget(Item& sl_item)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);

	using namespace Ui;

    item->setText(columns::UID,         QString::number(sl_item.id()));
    item->setText(columns::BOT_STATUS,  QString::fromUtf8(sl_item.statusStr().c_str()));
    item->setText(columns::RATING,      QString(QString::number(sl_item.rating())));
    item->setText(columns::LEAGUE,      QString::fromStdString(SL::leagues::toStr(SL::leagues::Default)));
    item->setText(columns::DEC,         QString::number(sl_item.decBalance()));
    ui->treeWidget->addTopLevelItem(item);
}

void window::on_treeWidget_destroyed()
{
    //free memory
}

void
window::on_pushButton_saveAccInfo_clicked()
{
	std::string uname = ui->lineEdit_username->text().toStdString(),
				key   = ui->lineEdit_posiginKey->text().toStdString(),
				email = ui->lineEdit_email->text().toStdString(),
				pass  = ui->lineEdit_password->text().toStdString();

    int id = ui->treeWidget->currentItem()->text(Ui::columns::UID).toInt();
    Item *item = __iStor.getItem(id);
	item->setUsername(uname)->
		setKey(key)->
		setEMail(email)->
		setPassword(pass);

	if (sl_db.ready()) {
		sl_db.update(item, id);
	} else {
		std::cerr << "Cant update Item!" << std::endl;
	}

    ui->pushButton_saveAccInfo->setDisabled(true);
}

void
window::on_rightClickTWItem(const QPoint &)
{
}

void
window::on_rightClickTWHeader(const QPoint &)
{
}

void
window::enable_save_acc_info_button(void)
{
	ui->pushButton_saveAccInfo->setDisabled(false);
}

void window::on_lineEdit_username_textEdited(const QString &arg1)
{
	enable_save_acc_info_button();
}

void window::on_lineEdit_posiginKey_textEdited(const QString &arg1)
{
	enable_save_acc_info_button();
}

void window::on_lineEdit_email_textEdited(const QString &arg1)
{
	enable_save_acc_info_button();
}

void window::on_lineEdit_password_textEdited(const QString &arg1)
{
	enable_save_acc_info_button();
}

void window::on_progressBar_questProgress_valueChanged(int value)
{
    //mb make color grad of progbar bg?
}


void window::on_lineEdit_username_returnPressed()
{
    on_pushButton_saveAccInfo_clicked();
}

void window::on_lineEdit_posiginKey_returnPressed()
{
    on_pushButton_saveAccInfo_clicked();
}

void window::on_lineEdit_email_returnPressed()
{
    on_pushButton_saveAccInfo_clicked();
}

void window::on_lineEdit_password_returnPressed()
{
    on_pushButton_saveAccInfo_clicked();
}

//DIALOG invocers

void
window::on_actionRead_data_from_file_triggered()
{
    IADialog = new InitAccountsDialog(this);
    IADialog->exec();

    if (IADialog->result() == QDialog::Accepted) {
        ui->statusbar->showMessage("Reading accounts data form file...", 10000);
		
		read_accounts_data_from_file(IADialog->getFileToReadFrom().toStdString(),
				__iStor,
				IADialog->getFormat(),
				IADialog->getRAF());

        ui->statusbar->showMessage("Saving accounts...", 10000);
        sl_db.insertAll(__iStor);
    }

	delete IADialog;
}

void window::on_actionEdit_UIDs_aligment_triggered() //TODO add check for an updating process, or stop all updaters
{
    CUADialog = new ChangeUIDsAlingDialog(__iStor, this);
    CUADialog->exec();

	if (CUADialog->result() == QDialog::Accepted) {
		long moveTo;
        ui->statusbar->showMessage("Reforming table...", 10000);
		moveTo = CUADialog->IDAlign();

		__iStor.changeUIDsAlign(moveTo);

		if (CUADialog->doSave()) {
			if (sl_db.ready()) {
				ui->statusbar->showMessage("Saving reforming table...", 10000);
				sl_db.erase();
				sl_db.insertAll(__iStor);
			} else {
				//mb do QMessage?
				ui->statusbar->showMessage("Cant save reforming table...", 10000);
			}
		}
	}

	delete CUADialog;
}
