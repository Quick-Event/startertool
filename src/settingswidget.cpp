#include "settingswidget.h"
#include "ui_settingswidget.h"

#include "settingspage.h"
#include "stagesettingspage.h"
#include "classfiltersettingspage.h"

#include <shv/coreqt/log.h>

#include <QButtonGroup>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QTimer>


SettingsWidget::SettingsWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::SettingsWidget)
{
	ui->setupUi(this);
	m_buttonGroup = new QButtonGroup(this);
	connect(ui->btBack, &QAbstractButton::clicked, this, [this]() {
		if(int page_index = m_buttonGroup->checkedId(); page_index >= 0) {
			page(page_index)->save();
		}
		deleteLater();
	});
	connect(m_buttonGroup, &QButtonGroup::idToggled, this, [this](int page_index, bool checked) {
		shvDebug() << "id toggled:" << page_index << checked;
		if(checked) {
			ui->stackedWidget->setCurrentIndex(page_index);
			page(page_index)->load();
		}
		else {
			page(page_index)->save();
		}
	});

	addPage(new StageSettingsPage());
	addPage(new ClassFilterSettingsPage());
}

SettingsWidget::~SettingsWidget()
{
	delete ui;
}

SettingsPage *SettingsWidget::page(int page_index)
{
	auto *page = ui->stackedWidget->widget(page_index)->findChild<SettingsPage*>(QString(), Qt::FindDirectChildrenOnly);
	Q_ASSERT(page);
	return page;
}

void SettingsWidget::addPage(SettingsPage *page)
{
	auto caption = page->caption();
	Q_ASSERT(!caption.isEmpty());
	auto *layout = qobject_cast<QBoxLayout*>(ui->buttonsWidget->layout());
	Q_ASSERT(layout);
	int page_index = m_buttonGroup->buttons().count();
	auto *btn = new QPushButton(caption);
	// set widget minimum width to show all buttons, default behavior is to srt width of widget
	// according to width of first button added
#if QT_VERSION_MAJOR >= 6
	ui->buttonsWidget->setMinimumWidth(std::max(ui->buttonsWidget->minimumWidth(), btn->sizeHint().width() + layout->contentsMargins().left() * 5));
#else
	ui->buttonsWidget->setMinimumWidth(std::max(ui->buttonsWidget->minimumWidth(), btn->sizeHint().width() + layout->margin() * 5));
#endif
	btn->setCheckable(true);
	layout->insertWidget(page_index, btn);
	m_buttonGroup->addButton(btn, page_index);
	auto *frame = new QFrame();
	frame->setFrameStyle(QFrame::Box);
	auto *label = new QLabel("  " + caption);
	label->setObjectName("CaptionFrame"); // important for CSS
	auto *ly = new QVBoxLayout(frame);
#if QT_VERSION_MAJOR >= 6
	ly->setContentsMargins({0, 0, 0, 0});
#else
	ly->setMargin(0);
#endif
	ly->addWidget(label);
	ly->addWidget(page);
	ui->stackedWidget->addWidget(frame);
	adjustSize();
	if(page_index == 0) {
		btn->click();
	}
}
