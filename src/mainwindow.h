#pragma once

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

private:
	Ui::MainWindow *ui;
};

