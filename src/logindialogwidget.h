#pragma once

#include <QWidget>
#include <QUrl>

namespace Ui {
class LoginDialogWidget;
}

class LoginDialogWidget : public QWidget
{
	Q_OBJECT

public:
	enum class AutoconnectEnabled {No, Yes};
	explicit LoginDialogWidget(AutoconnectEnabled autoconnect_enbled, QWidget *parent = nullptr);
	~LoginDialogWidget();
private:
	QUrl connectionUrl() const;
	void connectToBroker();
	void checkAutoConnect();
private:
	Ui::LoginDialogWidget *ui;
};

