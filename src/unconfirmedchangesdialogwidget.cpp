#include "unconfirmedchangesdialogwidget.h"
#include "ui_unconfirmedchangesdialogwidget.h"

UnconfirmedChangesDialogWidget::UnconfirmedChangesDialogWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::UnconfirmedChangesDialogWidget)
{
	ui->setupUi(this);
	connect(ui->btBack, &QAbstractButton::clicked, this, [this]() {
		deleteLater();
	});
}

UnconfirmedChangesDialogWidget::~UnconfirmedChangesDialogWidget()
{
	delete ui;
}
