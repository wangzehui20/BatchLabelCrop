#pragma once
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <QString>
#include <QWidget>
#include <QGraphicsItem>

static int clipSize = 256;

class ImageItem : public QWidget {

public:
	ImageItem(QWidget *parent = 0);
	~ImageItem();

	QPointF leftTopPoint;
	QString projection;
	double x_pixel, y_pixel;

	void writeShape(QVector<QPolygonF> polygons, QString path);
	void clipImage(QString dstDir, QString imgPath, QImage img, QVector<QPointF> centers);
	void setProjAndLeftTop(QString path);
	QVector<QPolygonF> readShape(QString path);
	QVector<QPointF> polygon2Point(QPolygonF polygon);
	QPolygonF ogrPolygon2Polygon(OGRPolygon *ogrPolygon);
	QPointF getPolygonCenter(QPolygonF polygon);
	QVector<QPointF> getPolygonCenterSet(QVector<QPolygonF> polygons);
	QVector<QPointF> polygon2Points(QPolygonF polygon);
	
};