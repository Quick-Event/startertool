#include "startlisttableview.h"
#include "startlistmodel.h"

#include <shv/coreqt/log.h>

#include <QMouseEvent>
#include <QScrollBar>
#include <QHeaderView>
#include <QPainter>

StartListTableView::StartListTableView(QWidget *parent)
	: Super(parent)
{
	setAttribute(Qt::WA_AcceptTouchEvents);
	viewport()->installEventFilter(this);
}

void StartListTableView::onMousePress(const QPoint &pos)
{
	//shvDebug() << "mousePressEvent:" << event->type();
	auto row = rowAt(pos.y());
	auto index = model()->index(row, 0);
	auto cell_rect = visualRect(index);
	auto run_id = index.data(StartListModel::RunId).toInt();
	auto in_button = pos.x() > (cell_rect.right() - cell_rect.height());
	if (in_button) {
		//shvInfo() << row << pos.x() << viewport()->geometry().size() << visualRect(index);
		emit editButtonPressed(run_id);
		return;
	}
	auto in_times = pos.x() < (cell_rect.height() * 2);
	if (in_times) {
		//shvInfo() << row << pos.x() << viewport()->geometry().size() << visualRect(index);
		emit corridorTimeButtonPressed(run_id);
		return;
	}
}

bool StartListTableView::event(QEvent *event)
{
	if (event->type() == QEvent::TouchBegin) {
		auto *te = static_cast<QTouchEvent*>(event);
		if (te->pointCount() == 1) {
			shvDebug() << "touch begin:" << te->point(0).position();
			m_touchState = TouchState {
					.startPos = te->point(0).position(),
					.sbPos = verticalScrollBar()->value(),
					// .m_durationTimer.start(),
			};
			event->setAccepted(true);
			return true;
		}
	}
	if (event->type() == QEvent::TouchUpdate) {
		auto *te = static_cast<QTouchEvent*>(event);
		if (te->pointCount() == 1 && m_touchState.has_value()) {
			shvDebug() << "touch update:" << te->point(0).position();
			auto &ts = m_touchState.value();
			auto point = te->point(0);
			auto curr_pos = point.position();
			auto dy1 = (ts.startPos - curr_pos).y();
			auto constexpr EPS = 5;
			if (-EPS < dy1 && dy1 < EPS) {
				ts.isClick = true;
			}
			else {
				ts.isClick = false;
				auto *sb = verticalScrollBar();
				auto sb_h = sb->maximum() - sb->minimum();
				auto wp_h = verticalHeader()->defaultSectionSize() * model()->rowCount();
				wp_h -= viewport()->size().height();
				auto dy2 = dy1 * sb_h / wp_h;
				//shvDebug() << "widget:" << geometry() << "viewport:" << viewport()->geometry();
				//shvDebug() << "sb range:" << sb_h << "ratio:" << (wp_h / sb_h);
				//shvDebug() << ts.startPos << "----------->" << ts.currPos;
				//shvDebug() << "touch, scrool by:" << dy1;
				//shvDebug() << "sb, scrool by:" << dy2;
				//shvDebug() << ts.sbPos << "============>" << (ts.sbPos + dy2);
				sb->setValue(ts.sbPos + dy2);
				emit manualScroll();
			}
			event->setAccepted(true);
			// update();
			return true;
		}
	}
	if (event->type() == QEvent::TouchEnd) {
		auto *te = static_cast<QTouchEvent*>(event);
		if (te->pointCount() == 1 && m_touchState.has_value()) {
			shvDebug() << "touch end:" << te->point(0).position();
			auto &ts = m_touchState.value();
			if (ts.isClick) {
				// click
				onMousePress(ts.startPos.toPoint());
			}
			else {

			}
			//shvDebug() << "velo:" << te->point(0).velocity();
			event->setAccepted(true);
			m_touchState = {};
			return true;
		}
	}
	return Super::event(event);
}

bool StartListTableView::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::MouseButtonPress) {
		auto *me = static_cast<QMouseEvent*>(event);
		if (me->button() == Qt::LeftButton) {
			//shvInfo() << "left mouse event:" << event->type();
			onMousePress(me->pos());
			return true;
		}
	}
	return false;
}


