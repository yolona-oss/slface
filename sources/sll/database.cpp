#include <sll/database.h>

#include <QFile>
#include <QVariant>
#include <iostream>

using namespace SL;

DataBase::DataBase(QObject *p) :
	QObject(p)
{
}

DataBase::~DataBase()
{
	this->close();
}

bool
DataBase::connect()
{
	if(!QFile(DATABASE_NAME).exists()){
		this->restore();
	} else {
		this->open();
	}
	return true;
}

bool
DataBase::restore()
{
	if(this->open()){
		return (this->createTable()) ? true : false;
	} else {
		/* "Не удалось восстановить базу данных"; */
		return false;
	}
	return false;
}

bool
DataBase::open()
{
	__db = QSqlDatabase::addDatabase("QSQLITE");
	__db.setHostName(TABLE_NAME);
	__db.setDatabaseName("./" DATABASE_NAME);
	if(__db.open()){
		return true;
	} else {
		return false;
	}
}

bool
DataBase::close()
{
	__db.close();
	return true;
}

bool
DataBase::createTable()
{
	QSqlQuery query;
	if (!query.exec("CREATE TABLE IF NOT EXISTS " TABLE_NAME "( \
				" TABLE_UID          " INT NOT NULL PRIMARY KEY, \
				" TABLE_AUTH_TYPE    " INT NOT NULL, \
				" TABLE_USERNAME     " TEXT, \
				" TABLE_POSTIGIN_KEY " key TEXT, \
				" TABLE_EMAIL        " email TEXT, \
				" TABLE_PASSWORD     " password TEXT, \
				" TABLE_BOT_PATH     " bot_path TEXT, \
				" TABLE_BOT_PRESET   " bot_path TEXT, \
				UNIQUE(" TABLE_UID "));"))
	{
		//err
		std::cerr << "cant create table" << std::endl;
		std::cerr << query.lastError().text().toStdString() << std::endl;
		return false;
	} else {
		return true;
	}

	return false;
}

void
DataBase::sql_bind_values(QSqlQuery& query, const Item *item)
{
	//rewrite with defenitions
	query.bindValue(":ID",				QVariant::fromValue(item->id()));
	query.bindValue(":AUTH_TYPE",		QVariant::fromValue(item->authType()));

	query.bindValue(":USERNAME",		QVariant(QString::fromStdString(item->username())));
	query.bindValue(":POSTIGIN_KEY",	QVariant(QString::fromStdString(item->postiginKey())));
	query.bindValue(":EMAIL",			QVariant(QString::fromStdString(item->email())));
	query.bindValue(":PASSWORD",		QVariant(QString::fromStdString(item->password())));
	query.bindValue(":BOT_PATH",		QVariant(QString::fromStdString(item->path())));
	/* query.bindValue(":BOT_PRESET", QVariant::fromValue(item->())); */

	//TODO add preset saving
}

void
DataBase::sql_read_values(const QSqlQuery& query, Item *item)
{
	if (!item) {
		std::cerr << "Cant bind values from table to Item: Item is NULL" << std::endl;
		return;
	}

	QSqlRecord rec = query.record();

	//id & authentication type
	item->setID(query.value(
				rec.indexOf(TABLE_UID))
			.toInt());
	item->setAuthType(query.value(rec.indexOf(TABLE_AUTH_TYPE))
			.toInt());

	char *tmp = nullptr;

	//bounding all we can
	//username
	tmp = (char*)query.value(
				rec.indexOf(TABLE_USERNAME))
			.toString().toStdString().c_str();
	if (tmp) item->setUsername(tmp);

	//postigin key
	tmp = (char*)query.value(
				rec.indexOf(TABLE_POSTIGIN_KEY))
			.toString().toStdString().c_str();
	if (tmp) item->setKey(tmp);

	tmp = 0;

	//email
	tmp = (char*)query.value(
			rec.indexOf(TABLE_EMAIL))
		.toString().toStdString().c_str();
	if (tmp) item->setEMail(tmp);

	//password
	tmp = (char*)query.value(
			rec.indexOf(TABLE_PASSWORD))
		.toString().toStdString().c_str();
	if (tmp) item->setPassword(tmp);

	//bot path
	tmp = (char*)query.value(
			rec.indexOf(TABLE_BOT_PATH))
			.toString().toStdString().c_str();
	if (tmp) item->setBotPath(tmp);

	//bot preset
	/* tmp = (char*)query.boundValue(BOT_PRESET_COLUMN).toString().toStdString().c_str(); */
	/* if (tmp) item->setBotPreset(tmp); */
}

