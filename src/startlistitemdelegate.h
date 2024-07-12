#ifndef STARTLISTITEMDELEGATE_H
#define STARTLISTITEMDELEGATE_H

#include <QStyledItemDelegate>

class QStyleOptionViewItem;
class QPainter;
class QModelIndex;

class StartListItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	using Super = QStyledItemDelegate;
public:
	StartListItemDelegate(QObject *parent = nullptr);
	~StartListItemDelegate() override = default;
public:
	void setRelativeStartTime(bool on) { m_relativeStartTime = on; }
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
private:
	bool m_relativeStartTime = false;
};

#endif // STARTLISTITEMDELEGATE_H
