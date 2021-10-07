#ifndef _SL_BASE_H_
#define _SL_BASE_H_

#define SPLINTERLANDS_API_URL "https://api.splinterlands.io/"

#include <QObject>
#include <QThread>
#include <QTimer>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
/* #include <QJson */

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QUrl>

#include <string>
#include <vector>
#include <array>
#include <chrono>

namespace SL
{
	class Item;
	class API_Interactor;
	class UpdaterQueue;
	class ItemUpdater;
	class ItemState;
	class Stor;

	typedef struct accountDetails accountDetails_t;
	typedef struct playerDetails  playerDetails_t;

	/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

	namespace authentication {
		enum Type {
			EMAIL,
			USERNAME,

			UNDEFINED,
		};
	};

	/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

	namespace botStat {
		enum Status {
			RUNNING,
			PAUSED,
			DISABLED,

			NOT_INSTALLED,
		};

		enum Run_state {
			WAITING_BATLE,
			FIGHT,
			LOGINING,
		};
	};

	/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

	namespace URL {
		namespace players {
			const std::string details  = SPLINTERLANDS_API_URL "players/details";
			const std::string balances = SPLINTERLANDS_API_URL "players/balances";
			const std::string quests   = SPLINTERLANDS_API_URL "players/quests";
		};
	};

	/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
	
	namespace leagues {
		const int tiers_per_league = 3;

		const std::string leagues_names[] {
			"Novice", //mb #define those?
			"Bronze",
			"Silver",
			"Gold",
			"Diamond",
			"Champion",
			"Undefined",
		};

		enum e_leagues {
			NOVICE = 0,
			Bronze,
			Silver,
			Gold,
			Diamond,
			Champion,

			Leagues_count,
		};

		const enum e_leagues Default = NOVICE;

		const std::string toStr(int l);
	};

	/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

	struct accountDetails {
		std::string username;
		std::string postiginKey;
		std::string email;
		std::string password;
	};

	struct playerDetails {
		int rating;
		int league;
		int questProgress; //mb make it unsign or use signed for detect unsynced value?
		double decBalance;
	};

	/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

	//Major object that contains all data of splinterlands account
	class Item : public QObject
	{
		Q_OBJECT
		QThread __api_interactorThread;

		private:
			long __id = 0;

			authentication::Type __auth_type = authentication::UNDEFINED;

			accountDetails_t __accountDetails { "", "", "", "" };
			playerDetails_t  __playerDetails  { 0, 0, 0, 0 };

			botStat::Status  __bot_status  = botStat::NOT_INSTALLED;
			std::string      __bot_path    = std::string();

			API_Interactor *__api_interactor;

		public:
			explicit Item(QObject *p = 0);
			virtual ~Item(void);

			API_Interactor *api(void) { return __api_interactor; }

			Item * setID(long id)  { __id = id; return this; }
			long   id(void) const { return __id; }

			friend bool operator< (const Item& a, const Item& b) { return a.id() < a.id();  }
			friend bool operator> (const Item& a, const Item& b) { return operator<(b, a);  }
			friend bool operator<=(const Item& a, const Item& b) { return a.id() <= a.id(); }
			friend bool operator>=(const Item& a, const Item& b) { return operator<=(b, a); }
			friend bool operator!=(const Item& a, const Item& b) { return a.id() != b.id(); }
			friend bool operator==(const Item& a, const Item& b) { return a.id() == b.id(); }

			friend  std::ostream& operator<<(std::ostream &out, const Item &i) { return i.print(out); };
			virtual std::ostream& print(std::ostream& out) const;

			//Account data
			static const authentication::Type PREFERED_AUTH_TYPE = authentication::USERNAME;

			Item* setAuthType(int type) { __auth_type = static_cast<authentication::Type>(type); return this; }
			int   authType(void) const  { return __auth_type; }

			Item *      setUsername(const std::string& uname) { __accountDetails.username = uname; /*emit _itemChanged(this);*/ return this; };
			std::string username(void) const                  { return __accountDetails.username; };
			Item *      setKey(const std::string& key)        { __accountDetails.postiginKey = key; /*emit _itemChanged(this);*/ return this; };
			std::string postiginKey(void) const               { return __accountDetails.postiginKey; };
			Item *      setEMail(const std::string& email)    { __accountDetails.email = email; /*emit _itemChanged(this);*/ return this; };
			std::string email(void) const                     { return __accountDetails.email; };
			Item *      setPassword(const std::string& pass)  { __accountDetails.password = pass; /*emit _itemChanged(this);*/ return this; };
			std::string password(void) const                  { return __accountDetails.password; };
			Item *      setRating(int points)                 { __playerDetails.rating = points; /*emit _itemChanged(this);*/ return this; };
			int         rating(void) const                    { return __playerDetails.rating; };
			Item *      setDecCount(double c)                 { __playerDetails.decBalance = c; /*emit _itemChanged(this);*/ return this; };
			double      decBalance(void) const                { return __playerDetails.decBalance; };
			Item *      setQuestProgress(int progress)        { __playerDetails.questProgress = progress; /*emit _itemChanged(this);*/ return this; };
			int         questProgress(void) const             { return __playerDetails.questProgress; };

			Item * setLeague(int l) { __playerDetails.league = l; return this; }
			int league(void) { return __playerDetails.league; }
			std::string leagueStr(void);

			//BOT
			botStat::Status status(void) const                   { return __bot_status; }
			std::string     statusStr(void) const;
			Item *          setBotPath(const std::string& bpath) { __bot_path = bpath; /*emit _itemChanged(this);*/ return this; }
			std::string     path(void) const                     { return __bot_path; }

