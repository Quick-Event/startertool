#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "application.h"

#include <shv/coreqt/log.h>
#include <shv/core/exception.h>
#include <shv/iotqt/rpc/clientconnection.h>

#include <QSettings>

namespace rpc = shv::iotqt::rpc;

MainWindow::MainWindow(QWidget *parent) :
	Super(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}


