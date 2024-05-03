#include "startlistitemdelegate.h"

#include "rpcsqlresultmodel.h"
#include "application.h"

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>

StartListItemDelegate::StartListItemDelegate(QObject *parent)
	: Super(parent)
{

}

void StartListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	painter->save();

	auto line_height = option.rect.height()	/ 2;
	int letter_width = line_height / 2;
	painter->fillRect(option.rect, Qt::black);
	auto pen = painter->pen();
	//auto brush = painter->brush();
	pen.setColor(Qt::yellow);
	painter->setPen(pen);
	painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
	auto start_time_w = 9 * letter_width;
	{
		auto rect = option.rect;
		rect.translate(start_time_w, 0);
		auto name = index.data(StartListModel::Role::CompetitorName).toString();
		painter->drawText(rect, name);
	}
	{
		auto name = index.data(StartListModel::Role::ClassName).toString();
		auto rect = option.rect;
		rect.moveTop(rect.center().y());
		rect.translate(start_time_w, 0);
		painter->drawText(rect, name);
	}
	{
		auto msecs = index.data(StartListModel::Role::StartTime).toInt();
		auto start00 = Application::instance()->currentStageStart();
		auto start = start00.addMSecs(msecs);
		auto f = painter->font();
		f.setBold(true);
		painter->setFont(f);
		// auto brush = painter->brush();
		// brush.setColor(Qt::white);
		// painter->setBrush(brush);
		auto pen = painter->pen();
		pen.setColor(Qt::white);
		painter->setPen(pen);
		painter->drawText(option.rect, start.toString("hh:mm:ss"));
	}

	painter->restore();
}
