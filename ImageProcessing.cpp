#include "ImageProcessing.h"

#include <QMimeData>
#include <QUrl>
#include <QDir>
#include <qlayout.h>
#include <QFileDialog>
#include <QTime>
#include <QLabel>

ImageProcessing::ImageProcessing(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->setWindowIcon(QIcon("./Resources/images/logo.jpg"));
	view1 = new ImageView(this);
	view2 = new ImageView(this);

	is_trueColor = true;
	//default mode is Browse
	is_paint = false;

	//update signal-slot regime
	this->updateWindow();

	//implement drag event
	this->setAcceptDrops(true);

	setMouseTracking(true);
	//set window top-right button
	setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
	setFocusPolicy(Qt::StrongFocus);
	setBackGround();
	initFront();

	//set view layout
	setViewHLayout();
	currentImgIndex = 0;

	//respond wheelEvent
	connect(view1, SIGNAL(wheelImg(QPointF, float)), view2, SLOT(on_wheelImg(QPointF, float)));
	connect(view2, SIGNAL(wheelImg(QPointF, float)), view1, SLOT(on_wheelImg(QPointF, float)));

	//respond moveEvent
	connect(view1, SIGNAL(moveImg(QPointF)), view2, SLOT(on_moveImg(QPointF)));
	connect(view2, SIGNAL(moveImg(QPointF)), view1, SLOT(on_moveImg(QPointF)));

	connect(ui.actionLast, SIGNAL(triggered()), this, SLOT(lastImg()));
	connect(ui.actionNext, SIGNAL(triggered()), this, SLOT(nextImg()));
	connect(ui.actionOpen_2, SIGNAL(triggered()), this, SLOT(openShp()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(saveShp()));
	connect(ui.actionClip, SIGNAL(triggered()), this, SLOT(clipImg()));
	connect(ui.actionBrowse_Paint, SIGNAL(triggered()), this, SLOT(isPaint()));
	connect(ui.actionRemove, SIGNAL(triggered()), this, SLOT(removeShp()));
	connect(ui.actionTrue_False_Color, SIGNAL(triggered()), this, SLOT(transImg()));
}

void ImageProcessing::initFront() {
	/*ui.menuBar->setStyleSheet("QMenuBar{background-color:white}");*/
}

void ImageProcessing::isPaint() {
	is_paint = !is_paint;
	scene1->is_paint = is_paint;
	labelText();
}

void ImageProcessing::removeShp() {
	scene1->removePolygonsAndItems();
}

/*
return image or remote sensing image
*/
QImage ImageProcessing::getImage(QString path){
	if (path == NULL) {
		QMessageBox::about(NULL, "Error", "Import image path is NULL");
	}

	//judge that whether image is remote sensing image
	GDALDataset *tempDataset = (GDALDataset*)GDALOpen(path.toStdString().c_str(), GA_ReadOnly);
	int imgBandNum = tempDataset->GetRasterCount();

	if (this->is_proj(path)) {
		ImageWithProjection imgWithProj;
		if (is_trueColor == false) {
			imgWithProj.defineBand(false, 3);
		}
		else {
			imgBandNum > 1 ? imgWithProj.defineBand(true, 3) : imgWithProj.defineBand(true, 1);
		}
		Mat imgMat(imgWithProj.image2Mat(path));
		QImage image(imgMat.data, imgMat.cols, imgMat.rows, imgMat.step, QImage::Format_RGB888);
		return image;
	}
	else {
		QImage image(path);
		return image;
	}
}

//set background
void ImageProcessing::setBackGround(){
	//main widget of window. centralWidget, menuBar and  statusBar
	ui.centralWidget->setAutoFillBackground(true);
	ui.menuBar->setAutoFillBackground(true);
	ui.statusBar->setAutoFillBackground(true);

	QPalette centralWidgetPalette = ui.centralWidget->palette();
	QPalette menuBarPalette = ui.menuBar->palette();
	QPalette statusBarPalette = ui.statusBar->palette();

	centralWidgetPalette.setColor(QPalette::Background, QColor(39,39,39));
	menuBarPalette.setColor(QPalette::Background, QColor(39, 39, 39));
	statusBarPalette.setColor(QPalette::Background, QColor(39, 39, 39));

	ui.centralWidget->setPalette(centralWidgetPalette);
	ui.menuBar->setPalette(menuBarPalette);
	ui.statusBar->setPalette(statusBarPalette);

	//load background image
	/*ui.centralWidget->setAutoFillBackground(true);
	QPixmap backGroundImage("./image/background.jpg");
	QPalette backGroundPalette = ui.centralWidget->palette();
	backGroundPalette.setBrush(QPalette::Background, backGroundImage.scaled(this->width(), this->height()));
	ui.centralWidget->setPalette(backGroundPalette);*/
}

/*
drag and drop image file
*/
void ImageProcessing::dragEnterEvent(QDragEnterEvent *event) {
	event->acceptProposedAction();
}

void ImageProcessing::dropEvent(QDropEvent *event) {
	this->updateWindow();
	urls = event->mimeData()->urls();
	if (urls.size() == 1) {
		view2->hide();
	}
	QString firstFileName = urls[0].toLocalFile();
	QStringList firstDir = getDirectory(firstFileName);
	currentImgIndex = 0;
	//from first image in its directory
	displayImage(firstFileName + "/" + firstDir[currentImgIndex], view1, scene1);
	if (urls.size() == 2) {
		QString lastFileName = urls[1].toLocalFile();
		QStringList lastDir = getDirectory(lastFileName);
		displayImage(lastFileName + "/" + lastDir[0], view2, scene2);
	}
	
}

//display image in window
void ImageProcessing::displayImage(QString path, ImageView *view, ImageScene *scene) {
	//it should set QGraphicsPixmapItem *. if set QGraphicsPixmapItem, it can not display image
	QGraphicsPixmapItem *imgItem = new QGraphicsPixmapItem(QPixmap::fromImage(getImage(path)));
	scene->imgBaseItem = imgItem;
	scene->addItem(imgItem);
	view->setScene(scene);
	view->show();
}

QStringList ImageProcessing::getDirectory(const QString path, bool is_img) {
	QDir dir(path);  
	QStringList nameFilters;
	if (is_img == true) {
		nameFilters << "*.jpg" << "*.jpeg" << "*.png" << "*.tif" << "*.tiff";
	}
	else {
		nameFilters << "*.shp";
	}
	
	QStringList fileDirectory = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);	
	return fileDirectory;
}

