#include <cctype>
#include <chrono>
#include <qnamespace.h>
#include <sll/base.h>

#include <iostream>
#include <utility>

using namespace SL;

API_Interactor::API_Interactor(Item& item, QObject *p) :
	__item(item)
{
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
	nm_pdReply = nm_pd->get(pd_request);

	//Fetch -- DEC --

	url = QString::fromStdString(URL::players::balances);
	QNetworkRequest pb_request;
	QUrlQuery pb_query;
	pb_query.addQueryItem("username", QString::fromStdString(__item.username()));
	url.setQuery(pb_query);
	pb_request.setUrl(url);
	nm_pbReply = nm_pb ->get(pb_request);

	//Fetch -- QUEST --

	QUrlQuery pq_query;
	pd_query.addQueryItem("username", QString::fromStdString(__item.username()));
	url = QString::fromStdString(URL::players::quests);
	url.setQuery(pq_query);
	QNetworkRequest pq_request;
	pq_request.setUrl(url);
	nm_pqReply = nm_pq->get(pq_request);

	/* connect(nm_pbReply, &QIODevice::readyRead, */
	/* 		this, &API_Interactor::on_playerBalancesResult); */
	/* /1* connect(nm_pbReply, &QNetworkReply::errorOccurred, *1/ */
	/* /1* 		this, &API_Interactor::on_playerBalancesResult); *1/ */
	/* /1* connect(nm_pbReply, &QNetworkReply::sslErrors, *1/ */
	/* /1* 		this, &API_Interactor::on_playerBalancesResult); *1/ */

	/* connect(nm_pqReply, &QIODevice::readyRead, */
	/* 		this, &API_Interactor::on_playerQuestsResult); */
	/* /1* connect(nm_pqReply, &QNetworkReply::errorOccurred, *1/ */
	/* /1* 		this, &API_Interactor::on_playerQuestsResult); *1/ */
	/* /1* connect(nm_pqReply, &QNetworkReply::sslErrors, *1/ */
	/* /1* 		this, &API_Interactor::on_playerQuestsResult); *1/ */

	/* connect(nm_pdReply, &QIODevice::readyRead, */
	/* 		this, &API_Interactor::on_playerDetailsResult); */
	/* /1* connect(nm_pdReply, &QNetworkReply::errorOccurred, *1/ */
	/* /1* 		this, &API_Interactor::on_playerDetailsResult); *1/ */
	/* /1* connect(nm_pdReply, &QNetworkReply::sslErrors, *1/ */
	/* /1* 		this, &API_Interactor::on_playerDetailsResult); *1/ */
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

	QString val = r->readAll();
	QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());

	assert(doc.isObject());

	//Rating
	__item.setRating(doc["rating"].toInt());

	//League
	__item.setLeague(doc["league"].toInt());

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

	QString val = r->readAll();
	QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());

	assert(doc.isArray());

	//Quest

	for (auto itr : doc.array()) { //completed_items
		QJsonObject d = itr.toObject();
		__item.setQuestProgress(d["completed_items"].toInt());
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

	QJsonDocument doc = QJsonDocument::fromJson(r->readAll());

	assert(doc.isArray());

	//DEC count
	
	QJsonArray root = doc.array();

	/* std::cout << root.at(0).toObject()["0"].toString().toStdString() << std::endl; */
	for (int i = 0; i < root.count(); ++i) {
		QJsonObject sub = root.at(i).toObject();
		if (sub.value("token").toString() == "DEC") {
			__item.setDecCount(sub.value("balance").toDouble());
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

bool
API_Interactor::canPerformRequest(void)
{
	if (__item.username().empty()) {
		return false;
	} else {
		return true;
	}
}

void
API_Interactor::finalize(bool success, bool free)
{
	__pd = false;
	__pq = false;
	__pb = false;

	if (free) {
		delete nm_pd;
		delete nm_pb;
		delete nm_pq;
		/* delete nm_pbReply; */
		/* delete nm_pdReply; */
		/* delete nm_pqReply; */
	}

	emit finishedForItem(&(this->__item));
	emit finished(success);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*SLOTS*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

void
API_Interactor::on_needUpdate() //TODO make forse request prop
{
	if (!canPerformRequest()) {
		finalize(false, false);
		return;
	}

	nm_pd = new (std::nothrow) QNetworkAccessManager();
	nm_pq = new (std::nothrow) QNetworkAccessManager();
	nm_pb = new (std::nothrow) QNetworkAccessManager();

	if (!nm_pd || !nm_pq || !nm_pb) {
		finalize(false, false);
	}

	connect(nm_pq, &QNetworkAccessManager::finished,
			this,  &API_Interactor::on_playerQuestsResult, Qt::DirectConnection);
	connect(nm_pd, &QNetworkAccessManager::finished,
			this,  &API_Interactor::on_playerDetailsResult, Qt::DirectConnection);
	connect(nm_pb, &QNetworkAccessManager::finished,
			this,  &API_Interactor::on_playerBalancesResult, Qt::DirectConnection);

	pushRequests(); //interact with api
}