			//Other
			bool   valid(void) const;
			Item * reset(void);

		signals:
			void _itemChanged(Item *);
			void _updateItem();
			void _reqFinished();

		private slots:
			void on_itemUpdated(bool success);
			void on_needUpdate();

	};

	/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

	class Stor : public QObject, public std::vector<Item*>
	{
		Q_OBJECT
		QThread __iUpdaterThread;

		private:
			long getItemPos(int id) const;
			long getAvalibleID() const;
			long __defaultFirstUID = 1;

			ItemUpdater *__iUpdater {nullptr};

		public:
			explicit Stor(QObject *p = 0);
			virtual ~Stor();
			
			bool isItemUIDsLinear(void);

			bool changeUIDsAlign(long to);

			long firstUID(void) { return (*this->begin())->id(); }

			Item* getItem(const int id) const;
			Item* addItem(Item *item);
			Stor* rmItem(const int id);

			void syncItems(void);
			/* void print(); */

		signals:
			void _itemAdded(Item *item);
			void _itemRemoved(long id);
			void _itemChanged(Item *item);
			void _itemsUIDsAlignReoganized();
			/* void syncItem(int id); */

		public slots:
			void on_childChanged(Item *item);
	};
	/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

	//Object for fetching data from splinterlands API
	//pb - player Balances
	//pd - player Details
	//pq - player Quests
	class API_Interactor : public QObject
	{
		Q_OBJECT

		private:
			Item &__item; //link with parent

			QNetworkAccessManager *nm_pd;
			QNetworkAccessManager *nm_pq;
			QNetworkAccessManager *nm_pb;

			QNetworkReply *nm_pdReply;
			QNetworkReply *nm_pqReply;
			QNetworkReply *nm_pbReply;

			bool __pd = false,
				 __pq = false,
				 __pb = false;

			bool __pd_completed = false;
			bool __pq_completed = false;
			bool __pb_completed = false;

		private:
			bool replyError(QNetworkReply *);
			void pushRequests(void);
			bool updateValues(void);

			void checkToFinalize(void);

			bool canPerformRequest(void);
			void finalize(bool success, bool free = true);

		public:
			static const std::size_t perUpdaterThreads = 3;

		public:
			explicit API_Interactor(Item& item, QObject *p = nullptr);
			virtual ~API_Interactor();

			bool isInteractionPossible(void);

		signals:
			void finished(bool success);
			void finishedForItem(Item *item);
			void updateReady(void);
			void _needUpdate(void);

		private slots:
			void on_playerDetailsResult(QNetworkReply *r);
			void on_playerQuestsResult(QNetworkReply *r);
			void on_playerBalancesResult(QNetworkReply *r);

		public slots:
			void on_needUpdate(void); //fetch data
	};

	/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
	//Item update scheduling

	class ItemState : public QObject
	{
		Q_OBJECT

		private:
			std::chrono::steady_clock::time_point __lastUpdateTime {
				std::chrono::steady_clock::now() };

		public:
			enum State {
				Execute, //api running
				Wait,   //its not time to execute
				Standby, //waiting for self turn in queue
			};

			Item *item {nullptr};
			int state;
			bool itsNew = true;
			std::chrono::steady_clock::time_point lastUpdateTime() {
				return __lastUpdateTime;
			}

			void setLastUpdateTime(std::chrono::steady_clock::time_point lut) { __lastUpdateTime = lut; }

			explicit ItemState(Item *pitem, QObject *p = 0);
			virtual ~ItemState() {};
	};

	class UpdaterQueue : public QObject
	{
		Q_OBJECT

		private:
			Stor& __stor; //link
			std::vector<ItemState*> queue;
			std::vector<Item*>     *sharedStandbyQueue;

		private:
			void createQueue(void);
			void clearQueue(void);
			long findItemPos(Item *item);

			bool isTimeToUpdate(ItemState *iState);

		public:
			explicit UpdaterQueue(Stor& stor);
			virtual ~UpdaterQueue();

		signals:

		public slots:
			std::vector<Item*> * getStandby(int count);
			/* void clear(void); */

		private slots:
			void on_interactorFinished(Item*);
			void on_itemAdded(Item*);
			void on_itemRemoved(long id);
			void on_itemChanged(Item*);
			void on_reorganized();
	};

	//Object to serve Stor with updating Item data via API_Interactor by qtimer
	class ItemUpdater : public QObject
	{
		Q_OBJECT
	
		private:
			Stor&        __stor;
			QTimer       *__timer;
			UpdaterQueue *__queue;

			//default setup in itemupdater.cpp
			static std::chrono::milliseconds __waitTime;
			std::chrono::milliseconds        __interval {100};
			bool __firstTime {true};

			std::size_t __maxThreads {21};

		public:
			ItemUpdater(Stor& stor, QObject *p = 0);
			virtual ~ItemUpdater();

			void setInterval(std::chrono::milliseconds msec) { __interval = msec; }
			static std::chrono::milliseconds waitTime(void) { return __waitTime; }
			void setWaitTime(std::chrono::milliseconds msec) { __waitTime = msec; }

		signals:
			void forseStop(void);
			void finished(void);

			void _outOfTurnRequest(Item *);
			void _forseUpdate(void);

		public slots:
			void doIteration(void);
			void stop(void);
			void start(void);

			//TODO
			void on_outOfTurnRequest(Item *);
			void on_forseUpdate(void);
	};
};

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

#endif
