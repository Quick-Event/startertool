#ifndef STARTLISTTABLEVIEW_H
#define STARTLISTTABLEVIEW_H

#include <QTableView>
#include <QElapsedTimer>

class StartListTableView : public QTableView
{
	Q_OBJECT
	using Super = QTableView;
public:
	StartListTableView(QWidget *parent = nullptr);

	Q_SIGNAL void editButtonPressed(int run_id);
	Q_SIGNAL void corridorTimeButtonPressed(int run_id);
	Q_SIGNAL void manualScroll();
protected:
	bool event(QEvent *event) override;
private:
	void onMousePress(const QPoint &pos);
private:
	struct TouchState
	{
		QPointF startPos;
		bool isClick = true;
		int sbPos = 0;
		//QElapsedTimer m_durationTimer;
	};
	std::optional<TouchState> m_touchState;
};

#endif // STARTLISTTABLEVIEW_H
