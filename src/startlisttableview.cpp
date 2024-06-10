#include "startlisttableview.h"
#include "startlistmodel.h"

#include <shv/coreqt/log.h>

#include <QMouseEvent>
#include <QGestureEvent>
#include <QSwipeGesture>

StartListTableView::StartListTableView(QWidget *parent)
	: Super(parent)
{
	grabGesture(Qt::SwipeGesture);
}

void StartListTableView::mousePressEvent(QMouseEvent *event)
{
	auto pos = event->pos();
	auto row = rowAt(pos.y());
	auto index = model()->index(row, 0);
	auto cell_rect = visualRect(index);
	auto run_id = index.data(StartListModel::RunId).toInt();
	auto in_button = pos.x() > (cell_rect.right() - cell_rect.height());
	if (in_button) {
		//shvInfo() << row << pos.x() << viewport()->geometry().size() << visualRect(index);
		emit editButtonPressed(run_id);
		event->accept();
		return;
	}
	auto in_times = pos.x() < (cell_rect.height() * 2);
	if (in_times) {
		//shvInfo() << row << pos.x() << viewport()->geometry().size() << visualRect(index);
		emit corridorTimeButtonPressed(run_id);
		event->accept();
		return;
	}
	Super::mousePressEvent(event);
}

bool StartListTableView::event(QEvent *event)
{
	if (event->type() == QEvent::Gesture)
		return gestureEvent(static_cast<QGestureEvent*>(event));
	return QWidget::event(event);
}

bool StartListTableView::gestureEvent(QGestureEvent *event)
{
	shvInfo() << "gestureEvent():" << event;
	if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
		swipeTriggered(static_cast<QSwipeGesture *>(swipe));
	return true;
}

void StartListTableView::swipeTriggered(QSwipeGesture *gesture)
{
	if (gesture->state() == Qt::GestureFinished) {
		if (gesture->verticalDirection() == QSwipeGesture::Up) {
			shvInfo() << "UP";
		}
		else if (gesture->verticalDirection() == QSwipeGesture::Down) {
			shvInfo() << "DOWN";
		}
		else {
			shvInfo() << "other";
		}
		update();
	}
}
