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
	using Super = QWidget;
public:
	explicit StartListWidget(QWidget *parent = nullptr);
	~StartListWidget();

	// QWidget interface
protected:
	void resizeEvent(QResizeEvent *event) override;
private:
	void reload();
	void updateHeadersSectionSizes();
private:
	Ui::StartListWidget *ui;
	RpcSqlResultModel *m_model;
};

#endif // STARTLISTWIDGET_H
