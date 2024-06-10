#ifndef STARTLISTTABLEVIEW_H
#define STARTLISTTABLEVIEW_H

#include <QTableView>

class QGestureEvent;
class QSwipeGesture;

class StartListTableView : public QTableView
{
	Q_OBJECT
	using Super = QTableView;
public:
	StartListTableView(QWidget *parent = nullptr);

	Q_SIGNAL void editButtonPressed(int run_id);
	Q_SIGNAL void corridorTimeButtonPressed(int run_id);
protected:
	void mousePressEvent(QMouseEvent *event) override;
	bool event(QEvent *event) override;

private:
	bool gestureEvent(QGestureEvent *event);
	void swipeTriggered(QSwipeGesture*gesture);
};

#endif // STARTLISTTABLEVIEW_H
