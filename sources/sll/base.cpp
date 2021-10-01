#include <qnamespace.h>
#include <sll/base.h>

#include <iostream>
#include <string>
#include <math.h>

using namespace SL;

/*-*-*-*-*-*-*-*-*-*-*-*-*-Item-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

namespace SL {
	namespace leagues {
		//first - int from API
		//second - processed value to league number
		const std::string Tier(int l, int League) {
			if (l == NOVICE) { //zero
				return "";
			} else {
				return std::to_string(League*3 - (l-1));
			}
		}

		const std::string LeagueTier(int l) {
			if (l == NOVICE) { //its zero
				return leagues_names[NOVICE];
			}

			double del = static_cast<double>(l+tiers_per_league) / tiers_per_league - 0.3;

			int asdf = static_cast<int>(floor(del));

			return leagues_names[asdf] + " " + Tier(l, asdf);
		}

		const std::string toStr(int leaguen) {
			return LeagueTier(leaguen);
		}
	};
}

Item::Item(QObject *p) :
	QObject(p),
		__id(-1)
{
	__api_interactor = new API_Interactor(*this, this);

	connect(__api_interactor, &API_Interactor::finished,
			&__api_interactorThread, &QThread::terminate, Qt::DirectConnection);
	connect(&__api_interactorThread, &QThread::started,
			__api_interactor, &API_Interactor::on_needUpdate, Qt::DirectConnection);

	connect(this, &Item::_updateItem,
			this, &Item::on_needUpdate);
	connect(__api_interactor, &API_Interactor::finished,
			this, &Item::on_itemUpdated, Qt::DirectConnection);
}

Item::~Item()
{
	delete __api_interactor;
}

Item *
Item::reset(void)
{
	return setAuthType(authentication::UNDEFINED)->
			   setUsername("")->
			   setKey("")->
			   setEMail("")->
			   setPassword("")->
			   setBotPath("");
}

std::string
Item::statusStr(void) const
{
	switch (this->__bot_status) {
		case botStat::PAUSED:
			return std::string("Paused"); break;
		case botStat::RUNNING:
			return std::string("Running"); break;
		case botStat::DISABLED:
			return std::string("Disabled"); break;
		case botStat::NOT_INSTALLED:
			return std::string("Not Installed"); break;
		default:
			return std::string("Undefined"); break;
	}
}

bool
Item::valid(void) const
{
	if (__auth_type == authentication::EMAIL) {
		if (__accountDetails.email.empty() || __accountDetails.password.empty()) {
			return false;
		}
	} else if (__auth_type == authentication::USERNAME) {
		if (__accountDetails.username.empty() || __accountDetails.postiginKey.empty()) {
			return false;
		}
	} else {
		return false;
	}
	return true;
}

std::ostream&
Item::print(std::ostream& out) const
{
	out << "##########################" << '\n';
	switch (__auth_type)
	{
		case authentication::EMAIL:
			out << "email: " + __accountDetails.email << std::endl 
				<< "pass: " + __accountDetails.password << std::endl;
			break;
		case authentication::USERNAME:
			out << "uname: " + __accountDetails.username << std::endl 
				<< "key: " + __accountDetails.postiginKey << std::endl;
			break;
		case authentication::UNDEFINED:
			out << "Auth type not defined" << std::endl;
			break;
		default:
			out << "Incorrect auth type defined. Code: \"" << + __auth_type << "\"" << std::endl;
			break;
	}
	out << "##########################" << std::endl << std::endl;

	return out;
}

//SLOTS

void
Item::on_needUpdate()
{
	if (!__api_interactorThread.isRunning()) {
		__api_interactor->moveToThread(&__api_interactorThread);
		__api_interactorThread.start();
	}
}

void
Item::on_itemUpdated(bool success)
{
	if (success) {
		if (this->rating() != -1) {
			emit _itemChanged(this);
		}
	} else {
		/* std::cout << "unsuccess" << std::endl; */
	}
}
