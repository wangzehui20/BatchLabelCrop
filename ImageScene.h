#pragma once
#include <qgraphicsscene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QGraphicsPixmapItem>
#include <QList>
#include <QPainter>

class ImageScene : public QGraphicsScene {
	
	Q_OBJECT

public:
	ImageScene();
	~ImageScene();

	//buffer polygon and item
	QPolygonF bufferPolygon; 
	QGraphicsItem *bufferItem;

	QPen bufferPen;
	QBrush bufferBrush;
	QPen initPen;
	QBrush initBrush;

	bool is_paint;
	QVector<QPolygonF> polygons;
	QGraphicsPixmapItem *imgBaseItem;
	QVector<QGraphicsItem*> allItems;
	QGraphicsLineItem *hCross;
	QGraphicsLineItem *vCross;

	void removeBufferItem();

	//compared with initializePolygons(), this add shapefile into polygons
	void displayShp(QVector<QPolygonF> shpPolygons);
	void removePolygonsAndItems();

	public slots:
	void on_linkedView(QPointF point);
	void on_deleteInitCross();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
};