bool
DataBase::insert(const Item *item)
{
	QSqlQuery query; //TODO add check
	query.prepare("INSERT INTO \
				  " TABLE_NAME "(\
						 " TABLE_UID ", \
						 " TABLE_AUTH_TYPE ", \
						 " TABLE_USERNAME ", \
						 " TABLE_POSTIGIN_KEY ", \
						 " TABLE_EMAIL ", \
						 " TABLE_PASSWORD ", \
						 " TABLE_BOT_PATH ", \
						 " TABLE_BOT_PRESET ") \
				 VALUES(:ID, :AUTH_TYPE, :USERNAME, :POSTIGIN_KEY, :EMAIL, :PASSWORD, :BOT_PATH, :BOT_PRESET);");
				 /* VALUES(?, ?, ?, ?, ?, ?, ?, ?) \ */
				 /* /1* ON CONFLICT (" TABLE_UID ") DO UPDATE SET " TABLE_UID " = " TABLE_UID " "; *1/ */

	if (item->valid()) {
		sql_bind_values(query, item);
	} else {
		std::cerr << "Cant insert Item: Item not valid!" << std::endl; //TODO error log
		return false;
	}

	if (!query.exec()) {
		std::cerr << query.lastError().text().toStdString() << std::endl;
		return false;
	} else {
		return true;
	}

	return false;
}

bool
DataBase::insertEvery(Stor *stor)
{
	for (int i = 0; i < stor->size(); i++) {
		this->insert(stor->at(i));
	}

	return true;
}

bool
DataBase::update(const Item *item) //TODO add check
{
	QSqlQuery query;
	query.prepare("UPDATE " TABLE_NAME " \
				SET (\
					  " TABLE_UID ", \
					  " TABLE_AUTH_TYPE ", \
					  " TABLE_USERNAME ", \
					  " TABLE_POSTIGIN_KEY ", \
					  " TABLE_EMAIL ", \
					  " TABLE_PASSWORD ", \
					  " TABLE_BOT_PATH ", \
					  " TABLE_BOT_PRESET ") = \
				VALUES(:ID, :AUTH_TYPE, :USERNAME, :POSTIGIN_KEY, :EMAIL, :PASSWORD, :BOT_PATH, :BOT_PRESET)\
				WHERE " TABLE_UID " = :CHANGE_ID");

	if (item->valid()) {
		query.bindValue(":CHANGE_ID", QVariant::fromValue(item->id()));
		sql_bind_values(query, item);
	} else {
		return false;
	}

	if (!query.exec()) {
		std::cerr << query.lastError().text().toStdString() << std::endl;
		return false;
	} else {
		return true;
	}

	return false;
}

bool
DataBase::remove(const long id) //TODO add check
{
	QSqlQuery query;
	query.prepare("DELETE FROM ItemT WHERE id = :DEL_ID");

	query.bindValue(":DEL_ID", QVariant::fromValue(id));

	if (!query.exec()) {
		std::cerr << query.lastError().text().toStdString() << std::endl;
		return false;
	} else {
		return true;
	}

	return true;
}

Item *
DataBase::read(const long id) //TODO
{
	QSqlQuery query;
	query.prepare("SELECT * FROM " TABLE_NAME " WHERE " TABLE_UID " = :ID");

	Item *retItem = new (std::nothrow) Item();

	query.bindValue(":ID", QVariant::fromValue(id));

	if (!query.exec()) {
		std::cerr << query.lastError().text().toStdString() << std::endl;
		return nullptr;
	}

	sql_read_values(query, retItem);
	
	return retItem;
}

bool
DataBase::readEvery(Stor *stor)
{
	Item *copyItem;
	QSqlQuery query;
	QSqlRecord rec;

	if (query.exec("SELECT * FROM " TABLE_NAME "")) {
		while (query.next()) {
			copyItem = new Item;
			sql_read_values(query, copyItem);
			stor->addItem(copyItem);
		} 
	} else {
		std::cerr << "cant read every row in table" << std::endl;
		std::cerr << query.lastError().text().toStdString() << std::endl;
		return false;
	}

	return true;
}

bool
DataBase::erase(void)
{
	QSqlQuery query;
	query.exec("DELETE FROM " TABLE_NAME "");
	return true;
}

/* long */
/* DataBase::size(void) */
/* { */
/* 	return __db.; */
/* } */
