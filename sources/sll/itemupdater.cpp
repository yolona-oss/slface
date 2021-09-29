#include <qnamespace.h>
#include <sll/base.h>
#include <QThread>
#include <QTimer>

#include <iostream>
#include <algorithm>

using namespace SL;

UpdaterQueue::UpdaterQueue(Stor& stor) :
	__stor(stor),
	queue(0)
{
	connect(&stor, &Stor::_itemAdded,
			this, &UpdaterQueue::on_itemAdded);
	connect(&stor, &Stor::_itemRemoved,
			this, &UpdaterQueue::on_itemRemoved);
	connect(&stor, &Stor::_itemChanged,
			this, &UpdaterQueue::on_itemChanged);
	connect(&stor, &Stor::_itemsUIDsAlignReoganized,
			this, &UpdaterQueue::on_reorganized);

	ItemState *is;
	for (auto item : stor) {
		connect(item->api(), &API_Interactor::finishedForItem,
				this, &UpdaterQueue::on_interactorFinished);

		is = new ItemState(*item);
		queue.push_back(is);
	}
}

UpdaterQueue::~UpdaterQueue()
{
}

long
UpdaterQueue::findItemPos(Item& item)
{
	int i= 0;
	for (auto is : queue) {
		if (is->item == item) {
			return i;
		}
		i++;
	}

	throw "No item here!!!";
}

//moving freshly updated item to end of thre queue with status Wait
void
UpdaterQueue::on_interactorFinished(Item& item)
{
	std::reverse(queue.begin(), queue.end());
	/* std::rotate(queue.begin(), queue.begin() + 1, queue.end()); */
	std::rotate(queue.begin(), queue.end() - findItemPos(item), queue.end());
}

void
UpdaterQueue::on_itemAdded(Item*)
{
}

void
UpdaterQueue::on_itemRemoved(long id)
{
}

void
UpdaterQueue::on_itemChanged(Item*)
{
}

void
UpdaterQueue::on_reorganized()
{
	//reconfig queue
}

ItemUpdater::ItemUpdater(Stor& stor, QObject *p) :
	QObject(p),
		__stor(stor),
		__queue(new UpdaterQueue(stor)),
		__timer(new QTimer(this))
{
	connect(__timer, &QTimer::timeout, this, &ItemUpdater::doIteration, Qt::DirectConnection); //Qt::BlockingQueuedConnection
	connect(this, &ItemUpdater::forseStop, this, &ItemUpdater::stop); //mb lymbda?
}

ItemUpdater::~ItemUpdater()
{
	delete __timer;
	delete __queue;
}

void
ItemUpdater::start(void)
{
	if (__firstTime) {
		__firstTime = false;
		doIteration();
	}
	__timer->start(__interval);
}

void
ItemUpdater::stop(void)
{
	__timer->stop();
	emit finished();
}

void
ItemUpdater::doIteration(void)
{
	int i = 0;
	for (auto item : __stor) {
		if (item->api()->isInteractionPossible()) {
			if (i < 90) {
				emit item->_updateItem();
				i++;
			} else {
				//add to queue
			}
		}
	}

	/* std::cout << "iteration :)" << std::endl; */
}

void
ItemUpdater::on_forseUpdate(void)
{
	//do update for entyre Stor
}

void
ItemUpdater::on_outOfTurnRequest(Item *item)
{
	//do update for single item
	//emit _updatedItem(item);
}
