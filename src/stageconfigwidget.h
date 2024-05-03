#ifndef STAGECONFIGWIDGET_H
#define STAGECONFIGWIDGET_H

#include <QWidget>

namespace Ui {
class StageConfigWidget;
}

class StageConfigWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StageConfigWidget(QWidget *parent = nullptr);
	~StageConfigWidget();

private:
	Ui::StageConfigWidget *ui;
};

#endif // STAGECONFIGWIDGET_H
