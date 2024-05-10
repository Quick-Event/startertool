#include "startlistitemdelegate.h"

#include "rpcsqlresultmodel.h"
#include "application.h"

#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
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
	auto name_offset = 9 * letter_width;
	auto registration_offset = option.rect.width() - option.rect.height() - 8 * letter_width;
	{
		auto rect = option.rect;
		rect.translate(name_offset, 0);
		auto name = index.data(StartListModel::Role::CompetitorName).toString();
		painter->drawText(rect, name);
	}
	{
		auto rect = option.rect;
		rect.translate(registration_offset, 0);
		auto text = index.data(StartListModel::Role::Registration).toString();
		painter->drawText(rect, text);
	}
	{
		auto name = index.data(StartListModel::Role::ClassName).toString();
		auto rect = option.rect;
		rect.moveTop(rect.center().y());
		rect.translate(name_offset, 0);
		painter->drawText(rect, name);
	}
	{
		auto rect = option.rect;
		rect.translate(registration_offset, option.rect.height() / 2);
		auto text = QString::number(index.data(StartListModel::Role::SiId).toInt());
		painter->drawText(rect, text);
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
	{
		auto dt = index.data(StartListModel::Role::CorridorTime).toDateTime();
		auto rect = option.rect;
		rect.moveTop(rect.center().y());
		auto pen = painter->pen();
		pen.setColor(Qt::magenta);
		painter->setPen(pen);
		painter->drawText(rect, dt.toString(Qt::ISODate));
	}
	//{
	//	auto siid = index.data(StartListModel::Role::SiId).toInt();
	//	auto f = painter->font();
	//	f.setBold(true);
	//	painter->setFont(f);
	//	auto pen = painter->pen();
	//	pen.setColor(Qt::cyan);
	//	painter->setPen(pen);
	//	auto rect = option.rect;
	//	rect.moveTop(rect.center().y());
	//	painter->drawText(rect, QString::number(siid));
	//}
	{
		static auto pencil = []() {
			QPixmap px(64, 64);
			QSvgRenderer rnd(QStringLiteral(":/images/pencil.svg"));
			QPainter p(&px);
			rnd.render(&p);
			return px;
		}();
		auto rect = option.rect;
		rect.setLeft(rect.right() - rect.height());
		int d = 4;
		rect.adjust(d, d, -d, -d);
		//painter->translate(rect.topLeft());
		painter->drawPixmap(rect, pencil);
	}
	painter->restore();
}
