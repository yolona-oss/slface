#include <sll/database.h>

#include <sqlite3.h>
#include <iostream>

using namespace SL;

DataBase::DataBase(const std::string &path)
{
	sqlite3_open(path.c_str(), &__db);

	const char *create_table_req("CREATE TABLE IF NOT EXISTS " TABLE_NAME "( \
				" TABLE_UID          " INT NOT NULL PRIMARY KEY, \
				" TABLE_AUTH_TYPE    " INT NOT NULL, \
				" TABLE_USERNAME     " TEXT, \
				" TABLE_POSTIGIN_KEY " key TEXT, \
				" TABLE_EMAIL        " email TEXT, \
				" TABLE_PASSWORD     " password TEXT, \
				" TABLE_BOT_PATH     " bot_path TEXT, \
				" TABLE_BOT_PRESET   " bot_path TEXT, \
				UNIQUE(" TABLE_UID "));");

	char *errMsg;

	int rc = sqlite3_exec(__db, create_table_req, 0, 0, &errMsg);

	if (rc != SQLITE_OK) {
		//TODO LOG/SHOW ERR MSG
		std::cerr << "Cant initialize Data base: " << sqlite3_errmsg(__db) << std::endl;
		sqlite3_free(errMsg);
		sqlite3_close(__db);
		__db = nullptr;
	}
}

DataBase::~DataBase()
{
	this->close();
}

void
DataBase::sqlite3_bind_item(sqlite3_stmt *stmt, const Item *item)
{
	sqlite3_bind_int(stmt, ID_COLUMN+1,
			item->id());
	sqlite3_bind_int(stmt, AUTH_TYPE_COLUMN+1,
			item->authType());

	if (! item->username().empty())
		sqlite3_bind_text(stmt, UNAME_COLUMN+1,
				item->username().c_str(),
				-1, SQLITE_TRANSIENT);

	if (! item->postiginKey().empty())
		sqlite3_bind_text(stmt, KEY_COLUMN+1,
				item->postiginKey().c_str(),
				-1, SQLITE_TRANSIENT);

	if (! item->email().empty())
		sqlite3_bind_text(stmt, EMAIL_COLUMN+1,
				item->email().c_str(),
				-1, SQLITE_TRANSIENT);

	if (! item->password().empty())
		sqlite3_bind_text(stmt, PASSWORD_COLUMN+1,
				item->password().c_str(),
				-1, SQLITE_TRANSIENT);

	if (! item->path().empty())
		sqlite3_bind_text(stmt, BOT_PATH_COLUMN+1,
				item->path().c_str(),
				-1, SQLITE_TRANSIENT);
	
	//TODO add preset saving
}

void
DataBase::sqlite3_get_from_column(sqlite3_stmt *stmt, Item *itemCopy)
{
	if (itemCopy) {
		char *tmp = nullptr;
		itemCopy->setID(sqlite3_column_int(stmt, ID_COLUMN));
		itemCopy->
			setAuthType(static_cast<authentication::Type>(
							sqlite3_column_int(stmt, AUTH_TYPE_COLUMN)));

		tmp = (char*)sqlite3_column_text(stmt, UNAME_COLUMN);
		if (tmp) itemCopy->setUsername(std::string(tmp));
		tmp = (char*)sqlite3_column_text(stmt, KEY_COLUMN);
		if (tmp) itemCopy->setKey(std::string(tmp));

		tmp = (char*)sqlite3_column_text(stmt, EMAIL_COLUMN);
		if (tmp) itemCopy->setEMail(std::string(tmp));
		tmp = (char*)sqlite3_column_text(stmt, PASSWORD_COLUMN);
		if (tmp) itemCopy->setPassword(std::string(tmp));

		tmp = (char*)sqlite3_column_text(stmt, BOT_PATH_COLUMN);
		if (tmp) itemCopy->setBotPath(std::string(tmp));
	} else {
		std::cout << "Cant bind values from table to Item: Item is NULL" << std::endl;
	}
}

bool DataBase::ready(void) { return (__db) ? true : false; }

