#ifndef STARTLISTWIDGET_H
#define STARTLISTWIDGET_H

#include <QWidget>

class RpcSqlResultModel;

namespace Ui {
class StartListWidget;
}

class StartListWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StartListWidget(QWidget *parent = nullptr);
	~StartListWidget();
private:
	void reload();
private:
	Ui::StartListWidget *ui;
	RpcSqlResultModel *m_model;
};

#endif // STARTLISTWIDGET_H
