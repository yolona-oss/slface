#include <cctype>
#include <qnamespace.h>
#include <sll/base.h>

#include <iostream>

using namespace SL;

API_Interactor::API_Interactor(Item& item, QObject *p) :
	QObject(p),
		__item(item)
{
	nm_pd = new (std::nothrow) QNetworkAccessManager(this);
	nm_pq = new (std::nothrow) QNetworkAccessManager(this);
	nm_pb= new (std::nothrow) QNetworkAccessManager(this);

	connect(nm_pq, &QNetworkAccessManager::finished,
			this, &API_Interactor::on_playerQuestsResult, Qt::QueuedConnection);
	connect(nm_pd, &QNetworkAccessManager::finished,
			this, &API_Interactor::on_playerDetailsResult, Qt::QueuedConnection);
	connect(nm_pb, &QNetworkAccessManager::finished,
			this, &API_Interactor::on_playerBalancesResult, Qt::QueuedConnection);

	connect(this, &API_Interactor::_needUpdate,
			this, &API_Interactor::on_needUpdate);
}

API_Interactor::~API_Interactor()
{
	//TODO mb delete replies?
	delete nm_pd;
	delete nm_pq;
	delete nm_pb;
}

bool
API_Interactor::isInteractionPossible()
{
	if (__item.username().empty()) { //add network check
		return false;
	} else {
		return true;
	}
}

void
API_Interactor::checkToFinalize(void)
{
	if (__pd_completed && __pb_completed && __pq_completed) {
		__pd_completed = false;
		__pb_completed = false;
		__pq_completed = false;
		std::cout << "exiting api interactor" << std::endl;
		finalize(__pd || __pq || __pb); //process data and save directly into item
	}
}

void
API_Interactor::pushRequests(void) //TODO add setTransferTimeout
{
	//Fetch -- RATING --

	QUrl url(QString::fromStdString(URL::players::details));
	QNetworkRequest pd_request;
	QUrlQuery pd_query;
	pd_query.addQueryItem("name", QString::fromStdString(__item.username()));
	url.setQuery(pd_query);
	pd_request.setUrl(url);
	/* std::cout << url. << std::endl; */
	nm_pdReply = nm_pd->get(pd_request);

	//Fetch -- DEC --

	url = QString::fromStdString(URL::players::balances);
	QNetworkRequest pb_request;
	QUrlQuery pb_query;
	pd_query.addQueryItem("usernamename", QString::fromStdString(__item.username()));
	url.setQuery(pb_query);
	pb_request.setUrl(url);
	nm_pbReply = nm_pb ->get(pb_request);

	//Fetch -- QUEST --

	QUrlQuery pq_query;
	pd_query.addQueryItem("usernamename", QString::fromStdString(__item.username()));
	url = QString::fromStdString(URL::players::quests);
	url.setQuery(pq_query);
	QNetworkRequest pq_request;
	pq_request.setUrl(url);
	nm_pqReply = nm_pq->get(pq_request);
}

void
API_Interactor::on_playerDetailsResult(QNetworkReply *r)
{
	__pd_completed = true;
	if (replyError(r)) {
		__pd = false;
		checkToFinalize();
		return;
	}
	rapidjson::Document doc;

	//Rating
	doc.Parse(r->readAll().toStdString().c_str());
	assert(doc.IsObject());
	__item.setRating(doc["rating"].GetInt());

	//League
	__item.setLeague(doc["league"].GetInt());

	__pd = true;
	checkToFinalize();
}

void
API_Interactor::on_playerQuestsResult(QNetworkReply *r)
{
	__pq_completed = true;
	if (replyError(r)) {
		__pq = false;
		checkToFinalize();
		return;
	}

	rapidjson::Document doc;

	//Quest
	doc.Parse(r->readAll().toStdString().c_str());
	assert(doc.IsArray());

	for (auto& itr : doc.GetArray()) {
		__item.setQuestProgress(itr["completed_items"].GetInt());
		break;
	}

	__pq = true;
	checkToFinalize();
}

void
API_Interactor::on_playerBalancesResult(QNetworkReply *r)
{
	__pb_completed = true;
	if (replyError(r)) {
		__pb = false;
		checkToFinalize();
		return;
	}
	rapidjson::Document doc;

	//DEC count
	doc.Parse(r->readAll().toStdString().c_str());
	assert(doc.IsArray());

	for (auto& itr : doc.GetArray()) {
		if (!strcmp(itr["token"].GetString(), "DEC")) {
			__item.setDecCount(itr["balance"].GetDouble());
			break;
		}
	}

	__pb = true;
	checkToFinalize();
}

bool
API_Interactor::replyError(QNetworkReply *r)
{
	if (r->error()) {
		std::cerr << r->errorString().toStdString() << std::endl;
		return true;
	} else {
		return false;
	}

	return true;
}

/* bool */
/* API_Interactor::updatePlayerDetails(void) */
/* { */
/* 	if (badResnose(__r_playerDetails)) { */
/* 		return false; */
/* 	} */

/* 	rapidjson::Document doc; */

/* 	//Rating */
/* 	doc.Parse(__r_playerDetails.text.c_str()); */
/* 	assert(doc.IsObject()); */
/* 	__item.setRating(doc["rating"].GetInt()); */

/* 	//League */
/* 	__item.setLeague(doc["league"].GetInt()); */

/* 	return tRue; */
/* } */

/* bool */
/* API_Interactor::updatePlayerBalances(void) */
/* { */
/* 	if (badResnose(__r_playerBalance)) { */
/* 		return false; */
/* 	} */

/* 	rapidjson::Document doc; */

/* 	//Rating */
/* 	doc.Parse(__r_playerDetails.text.c_str()); */
/* 	assert(doc.IsObject()); */
/* 	__item.setRating(doc["rating"].GetInt()); */

/* 	//League */
/* 	__item.setLeague(doc["league"].GetInt()); */

/* 	return true; */
/* } */

/* bool */
/* API_Interactor::updatePlayerQuests(void) */
/* { */
/* 	if (badResnose(__r_playerQuests)) { */
/* 		return false; */
/* 	} */

/* 	rapidjson::Document doc; */

/* 	//Quest */
/* 	doc.Parse(__r_playerQuests.text.c_str()); */
/* 	assert(doc.IsArray()); */

/* 	for (auto& itr : doc.GetArray()) { */
/* 		__item.setQuestProgress(itr["completed_items"].GetInt()); */
/* 		break; */
/* 	} */

/* 	return true; */
/* } */

bool
API_Interactor::canPerformRequest(void)
{
	if (__item.username().empty() || !isTimeToRequest()) {
		return false;
	} else {
		return true;
	}
}

bool
API_Interactor::isTimeToRequest()
{
	using namespace std::chrono;
	auto now = steady_clock::now();
	if (duration_cast<milliseconds>(now - __lastGetTime) >= G_GET_REQ_GAP_TIME || __firstTime) {
		__firstTime = false;
		__lastGetTime = steady_clock::now(); //update timer
		return true;
	} else {
		return false;
	}
}

void
API_Interactor::finalize(bool success)
{
	__pd = false;
	__pq = false;
	__pb = false;
	emit finishedForItem(this->__item);
	emit finished(success);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*SLOTS*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

void
API_Interactor::on_needUpdate() //TODO make forse request prop
{
	if (!canPerformRequest()) {
		finalize(false);
		return;
	}

	pushRequests(); //interact with api
}