void ImageProcessing::setViewHLayout() {
	QHBoxLayout *viewHLayout = new QHBoxLayout();
	viewHLayout->addWidget(view1);
	viewHLayout->addWidget(view2);
	ui.centralWidget->setLayout(viewHLayout);
	//ui.gridLayout->setSpacing(10);//设置间距
	//ui.gridLayout->addWidget(view1, 0, 0);//往网格的不同坐标添加不同的组件
	//ui.gridLayout->addWidget(view2, 0, 1);
	//ui.gridLayoutWidget->setLayout(ui.gridLayout);
	view1->hide();
	view2->hide();
}

void ImageProcessing::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Right){
		nextImg();
	}
	if (event->key() == Qt::Key_Left){
		lastImg();
	}
	//paint or not
	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_P)) {
		isPaint();
	}
	//save painted shp
	if (event->key() == Qt::Key_S) {
		saveShp();
	}
	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_O)) {
		openShp();
	}
	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_C)) {
		clipImg();
	}
	//remove shp(include opened shapefile and painted shp)
	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_R)) {
		removeShp();
	}
	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_T)) {
		transImg();
	}
}

/*
use key right to next image
file 1 and file 2 have same images
*/
void ImageProcessing::nextImg() {
	//remove painted polygons of scene1 mainly
	this->updateWindow();
	QString firstFileName = urls[0].toLocalFile();
	QStringList firstDir = getDirectory(firstFileName);
	currentImgIndex == firstDir.size() - 1 ? currentImgIndex = 0 : ++currentImgIndex;
	displayImage(firstFileName + "/" + firstDir[currentImgIndex], view1, scene1);
	if (urls.size() == 2) {
		QString lastFileName = urls[1].toLocalFile();
		QStringList lastDir = getDirectory(lastFileName);
		displayImage(lastFileName + "/" + lastDir[currentImgIndex], view2, scene2);
	}
}

