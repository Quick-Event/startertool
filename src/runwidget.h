#ifndef RUNWIDGET_H
#define RUNWIDGET_H

#include <QWidget>

class StartListModel;

namespace Ui {
class RunWidget;
}

class RunWidget : public QWidget
{
	Q_OBJECT

public:
	explicit RunWidget(StartListModel *model, QWidget *parent = nullptr);
	~RunWidget();

	void load(int run_id);
private:
	void save();
private:
	Ui::RunWidget *ui;
	StartListModel *m_model;
	int m_runId = 0;
};

#endif // RUNWIDGET_H
