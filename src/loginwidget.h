#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QUrl>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
	Q_OBJECT

public:
	explicit LoginWidget(QWidget *parent = nullptr);
	~LoginWidget();

private:
	QUrl connectionUrl() const;
	void connectToBroker();
private:
	Ui::LoginWidget *ui;
};

#endif // LOGINWIDGET_H