bool
DataBase::insert(Item *item)
{
	bool ret = true;
	sqlite3_stmt *stmt;
	const char *insert_req = "INSERT INTO \
							 " TABLE_NAME "(\
									 " TABLE_UID ", \
									 " TABLE_AUTH_TYPE ", \
									 " TABLE_USERNAME ", \
									 " TABLE_POSTIGIN_KEY ", \
									 " TABLE_EMAIL ", \
									 " TABLE_PASSWORD ", \
									 " TABLE_BOT_PATH ", \
									 " TABLE_BOT_PRESET ") \
							 VALUES(?, ?, ?, ?, ?, ?, ?, ?);";
							 /* VALUES(?, ?, ?, ?, ?, ?, ?, ?) \ */
							 /* /1* ON CONFLICT (" TABLE_UID ") DO UPDATE SET " TABLE_UID " = " TABLE_UID " "; *1/ */

	int rc = sqlite3_prepare_v2(__db, insert_req, -1, &stmt, 0);

	if (rc == SQLITE_OK) {
		/* Item *itemp = static_cast<Item *>(Itemvp); */

		if (item->valid()) {
			sqlite3_bind_item(stmt, item);
			sqlite3_step(stmt); //TODO add check
		} else {
			std::cerr << "Cant insert Item: Item not valid!" << std::endl; //TODO error log
			ret = false;
		}
	} else {
		std::cerr << "Cant insert Item: " << sqlite3_errmsg(__db) << std::endl;
		ret = false;
	}
	sqlite3_finalize(stmt);

	return ret;
}

bool
DataBase::insertAll(Stor &stor)
{
	for (int i = 0; i < stor.size(); i++) {
		insert(stor[i]);
	}

	return true;
}

bool
DataBase::update(const Item *item, int updId) //TODO add check
{
	bool ret = true;
	const char *update_item_req= "UPDATE " TABLE_NAME " \
								  SET (\
									  " TABLE_UID ", \
									  " TABLE_AUTH_TYPE ", \
									  " TABLE_USERNAME ", \
									  " TABLE_POSTIGIN_KEY ", \
									  " TABLE_EMAIL ", \
									  " TABLE_PASSWORD ", \
									  " TABLE_BOT_PATH ", \
									  " TABLE_BOT_PRESET ") = \
								  (?, ?, ?, ?, ?, ?, ?, ?) \
								  WHERE " TABLE_UID " = ?";
	sqlite3_stmt *stmt;

	int rc = sqlite3_prepare_v2(__db, update_item_req, -1, &stmt, 0);
	if (rc == SQLITE_OK)
	{
		if (item->valid()) {
			sqlite3_bind_int(stmt, COLUMNS_COUNT+1,
					updId);
			sqlite3_bind_item(stmt, item);
			sqlite3_step(stmt);
		} else {
			ret = false;
		}
	} else {
		ret = false;
	}
	sqlite3_finalize(stmt);

	return ret;
}

bool
DataBase::remove(const int delId) //TODO add check
{
	const char *delete_req = "DELETE FROM ItemT WHERE id = ?";
	sqlite3_stmt *stmt;

	int rc = sqlite3_prepare_v2(__db, delete_req, -1, &stmt, 0);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, delId);
		sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);

	return true;
}

Item *
DataBase::getItem(const int id) //TODO
{
	Item *retItem = new (std::nothrow) Item();
	const char *select_by_id_req = "SELECT * FROM " TABLE_NAME " WHERE " TABLE_UID " = ?";
	sqlite3_stmt *stmt;

	int rc = sqlite3_prepare_v2(__db, select_by_id_req, -1, &stmt, 0);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, id);
		rc = sqlite3_step(stmt); //TODO add check

		if (rc == SQLITE_ROW) {
			sqlite3_get_from_column(stmt, retItem);
			sqlite3_step(stmt);
		} else {
			std::cerr << "No such entry in Stor" << std::endl;
		}
	} else {
		std::cerr << "Cant recive Item: " << sqlite3_errmsg(__db) << std::endl;
	}
	sqlite3_finalize(stmt);
	
	return retItem;
}

bool
DataBase::getAll(Stor &stor)
{
	bool ret = true;
	const char *select_by_id_req = "SELECT * FROM " TABLE_NAME "";
	sqlite3_stmt *stmt;

	Item *copyItem;

	int rc = sqlite3_prepare_v2(__db, select_by_id_req, -1, &stmt, 0);
	if (rc == SQLITE_OK) {
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			copyItem = new Item;
			sqlite3_get_from_column(stmt, copyItem);
			stor.addItem(copyItem);
		}
	} else {
		std::cerr << "Cant get all rows: " << sqlite3_errmsg(__db) << std::endl;
	}

	return ret;
}

bool DataBase::erase(void)
{
	const char *delete_all_req = "DELETE FROM " TABLE_NAME "";
	char *errMsg;

	int rc = sqlite3_exec(__db, delete_all_req, 0, 0, &errMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(errMsg); //show err TODO
		return false;
	}

	return true;
}

bool DataBase::close(void)
{
	if (__db) {
		sqlite3_close(__db);
		return true;
	} else {
		return false;
	}
}

int DataBase::size(void)
{
	return sqlite3_last_insert_rowid(__db);
}
