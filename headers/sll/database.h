#ifndef _SL_DB_H_
#define _SL_DB_H_

#include <string>
#include <sqlite3.h>
#include "base.h"

/* -- # TABLE PEREFERENCES # -- */
//mb const char ptrs instead?
#define TABLE_NAME			"SL_StorDB"
#define TABLE_UID			"UID"
#define TABLE_AUTH_TYPE		"AuthType"
#define TABLE_USERNAME		"Username"
#define TABLE_POSTIGIN_KEY	"PostiginKey"
#define TABLE_EMAIL			"EMail"
#define TABLE_PASSWORD		"Password"
#define TABLE_BOT_PATH		"BotPath"
#define TABLE_BOT_PRESET	"BotPreset"

namespace SL {

	/* class IDataBase */
	/* { */
	/* public: */
	/* 	virtual bool ready() = 0; */
	/* 	virtual bool insert(void *) = 0; */
	/* 	virtual bool insertAll(void *from) = 0; */
	/* 	virtual bool update(void *, int) = 0; */
	/* 	virtual bool remove(const int &) = 0; */
	/* 	virtual void *getRow(const int &, void *row) = 0; */
	/* 	virtual bool getAll(void *to) = 0; */
	/* 	virtual bool erase(void) = 0; */
	/* 	virtual bool close(void) = 0; */
	/* 	virtual int  size(void) = 0; */

	/* 	virtual ~IDataBase() {}; */
	/* }; */

	class DataBase : public QObject
	{
		Q_OBJECT

	private:
		enum TABLE_COLUMNS {
			ID_COLUMN = 0,
			AUTH_TYPE_COLUMN,
			UNAME_COLUMN,
			KEY_COLUMN,
			EMAIL_COLUMN,
			PASSWORD_COLUMN,
			BOT_PATH_COLUMN,
			BOT_PRESET_COLUMN,

			COLUMNS_COUNT,
		};

	private:
		sqlite3 *__db {nullptr};

		void sqlite3_get_from_column(sqlite3_stmt *stmt, Item *itemCopy); //TODO come up with a new name!! backa
		void sqlite3_bind_item(sqlite3_stmt *stmt,  const Item *item);

	public:
		DataBase(const std::string &path);
		virtual ~DataBase();

		virtual bool ready(void);

		bool insert(Item *item);
		bool insertAll(Stor &slstor);
		Item* getItem(const int id);
		bool getAll(Stor &to);
		bool remove(const int delId);
		bool update(const Item *item, int updId);

		bool erase(void);
		bool close(void);
		int  size(void);

	signals:
		void db_itemInserted(Item *item);
		void db_itemReaded(Item *item);

	private slots:
	};
};

#endif
