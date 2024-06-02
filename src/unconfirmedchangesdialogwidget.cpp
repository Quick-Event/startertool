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
	connect(ui->btCopy, &QAbstractButton::clicked, this, [this]() {
		ui->textEdit->copy();
	});
}

UnconfirmedChangesDialogWidget::~UnconfirmedChangesDialogWidget()
{
	delete ui;
}

void UnconfirmedChangesDialogWidget::setText(const QString &txt)
{
	ui->textEdit->setPlainText(txt);
}
