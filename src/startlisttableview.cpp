#include "startlisttableview.h"
#include "startlistmodel.h"

#include <shv/coreqt/log.h>

#include <QMouseEvent>

StartListTableView::StartListTableView(QWidget *parent)
	: Super(parent)
{

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
