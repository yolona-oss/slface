#include <sll/base.h>

#include <iostream>

using namespace SL;

API_Interactor::API_Interactor(Item& item, QObject *p) :
	QObject(p),
		__item(item)
{
	connect(this, &API_Interactor::_needUpdate,
			this, &API_Interactor::on_needUpdate);
}

API_Interactor::~API_Interactor()
{
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
API_Interactor::pushRequest(void)
{
	//Fetch -- RATING --
	__r_playerDetails = cpr::Get(cpr::Url{URL::players::details},
								 cpr::Parameters{ {"name", __item.username()} } );

	//Fetch -- DEC --
	__r_playerBalance = cpr::Get(cpr::Url{URL::players::balances},
								 cpr::Parameters{{"username", __item.username()}} );

	//Fetch -- QUEST --
	__r_playerQuests = cpr::Get(cpr::Url{URL::players::quests},
								 cpr::Parameters{{"username", __item.username()}} );

	//TODO add power and quest details
}

bool
API_Interactor::badResnose(cpr::Response r)
{
	if (!r.status_code) {
		std::cerr << "Errors for Request url: " << r.url << std::endl;
		std::cerr << r.error.message << std::endl;
		return true;
	} else if (r.status_code >= 400) {
		std::cerr << "Errors for Request url: " << r.url << std::endl;
		std::cerr << "Error [" << r.status_code << "] making request" << std::endl;
		return true;
	}

	return false;
}

bool
API_Interactor::responsesOK(void) //TODO
{
	//Check responce status
	return !badResnose(__r_playerQuests) &&
		!badResnose(__r_playerBalance) &&
		!badResnose(__r_playerDetails);
}

bool
API_Interactor::updatePlayerDetails(void)
{
	if (badResnose(__r_playerDetails)) {
		return false;
	}

	rapidjson::Document doc;

	//Rating
	doc.Parse(__r_playerDetails.text.c_str());
	assert(doc.IsObject());
	__item.setRating(doc["rating"].GetInt());

	//League
	__item.setLeague(doc["league"].GetInt());

	return true;
}

bool
API_Interactor::updatePlayerBalances(void)
{
	if (badResnose(__r_playerBalance)) {
		return false;
	}

	rapidjson::Document doc;

	//DEC count
	doc.Parse(__r_playerBalance.text.c_str());
	assert(doc.IsArray());

	for (auto& itr : doc.GetArray()) {
		if (!strcmp(itr["token"].GetString(), "DEC")) {
			__item.setDecCount(itr["balance"].GetDouble());
			break;
		}
	}

	return true;
}

bool
API_Interactor::updatePlayerQuests(void)
{
	if (badResnose(__r_playerQuests)) {
		return false;
	}

	rapidjson::Document doc;

	//Quest
	doc.Parse(__r_playerQuests.text.c_str());
	assert(doc.IsArray());

	for (auto& itr : doc.GetArray()) {
		__item.setQuestProgress(itr["completed_items"].GetInt());
		break;
	}

	return true;
}

bool
API_Interactor::updateValues(void)
{
	bool pd = updatePlayerDetails();
	bool pq = updatePlayerQuests();
	bool pb = updatePlayerBalances();

	return pd || pq || pb;
}

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

	pushRequest(); //interact with api
	finalize(updateValues()); //process data and save directly into item
}

