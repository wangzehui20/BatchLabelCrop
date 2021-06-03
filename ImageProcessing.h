#pragma once

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QDragEnterEvent>

#include "ui_ImageProcessing.h"
#include "ImageWithProjection.h"
#include "ImageView.h"
#include "ImageScene.h"
#include "ImageItem.h"

class ImageProcessing : public QMainWindow
{
	Q_OBJECT

private:
	Ui::ImageProcessingClass ui;

	ImageView *view1;
	//reference window
	ImageView *view2;

public:
	ImageProcessing(QWidget *parent = Q_NULLPTR);

	ImageScene *scene1;
	ImageScene *scene2;
	ImageItem item;
	QList<QUrl> urls;
	int currentImgIndex;
	bool is_trueColor;
	bool is_paint;

	QImage getImage(QString path);
	void setBackGround();
	void setViewHLayout();
	void displayImage(QString path, ImageView *view, ImageScene *scene);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void updateWindow();
	
	//if alter paint mode, it clues
	void labelText();
	QStringList getDirectory(const QString path, bool is_img=true);
	void initFront();
	QString saveShpFile();
	bool is_proj(QString path);
	void windowLink(ImageView *view1, ImageView *view2, ImageScene *scene1, ImageScene *scene2);

public slots:
	void lastImg();
	void nextImg();
	void saveShp();
	void openShp();
	void clipImg();
	void isPaint();
	void removeShp();
	void transImg();
};
