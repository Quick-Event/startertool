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

	void setAutoConnect(bool on);
private:
	QUrl connectionUrl() const;
	void connectToBroker();
	void checkAutoConnect();
private:
	Ui::LoginWidget *ui;
};

#endif // LOGINWIDGET_H
