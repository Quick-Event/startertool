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
	enum class AutoconnectEnabled {No, Yes};
	explicit LoginWidget(AutoconnectEnabled autoconnect_enbled, QWidget *parent = nullptr);
	~LoginWidget();
private:
	QUrl connectionUrl() const;
	void connectToBroker();
	void checkAutoConnect();
private:
	Ui::LoginWidget *ui;
};

#endif // LOGINWIDGET_H
