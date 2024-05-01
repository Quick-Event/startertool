#include "startlistitemdelegate.h"

#include "rpcsqlresultmodel.h"

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>

StartListItemDelegate::StartListItemDelegate(QObject *parent)
	: Super(parent)
{

}

void StartListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	painter->fillRect(option.rect, Qt::black);
	auto pen = painter->pen();
	//auto brush = painter->brush();
	pen.setColor(Qt::yellow);
	painter->setPen(pen);
	painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
	auto *model = qobject_cast<const StartListModel*>(index.model());
	Q_ASSERT(model);
	{
		auto name = model->columnValue(index.row(), StartListModel::Column::CompetitorName).toString();
		painter->drawText(option.rect, name);
	}
	{
		auto name = model->columnValue(index.row(), StartListModel::Column::ClassName).toString();
		auto rect = option.rect;
		rect.moveTop(rect.center().y());
		painter->drawText(rect, name);
	}
}
