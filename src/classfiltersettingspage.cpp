#include "classfiltersettingspage.h"
#include "ui_classfiltersettingspage.h"

#include "application.h"

#include <shv/chainpack/chainpack.h>
#include <shv/coreqt/data/rpcsqlresult.h>

#include <QSettings>

using namespace shv::chainpack;

ClassFilterSettingsPage::ClassFilterSettingsPage(QWidget *parent)
	: Super(parent)
	, ui(new Ui::ClassFilterSettingsPage)
{
	m_caption = tr("Filter");
	ui->setupUi(this);
	ui->buttonBar->hide();

	connect(ui->btH, &QPushButton::clicked, this, [this]() {
		auto *lst = ui->lstClasses;
		for (auto i = 0; i < ui->lstClasses->count(); ++i) {
			auto *item = lst->item(i);
			if (item->text().startsWith('H', Qt::CaseInsensitive)) {
				item->setCheckState(Qt::Checked);
			}
		}
	});
	connect(ui->btD, &QPushButton::clicked, this, [this]() {
		auto *lst = ui->lstClasses;
		for (auto i = 0; i < ui->lstClasses->count(); ++i) {
			auto *item = lst->item(i);
			if (item->text().startsWith('D', Qt::CaseInsensitive)) {
				item->setCheckState(Qt::Checked);
			}
		}
	});
	connect(ui->btClear, &QPushButton::clicked, this, [this]() {
		auto *lst = ui->lstClasses;
		for (auto i = 0; i < ui->lstClasses->count(); ++i) {
			auto *item = lst->item(i);
			item->setCheckState(Qt::Unchecked);
		}
	});
	connect(ui->btInvert, &QPushButton::clicked, this, [this]() {
		auto *lst = ui->lstClasses;
		for (auto i = 0; i < ui->lstClasses->count(); ++i) {
			auto *item = lst->item(i);
			item->setCheckState(item->checkState() == Qt::Checked? Qt::Unchecked: Qt::Checked);
		}
	});
}

ClassFilterSettingsPage::~ClassFilterSettingsPage()
{
	delete ui;
}

namespace {
constexpr auto CLASS_FILTER_ON = "classFilter/enabled";
constexpr auto CLASS_FILTER_CHECKED_CLASSES = "classFilter/checkedClasses";
}

ClassFilterSettingsPage::ClassFilter ClassFilterSettingsPage::checkedClasses()
{
	QSettings settings;
	auto is_class_filter = settings.value(CLASS_FILTER_ON).toBool();
	auto checked_classes = settings.value(CLASS_FILTER_CHECKED_CLASSES).toStringList();
	return {.enabled = is_class_filter, .checkedClasses = checked_classes };
}

void ClassFilterSettingsPage::load()
{
	auto *app = Application::instance();
	if (m_definedClasses.isEmpty()) {
		app->callShvApiMethod("event/currentStage/classes", "table", {}, this,
			[this](const RpcValue &result) {
				auto table = shv::coreqt::data::RpcSqlResult::fromRpcValue(result);
				for (int i = 0; i < table.rows.count(); ++i) {
					auto class_name = table.value(i, "classes.name").toString();
					m_definedClasses << class_name;
				}
				loadClassFilter();
			}
		);
	}
}

void ClassFilterSettingsPage::save()
{
	QSettings settings;
	settings.setValue(CLASS_FILTER_ON, ui->groupBox->isChecked());
	QStringList checked_classes;
	auto *lst = ui->lstClasses;
	for (auto i = 0; i < ui->lstClasses->count(); ++i) {
		auto *item = lst->item(i);
		if (item->checkState() == Qt::Checked) {
			checked_classes << item->text();
		}
	}
	settings.setValue(CLASS_FILTER_CHECKED_CLASSES, checked_classes);
}

void ClassFilterSettingsPage::loadClassFilter()
{
	auto class_filter = checkedClasses();
	ui->groupBox->setChecked(class_filter.enabled);
	auto *lst = ui->lstClasses;
	for (const auto &cn : m_definedClasses) {
		auto *item = new QListWidgetItem(cn);
		auto is_checked = class_filter.checkedClasses.contains(cn);
		item->setCheckState(is_checked? Qt::Checked: Qt::Unchecked);
		lst->addItem(item);
	}
}