void ImageProcessing::lastImg() {
	this->updateWindow();
	QString firstFileName = urls[0].toLocalFile();
	QStringList firstDir = getDirectory(firstFileName);
	currentImgIndex == 0 ? currentImgIndex = firstDir.size() - 1 : --currentImgIndex;
	displayImage(firstFileName + "/" + firstDir[currentImgIndex], view1, scene1);
	if (urls.size() == 2) {
		QString lastFileName = urls[1].toLocalFile();
		QStringList lastDir = getDirectory(lastFileName);
		displayImage(lastFileName + "/" + lastDir[currentImgIndex], view2, scene2);
	}
}

void ImageProcessing::updateWindow() {
	scene1 = new ImageScene();
	scene1->is_paint = is_paint;
	scene2 = new ImageScene();
	this->windowLink(view1, view2, scene1, scene2);
}

//set file directory
QString ImageProcessing::saveShpFile() {
	QString saveShpFilePath = QFileDialog::getExistingDirectory(
		this,
		tr("Choose file to save shape(label)"),
		"/home",
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	if (saveShpFilePath == NULL) {
		QMessageBox::about(NULL, "Error", "Save shape file path is NULL");
	}
	return saveShpFilePath;
}

void ImageProcessing::saveShp() {
	if (is_paint == false) {
		QMessageBox::about(NULL, "Warning", "You do not open paint mode, please use ctrl+P to open!");
		return;
	}
	QString firstFileName = urls[0].toLocalFile();
	QStringList firstDir = getDirectory(firstFileName);
	QString imgPath = firstFileName + "/" + firstDir[currentImgIndex];
	QString currentImgFullName = firstDir[currentImgIndex];
	QStringList nameAndSuffix = currentImgFullName.split('.');
	QString currentImgName = nameAndSuffix.at(0);
	//set shapefile path
	QString saveShpFilePath = this->saveShpFile();
	QString saveShpPath = saveShpFilePath + "/" + currentImgName + ".shp";
	if (scene1->polygons.size() == 0) {
		QMessageBox::about(NULL, "Warning", "You do not draw any shapefiles");
		return;
	}
	item.setProjAndLeftTop(imgPath);
	item.writeShape(scene1->polygons, saveShpPath);
}

void ImageProcessing::openShp() {
	QString shp = QFileDialog::getOpenFileName(this,
		"Please choose a shapefile to open",
		"",
		"*.shp;;");
	if (shp == NULL) {
		QMessageBox::about(NULL, "Error", "Shapefile path is NULL");
		return;
	}
	QString firstFileName = urls[0].toLocalFile();
	QStringList firstDir = getDirectory(firstFileName);
	QString imgPath = firstFileName + "/" + firstDir[currentImgIndex];
	item.setProjAndLeftTop(imgPath);
	scene1->displayShp(item.readShape(shp));
}

void ImageProcessing::clipImg() {
	//origin image
	QString imgFilePath = QFileDialog::getExistingDirectory(
		this,
		tr("Choose file to open image"),
		"/home",
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	if (imgFilePath == NULL) {
		QMessageBox::about(NULL, "Error", "Image file path is NULL");
		return;
	}
	//origion shapefile
	QString shpFilePath = QFileDialog::getExistingDirectory(
		this,
		tr("Choose file to open shape"),
		"/home",
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	if (shpFilePath == NULL) {
		QMessageBox::about(NULL, "Error", "Shape file path is NULL");
		return;
	}
	//clipped image directory
	QString clippedImgFilePath = QFileDialog::getExistingDirectory(
		this,
		tr("Choose file to save clipped image"),
		"/home",
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	if (clippedImgFilePath == NULL) {
		QMessageBox::about(NULL, "Error", "Clipped image file path is NULL");
		return;
	}

	QStringList imgDir = getDirectory(imgFilePath);
	QStringList shpDir = getDirectory(shpFilePath, false);
	for (int i = 0; i < imgDir.size(); ++i) {
		QStringList imgNameAndSuffix = imgDir[i].split('.');
		QString tempImgName = imgNameAndSuffix.at(0);
		QString tempImgPath = imgFilePath + "/" + imgDir[i];
		QImage img = this->getImage(tempImgPath);
		if (this->is_proj(tempImgPath))item.setProjAndLeftTop(tempImgPath);
		for (int j = 0; j < shpDir.size(); ++j) {
			QStringList shpNameAndSuffix = shpDir[j].split('.');
			//if shape file has the same name, clip
			QString tempShpName = shpNameAndSuffix.at(0);
			if (tempImgName == tempShpName) {
				QString tempShpPath = shpFilePath + "/" + shpDir[j];
				QVector<QPointF> centers = item.getPolygonCenterSet(item.readShape(tempShpPath));
				item.clipImage(clippedImgFilePath, tempImgPath, img, centers);
			}
		}
	}

}

void ImageProcessing::transImg() {
	QString firstFileName = urls[0].toLocalFile();
	QStringList firstDir = getDirectory(firstFileName);
	QString firstPath = firstFileName + "/" + firstDir[currentImgIndex];

	GDALDataset *tempDataset = (GDALDataset*)GDALOpen(firstPath.toStdString().c_str(), GA_ReadOnly);
	int imgBandNum = tempDataset->GetRasterCount();
	if (imgBandNum >= 4) {
		//remove painted polygons of scene1 mainly
		scene1 = new ImageScene();
		scene1->is_paint = is_paint;
		is_trueColor = !is_trueColor;
		displayImage(firstPath, view1, scene1);
	}
	if (urls.size() == 2) {
		QString lastFileName = urls[1].toLocalFile();
		QStringList lastDir = getDirectory(lastFileName);
		QString lastPath = lastFileName + "/" + lastDir[currentImgIndex];
		GDALDataset *tempDataset = (GDALDataset*)GDALOpen(lastPath.toStdString().c_str(), GA_ReadOnly);
		int imgBandNum = tempDataset->GetRasterCount();
		if (imgBandNum >= 4) {
			//remove painted polygons of scene2 mainly
			scene2 = new ImageScene();
			displayImage(lastPath, view2, scene2);
		}
	}
	this->windowLink(view1, view2, scene1, scene2);
}

//display mode
void ImageProcessing::labelText() {
	QLabel label(ui.centralWidget);
	//label color, palette can not change volor
	label.setStyleSheet("color:rgb(255, 255, 255);");
	//label size
	QFont font;
	font.setPointSize(10);
	label.setFont(font);
	if (is_paint == true) {
		label.setText(QStringLiteral("Paint mode"));
	}
	else {
		label.setText(QStringLiteral("Browse mode"));
	}
	label.show();	
	//delay 2 second
	QTime time;
	time.start();
	while (time.elapsed() < 1500)
		QCoreApplication::processEvents();
	label.hide();
}

bool ImageProcessing::is_proj(QString path) {
	//judge that whether image is remote sensing image
	GDALDataset *tempDataset = (GDALDataset*)GDALOpen(path.toStdString().c_str(), GA_ReadOnly);
	QString prj = tempDataset->GetProjectionRef();
	return prj == NULL ? false : true;
}

void ImageProcessing::windowLink(ImageView *view1, ImageView *view2, ImageScene *scene1, ImageScene *scene2) {
	//linked view
	connect(view1, SIGNAL(linkedView(QPointF)), scene2, SLOT(on_linkedView(QPointF)));
	connect(view2, SIGNAL(linkedView(QPointF)), scene1, SLOT(on_linkedView(QPointF)));

	//delete self initial cross
	connect(view1, SIGNAL(deleteInitCross()), scene1, SLOT(on_deleteInitCross()));
	connect(view2, SIGNAL(deleteInitCross()), scene2, SLOT(on_deleteInitCross()));
}