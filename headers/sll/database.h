#ifndef _SL_DB_H_
#define _SL_DB_H_

#include <string>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "base.h"

/* -- # TABLE PEREFERENCES # -- */
//mb const char ptrs instead?
#define DATABASE_NAME		"SL_Stor.sqlite"
#define TABLE_NAME			"SL_Stor"
#define TABLE_UID			"UID"
#define TABLE_AUTH_TYPE		"AuthType"
#define TABLE_USERNAME		"Username"
#define TABLE_POSTIGIN_KEY	"PostiginKey"
#define TABLE_EMAIL			"EMail"
#define TABLE_PASSWORD		"Password"
#define TABLE_BOT_PATH		"BotPath"
#define TABLE_BOT_PRESET	"BotPreset"

namespace SL {

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
		QSqlDatabase __db;

	private:
		bool open();
		bool close();
		bool restore();
		bool createTable();

		void sql_read_values(const QSqlQuery &query, Item *item); //TODO come up with a new name!! backa
		void sql_bind_values(QSqlQuery &query, const Item *item);

	public:
		explicit DataBase(QObject *p = 0);
		virtual ~DataBase();

		bool connect(void);
		/* long size(); */

	signals:

	public slots:
		bool insert(const Item *);
		bool insertEvery(Stor *);
		Item * read(const long id);
		bool readEvery(Stor *);
		bool remove(const long id);
		bool update(const Item *);
		bool erase(void);
	};
};

#endif
