#include "ImageScene.h"

extern float cross;
extern float crossSize;

ImageScene::ImageScene() {
	is_paint = false;
	imgBaseItem = NULL;
	bufferItem = NULL;

	bufferPen = QPen(QBrush(QColor(253, 130, 124)), 0);
	bufferBrush = QBrush(QColor(169, 169, 169, 80));
	initPen = QPen(QBrush(QColor(69, 137, 148)), 0);
	initBrush = QBrush(QColor(248, 147, 29, 80));

	hCross = new QGraphicsLineItem;
	vCross = new QGraphicsLineItem;
}

ImageScene::~ImageScene() {

}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	if (!event->isAccepted()) {
		if (event->button() == Qt::RightButton && is_paint == true) {
			QGraphicsItem *itemToRemove = this->itemAt(event->scenePos(), this->items()[0]->transform());
			//not remove base image
			if (itemToRemove == imgBaseItem) {
				itemToRemove = NULL;
			}
			if (itemToRemove != NULL) {
				this->removeItem(itemToRemove);
				int removeIndex = allItems.indexOf(itemToRemove);
				//remove polygon
				polygons.remove(removeIndex);
				//remove this polygon in Item
				allItems.remove(removeIndex);
				delete itemToRemove;
				this->update();
			}
		}
		else if (event->button() == Qt::LeftButton && this->items().size() > 0) {
			//whether paint
			if (is_paint == true) {
				QPointF currentScenePos = event->scenePos();
				if (bufferPolygon.isEmpty()) {
					bufferPolygon.append(currentScenePos);
				}
				else {
					bufferPolygon.append(currentScenePos);
					removeBufferItem();
					bufferItem = (QGraphicsItem *)(this->addPolygon(bufferPolygon, bufferPen, bufferBrush));
				}
				this->update();
			}
		}
	}
	QGraphicsScene::mousePressEvent(event);
}

void ImageScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
	if (this->items().size() > 0) {
		if (!bufferPolygon.isEmpty() && is_paint == true) {
			polygons.append(bufferPolygon);
			removeBufferItem();
			allItems.append((QGraphicsItem*)this->addPolygon(bufferPolygon, bufferPen, bufferBrush));
			bufferPolygon.clear();
		}
	this->update();	
	}
	QGraphicsScene::mousePressEvent(event);
}

void ImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	if (this->items().size() > 0) {
		if (!bufferPolygon.isEmpty() && is_paint == true) {
			QPointF currentScenePos = event->scenePos();
			QPolygonF tempPolygon(bufferPolygon);
			tempPolygon.append(currentScenePos);
			removeBufferItem();
			bufferItem = (QGraphicsItem *)(this->addPolygon(tempPolygon, bufferPen, bufferBrush));
			tempPolygon.clear();
		}
		this->update();
	}
	QGraphicsScene::mouseMoveEvent(event);
}

void ImageScene::removeBufferItem(){
	if (bufferItem != NULL){
		this->removeItem(bufferItem);
		delete bufferItem;
		bufferItem = NULL;
	}
}

void ImageScene::removePolygonsAndItems() {
	int num = allItems.size();
	if (num == 0) return;
	while (num != 0) {
		this->removeItem(allItems[num - 1]);
		--num;
	}
	polygons.clear();
	allItems.clear();
	this->update();
}

void ImageScene::on_linkedView(QPointF point) {
	if (hCross != NULL) {
		this->removeItem(hCross);
		this->removeItem(vCross);
	}
	QGraphicsLineItem *hLine = new QGraphicsLineItem(point.x() - cross, point.y(), point.x() + cross, point.y());
	/*hLine->setPen(QPen(Qt::red, crossSize));*/
	hLine->setPen(QPen(QColor(138,43,226), crossSize));
	hCross = hLine;
	QGraphicsLineItem *vLine = new QGraphicsLineItem(point.x(), point.y() - cross, point.x(), point.y() + cross);
	/*vLine->setPen(QPen(Qt::red, crossSize));*/
	vLine->setPen(QPen(QColor(138, 43, 226), crossSize));
	vCross = vLine;
	this->addItem(hLine);
	this->addItem(vLine);
	this->update();
	//update cross immediately
	invalidate();
}

void ImageScene::on_deleteInitCross() {
	if (hCross != NULL) {
		this->removeItem(hCross);
		this->removeItem(vCross);
	}
	this->update();
}

void ImageScene::displayShp(QVector<QPolygonF> shpPolygons) {
	for (int i = 0; i < shpPolygons.size(); ++i) {
		polygons.append(shpPolygons[i]);
		allItems.append((QGraphicsItem*)this->addPolygon(polygons[i], initPen, initBrush));
	}
	this->update();
}