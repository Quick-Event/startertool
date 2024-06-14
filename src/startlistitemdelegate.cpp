#include "startlistitemdelegate.h"

#include "startlistmodel.h"
#include "application.h"

#include <shv/coreqt/log.h>

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
	shvLogFuncFrame();
	painter->save();
	auto is_selected = index.data(StartListModel::Role::IsSelectedRow).toBool();
	auto line_height = option.rect.height()	/ 2;
	int letter_width = line_height / 2;
	int corridor_status_width = letter_width;

	auto start_time = index.data(StartListModel::Role::StartDateTime).toDateTime();
	auto corridor_time = index.data(StartListModel::Role::CorridorTime).toDateTime();

	constexpr auto CORRIDOR1_COLOR = Qt::red;
	constexpr auto CORRIDOR2_COLOR = QColorConstants::Svg::orange;
	constexpr auto CORRIDOR3_COLOR = Qt::green;

	auto corridor = static_cast<StartListModel::CorridorStage>(index.data(StartListModel::Role::Corridor).toInt());

	constexpr auto TEXT_COLOR = Qt::yellow;
	constexpr auto SELECTED_COLOR = Qt::magenta;
	constexpr auto SELECTED_BACKGROUND_COLOR = Qt::darkGray;
	constexpr auto STARTED_TEXT_COLOR = Qt::white;
	constexpr auto BACKGROUND_COLOR = Qt::black;
	constexpr auto STARTED_BACKGROUND_COLOR = Qt::darkGreen;
	constexpr auto CORRIDOR_TIME_COLOR = Qt::yellow;

	auto bg_color = BACKGROUND_COLOR;
	if (is_selected)
		bg_color = SELECTED_BACKGROUND_COLOR;
	if (corridor_time.isValid())
		bg_color = STARTED_BACKGROUND_COLOR;
	painter->fillRect(option.rect, bg_color);
	auto text_color = corridor_time.isValid()? STARTED_TEXT_COLOR: TEXT_COLOR;
	auto pen = painter->pen();
	//auto brush = painter->brush();
	pen.setColor(text_color);
	painter->setPen(pen);
	{
		auto rect = option.rect;
		rect.setWidth(corridor_status_width);
		QColor c;
		if (corridor > StartListModel::CorridorStage::CEarly && corridor < StartListModel::CorridorStage::CStarted) {
			switch (corridor) {
			case StartListModel::CorridorStage::C1: c = CORRIDOR1_COLOR; break;
			case StartListModel::CorridorStage::C2: c = CORRIDOR2_COLOR; break;
			case StartListModel::CorridorStage::C3: c = CORRIDOR3_COLOR; break;
			default: break;
			}
		}
		// painter->drawRect(rect);
		if (c.isValid()) {
			painter->fillRect(rect, c);
		}
	}
	if (is_selected) {
		painter->save();
		auto pen = painter->pen();
		pen.setColor(SELECTED_COLOR);
		pen.setWidth(5);
		painter->setPen(pen);
		painter->drawRect(option.rect);
		painter->restore();
	}
	else {
		painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
	}
	auto time_offset = corridor_status_width;
	auto name_offset = time_offset + 7 * letter_width;
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
		auto card_read_siid = Application::instance()->cardInserted();
		auto rect = option.rect;
		rect.translate(registration_offset, option.rect.height() / 2);
		rect.setRight(option.rect.right() - option.rect.height());
		rect.setHeight(option.rect.height() / 2);
		auto siid = index.data(StartListModel::Role::SiId).toUInt();
		auto text = QString::number(siid);
		if (card_read_siid == siid) {
			painter->fillRect(rect, Qt::yellow);
			auto f = painter->font();
			f.setBold(true);
			painter->setFont(f);
			auto pen = painter->pen();
			pen.setColor(Qt::black);
			painter->setPen(pen);
		}
		painter->drawText(rect, text);
	}
	{
		auto f = painter->font();
		f.setBold(true);
		painter->setFont(f);
		// auto brush = painter->brush();
		// brush.setColor(Qt::white);
		// painter->setBrush(brush);
		auto pen = painter->pen();
		pen.setColor(Qt::white);
		painter->setPen(pen);
		auto rect = option.rect;
		rect.translate(time_offset, 0);
		painter->drawText(rect, start_time.toString("hh:mm:ss"));
	}
	{
		auto rect = option.rect;
		rect.moveTop(rect.center().y());
		rect.translate(time_offset, 0);
		auto pen = painter->pen();
		pen.setColor(CORRIDOR_TIME_COLOR);
		painter->setPen(pen);
		painter->drawText(rect, corridor_time.toString("hh:mm:ss"));
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
			p.fillRect(QRect(QPoint(), px.size()), BACKGROUND_COLOR);
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
