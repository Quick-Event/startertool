#ifndef UNCONFIRMEDCHANGESDIALOGWIDGET_H
#define UNCONFIRMEDCHANGESDIALOGWIDGET_H

#include <QWidget>

namespace Ui {
class UnconfirmedChangesDialogWidget;
}

class UnconfirmedChangesDialogWidget : public QWidget
{
	Q_OBJECT

public:
	explicit UnconfirmedChangesDialogWidget(QWidget *parent = nullptr);
	~UnconfirmedChangesDialogWidget();

	void setText(const QString &txt);
private:
	Ui::UnconfirmedChangesDialogWidget *ui;
};

#endif // UNCONFIRMEDCHANGESDIALOGWIDGET_H
