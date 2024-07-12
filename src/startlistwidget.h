#ifndef STARTLISTWIDGET_H
#define STARTLISTWIDGET_H

#include <QWidget>

class StartListModel;

namespace Ui {
class StartListWidget;
}

class StartListWidget : public QWidget
{
	Q_OBJECT
	using Super = QWidget;
public:
	explicit StartListWidget(QWidget *parent = nullptr);
	~StartListWidget();

	Q_SIGNAL void manualScroll();

	StartListModel *startListModel() { return m_model; }

	void setSelectedRow(std::optional<int> row);
	void scrollToStartTime(const QDateTime &tm);
	void onCardInserted(unsigned siid);
protected:
	void resizeEvent(QResizeEvent *event) override;
private:
	void applySettings();
	void reload();
	void updateHeadersSectionSizes();
private:
	Ui::StartListWidget *ui;
	StartListModel *m_model;
};

#endif // STARTLISTWIDGET_H
