#pragma once

#include <necrologlevel.h>
#include <shv/iotqt/rpc/clientconnection.h>

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	using Super = QMainWindow;

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

	void showDialogWidget(QWidget *widget);
	void showError(const QString &msg, NecroLogLevel level);
	void hideError() { showError({}, NecroLogLevel::Invalid); }
private:
	void initCardReader();
private:
	Ui::MainWindow *ui;
	struct MatchContext
	{
		QList<int> matchedRows;
		int currentMatchedRow = 0;
	};
	MatchContext m_matchContext;
};

