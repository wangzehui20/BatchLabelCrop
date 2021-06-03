#include "ImageView.h"

#include <QScrollBar>

float cross = 12;
float crossSize = 2;

ImageView::ImageView(QWidget *parent) {
	//set view background and its border transparent
	setStyleSheet("background: transparent");

	this->setParent(parent);
	//window motion without cursor
	this->setDragMode(QGraphicsView::NoDrag);
	//set view as child window
	this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	this->setCursor(Qt::CrossCursor);
	setMouseTracking(true);

	//close view scrollbar
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
	scaleUp = 1.2;
	scaleDown = 1 / 1.2;
}

ImageView::~ImageView() {

}

void ImageView::mousePressEvent(QMouseEvent *event){
	if (event->button() == Qt::MidButton){
		this->setCursor(Qt::OpenHandCursor);
		this->setDragMode(QGraphicsView::ScrollHandDrag);
		//view coordinate
		lastCursorPos = event->pos();
	}
	QGraphicsView::mousePressEvent(event);
}

void ImageView::wheelEvent(QWheelEvent *event) {
	float scaleIndex;
	QPointF viewPos = event->pos();
	emit deleteInitCross();
	emit linkedView(this->mapToScene(viewPos.toPoint()));
	event->delta() > 0 ? (scaleIndex = scaleUp) : (scaleIndex = scaleDown);

	//change linked cross-cusor as soon as wheeling  
	cross /= scaleIndex;
	crossSize /= scaleIndex;

	emit wheelImg(viewPos, scaleIndex);

	on_wheelImg(viewPos, scaleIndex);
}

void ImageView::on_wheelImg(QPointF viewPos, float scaleIndex) {
	//scene coordinate
	QPointF scenePos = this->mapToScene(viewPos.toPoint());
	this->scale(scaleIndex, scaleIndex);

	//image in view has scaled
	QPointF scaledViewPos = this->matrix().map(scenePos);
}

void ImageView::mouseMoveEvent(QMouseEvent *event) {
	//emit signal to delete initial existed cross item
	emit deleteInitCross();
	//emit cursor point to scene
	emit linkedView(this->mapToScene(event->pos()));

	if (this->dragMode() == QGraphicsView::ScrollHandDrag) {
		QPoint currentCursorPos = event->pos();
		QPointF cursorTranslation = mapToScene(currentCursorPos) - mapToScene(lastCursorPos);
		emit moveImg(cursorTranslation);
		on_moveImg(cursorTranslation);
		lastCursorPos = currentCursorPos;
	}
	QGraphicsView::mouseMoveEvent(event);
}

void ImageView::on_moveImg(QPointF cursorTranslation) {
	cursorTranslation *= this->transform().m11();

	//call scale method
	this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	//translation need to scale
	this->centerOn(this->mapToScene(QPoint(this->viewport()->rect().width() / 2 - cursorTranslation.x(),
		this->viewport()->rect().height() / 2 - cursorTranslation.y())));
	this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}

void ImageView::mouseReleaseEvent(QMouseEvent *event){
	if (event->button() == Qt::MidButton){
		this->setDragMode(QGraphicsView::NoDrag);
		this->setCursor(Qt::CrossCursor);
	}
	QGraphicsView::mouseReleaseEvent(event);
}