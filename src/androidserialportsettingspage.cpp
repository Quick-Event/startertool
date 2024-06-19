#include "androidserialportsettingspage.h"
#include "ui_androidserialportsettingspage.h"

AndroidSerialPortSettingsPage::AndroidSerialPortSettingsPage(QWidget *parent)
	: Super(tr("Serial port"), parent)
	, ui(new Ui::AndroidSerialPortSettingsPage)
{
	ui->setupUi(this);
}

AndroidSerialPortSettingsPage::~AndroidSerialPortSettingsPage()
{
	delete ui;
}
