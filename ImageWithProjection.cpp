#include "ImageWithProjection.h"

static int imgSize = 512;

/*
create image with projection
*/
ImageWithProjection::ImageWithProjection() {
	
}

ImageWithProjection::~ImageWithProjection() {

}

Mat ImageWithProjection::image2Mat(const QString path) {
	GDALDataset *gdalDataset = (GDALDataset*)GDALOpen(path.toStdString().c_str(), GA_ReadOnly);

	//Height, Width and BandNum of image
	int imgHeight = gdalDataset->GetRasterYSize();
	int imgWidth = gdalDataset->GetRasterXSize();
	int imgBandNum = gdalDataset->GetRasterCount();

	//block size
	int blockWidth = imgSize;
	int blockHeight = imgSize;

	//set height, width of block to block processing
	int blockXNum = (imgWidth - 1) / blockWidth + 1;
	int blockYNum = (imgHeight - 1) / blockHeight + 1;

	//temporary band to read image band
	GDALRasterBand *tempBand = NULL;
	GDALDataType tempType = gdalDataset->GetRasterBand(1)->GetRasterDataType();

	Mat mergeMatWidth;
	Mat mergeMatHeight;
	std::vector<cv::Mat> imgMat(3);
	
	for (int i = 0; i < 3; ++i) {
		tempBand = gdalDataset->GetRasterBand(selectBand[i]);
		GDALDataType tempType = tempBand->GetRasterDataType();

		//patch-read
		for (int iy = 0; iy < blockYNum; ++iy) {
			for (int ix = 0; ix < blockXNum; ++ix) {
				//default static int is 512
				blockHeight = imgSize;
				blockWidth = imgSize;
				if (iy == blockYNum - 1) {
					blockHeight = imgHeight % blockHeight;
				}
				if (ix == blockXNum - 1) {
					blockWidth = imgWidth % blockWidth;
				}

				unsigned char *bufferByte = new unsigned char[blockWidth * blockHeight];
				float *bufferFloat = new float[blockWidth * blockHeight];

				//if data type is byte, read it straightly
				if (tempType == GDT_Byte) {					
					tempBand->RasterIO(GF_Read, ix * 512, iy * 512, blockWidth, blockHeight, bufferByte, blockWidth, blockHeight, GDT_Byte, 0, 0);
				}
				else {				
					tempBand->RasterIO(GF_Read, ix * 512, iy * 512, blockWidth, blockHeight, bufferFloat, blockWidth, blockHeight, GDT_Float32, 0, 0);
					bufferByte = imgSketchFloat2Byte(bufferFloat, tempBand, blockWidth * blockHeight, tempBand->GetNoDataValue());
				}
				cv::Mat tempMat(blockHeight, blockWidth, 0, bufferByte);
				if (ix > 0){
					mergeMatWidth = mergeWidth(mergeMatWidth, tempMat);
					continue;
				}
				mergeMatWidth = tempMat.clone();
			}
			if (iy > 0){
				mergeMatHeight = mergeHeight(mergeMatHeight, mergeMatWidth);
				continue;
			}
			mergeMatHeight = mergeMatWidth.clone();
		}
		imgMat.at(2 - i) = mergeMatHeight.clone();
		mergeMatWidth = NULL;
		mergeMatHeight = NULL;
	}
	//it should be cv::Mat *. if it is cv::Mat, it can not display(the whole white)
	cv::Mat *mergeImgMat = new cv::Mat;
	cv::merge(imgMat, *mergeImgMat);
	imgMat.clear();
	return *mergeImgMat;
}

/*
define the order of band in image
*/
void ImageWithProjection::defineBand(bool color, int bandNum) {
	//single band
	if (bandNum == 1){
		selectBand[0] = 1;
		selectBand[1] = 1;
		selectBand[2] = 1;
	}
	else {
		if (color == true) {
			//True color
			selectBand[0] = 1;
			selectBand[1] = 2;
			selectBand[2] = 3;
		}
		else {
			//Psedo color
			selectBand[0] = 2;
			selectBand[1] = 3;
			selectBand[2] = 4;
		}
	}
}

unsigned char *ImageWithProjection::imgSketchFloat2Byte(float *imgFloat, GDALRasterBand *band, int size, float noValue) {
	unsigned char *bufferByte = new unsigned char[size];
	float max, min;
	double minmax[2];
    //precise search's minimium is -32768 when parameter is false
	band->ComputeRasterMinMax(true, minmax);
	min = minmax[0];
	max = minmax[1];
	if (min <= noValue && noValue <= max){
		min = 0.0;
	}
	for (int i = 0; i < size; i++){
		if (imgFloat[i] > max){
			bufferByte[i] = 255.0;
		}
		else if (imgFloat[i] <= max && imgFloat[i] >= min){
			bufferByte[i] = (imgFloat[i] - min) / (max - min)  * 255.0;
		}
		else{
			bufferByte[i] = 0.0;
		}
	}
	return bufferByte;
}

Mat ImageWithProjection::mergeHeight(Mat A, Mat B)
{
	int totalRows = A.rows + B.rows;
	Mat mergedDescriptors(totalRows, A.cols, A.type());
	Mat submat = mergedDescriptors.rowRange(0, A.rows);
	A.copyTo(submat);
	submat = mergedDescriptors.rowRange(A.rows, totalRows);
	B.copyTo(submat);
	return mergedDescriptors;
}

Mat ImageWithProjection::mergeWidth(Mat A, Mat B)
{
	int totalCols = A.cols + B.cols;
	Mat mergedDescriptors(A.rows, totalCols, A.type());
	Mat submat = mergedDescriptors.colRange(0, A.cols);
	A.copyTo(submat);
	submat = mergedDescriptors.colRange(A.cols, totalCols);
	B.copyTo(submat);
	return mergedDescriptors;
}