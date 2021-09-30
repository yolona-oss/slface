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

	sharedStandbyQueue = new std::vector<Item*>;
	createQueue();
}

UpdaterQueue::~UpdaterQueue()
{
	delete sharedStandbyQueue;
}

void
UpdaterQueue::createQueue(void)
{
	ItemState *is;
	for (auto item : __stor) {
        connect(item->api(), &API_Interactor::finishedForItem,
        		this, &UpdaterQueue::on_interactorFinished);

		is = new ItemState(item);
		queue.push_back(is);
	}
}

void
UpdaterQueue::clearQueue(void)
{
	//TODO TEST AFTER. delete or not itemstates????
	for (auto is : queue) {
		delete is;
	}
	queue.clear();
}

long
UpdaterQueue::findItemPos(Item *item)
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

//API_interactor finished
//moving freshly updated item to end of thre queue with status Wait
void
UpdaterQueue::on_interactorFinished(Item *item)
{
	std::reverse(queue.begin(), queue.end());
	/* std::rotate(queue.begin(), queue.begin() + 1, queue.end()); */
	std::rotate(queue.begin(), queue.end() - findItemPos(item), queue.end());
	std::reverse(queue.begin(), queue.end());
}

//add item to begin of queue
void
UpdaterQueue::on_itemAdded(Item *item)
{
	ItemState *is = new ItemState(item);
	queue.insert(queue.begin(), is);
}

void
UpdaterQueue::on_itemRemoved(long id)
{
	/* queue.erace(std::next(queue.next() + */ 
}

void
UpdaterQueue::on_itemChanged(Item*)
{
}

void
UpdaterQueue::on_reorganized()
{
	//reconfig queue
	clearQueue();
	createQueue();
}

bool
UpdaterQueue::isTimeToUpdate(ItemState *iState)
{
	using namespace std::chrono;
	auto now = steady_clock::now();

	if (duration_cast<milliseconds>(now - iState->lastUpdateTime()) >= G_GET_REQ_GAP_TIME ||
			iState->itsNew()) {
		iState->setLastUpdateTime(steady_clock::now()); //update timer
		return true;
	} else {
		return false;
	}
}

std::vector<Item*> *
UpdaterQueue::getStandby(int count)
{
	//check item for mustUpdate
	//make returning ItemStates status to wating
	
	sharedStandbyQueue->clear();

	for (auto istate : queue) {
		if (isTimeToUpdate(istate)) {
			istate->state = ItemState::Standby;
		}
		if (count > 0 && !istate->item->username().empty() &&
				istate->state == ItemState::Standby)
		{
			istate->state = ItemState::Wait;
			sharedStandbyQueue->push_back(istate->item);
			count--;
		}
	}

	return sharedStandbyQueue;
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
	/* int i = 0; */
	/* for (auto item : __stor) { */
	/* 	if (item->api()->isInteractionPossible()) { */
	/* 		if (i < 10) { */
	/* 			emit item->_updateItem(); */
	/* 			i++; */
	/* 		} else { */
	/* 			//add to queue */
	/* 		} */
	/* 	} */
	/* } */

	//1 queue get standby items with count pred
	//2 start updaters in this items(queue after this precess these item with execute state)
	//3 go to sleep
	
	for (auto item : *__queue->getStandby(__maxUpdaters)) {
		item->_updateItem();
	}
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
