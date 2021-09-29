#include <qnamespace.h>
#include <sll/base.h>

#include <initializer_list>
#include <string>
#include <algorithm>
#include <vector>
#include <array>
#include <iostream>

using namespace SL;

Stor::Stor(QObject *p) :
	QObject(p),
		__defaultFirstUID(1)
{
	__iUpdater = new (std::nothrow) ItemUpdater(*this);

	connect(&__iUpdaterThread, &QThread::started,
			__iUpdater, &ItemUpdater::start);

	connect(__iUpdater, &ItemUpdater::finished,
			&__iUpdaterThread, &QThread::terminate); //, Qt::DirectConnection

	__iUpdater->moveToThread(&__iUpdaterThread);
	__iUpdaterThread.start();
}

Stor::~Stor()
{
	emit __iUpdater->forseStop();
	__iUpdaterThread.quit();
	__iUpdaterThread.wait();

	if (__iUpdater) {
		delete __iUpdater;
	}
	/* auto iter = this->begin(); */
	/* while (iter != this->end()) { */
	/* 	delete *iter; */
	/* 	iter++; */
	/* } */
}

long
Stor::getItemPos(int id) const
{
	for (size_t i = 0; i < this->size(); i++)
	{
		if (this->at(i)->id() == id) {
			return i;
		}
	}

	return -1;
}

long
Stor::getAvalibleID(void) const //TODO SIMPLIFY IT!
{
	long aId {__defaultFirstUID};

	if (size() > 1) {
		// TODO
		/* std::sort(this->begin(), this->end(), */
		/* 		[] (const Item* first, const Item* second) { return first->id() < second->id(); }); */
		aId = this->back()->id()+1;
	}

	return aId;
}

Item *
Stor::getItem(const int id) const
{
	int pos = getItemPos(id);
	if (pos >= 0)
		return this->at(pos);
	else 
		return nullptr;
}

Item *
Stor::addItem(Item *item)
{
	QObject::connect(item, &Item::_itemChanged, this, &Stor::on_childChanged);
	//add edinting undef item ID
	if (item->id() <= __defaultFirstUID) {
		item->setID(this->getAvalibleID());
	}
	this->push_back(item);
	emit _itemAdded(item);
	return item;
}

Stor *
Stor::rmItem(int id)
{
	//TODO do it itirationaly for >>(sdvig)
	std::swap(this->at(getItemPos(id)), this->back());
	this->pop_back();
	emit _itemRemoved(id); //TODO
	return this;
}

void
Stor::on_childChanged(Item *item)
{
	emit _itemChanged(item);
}

void
Stor::syncItems(void)
{
	for (Item *i : *this)
	{
		if (!i->username().empty()) {
			emit i->_updateItem();
		}
	}
}

bool
Stor::isItemUIDsLinear(void)
{
	long prev = __defaultFirstUID - 1;

	for (auto item : *this) {
		if (!(item->id() == (prev + 1))) { //mv prev++?
			return false;
		}
		prev = item->id();
	}

	return true;
}

bool
Stor::changeUIDsAlign(long to)
{
	__defaultFirstUID = to;

	for (auto item : *this) {
		(*item).setID(to++);
	}

	emit _itemsUIDsAlignReoganized();

	return true;
}
