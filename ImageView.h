#pragma once
#include <QgraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>

class ImageView : public QGraphicsView {
	
	Q_OBJECT

public:
	ImageView(QWidget *parent = 0);
	~ImageView();

	float scaleUp, scaleDown;
	QPoint lastCursorPos;

signals:
	void wheelImg(QPointF viewPos, float scaleIndex);
	void moveImg(QPointF cursorTranslation);
	void linkedView(QPointF point);
	void deleteInitCross();

	public slots:
	void on_wheelImg(QPointF viewPos, float scaleIndex);
	void on_moveImg(QPointF cursorTranslation);

protected:
	//implement scale
	void wheelEvent(QWheelEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
};