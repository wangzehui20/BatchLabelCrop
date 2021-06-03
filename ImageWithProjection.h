#pragma once

#ifndef ImageWithProjection_H
#define ImageWithProjection_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <gdal_priv.h>
#include <vector>
#include <QString>

#pragma comment(lib,"gdal_i.lib")

namespace cv
{
	using std::vector;
}
using
namespace  cv;

using
namespace  std;

class ImageWithProjection {

public :
	ImageWithProjection();
	~ImageWithProjection();

	int selectBand[3];

	Mat image2Mat(const QString path);
	unsigned char *imgSketchFloat2Byte(float *imgFloat, GDALRasterBand *band, int size, float noValue);
	void defineBand(bool color=true, int bandNum=3);
	//merge in col
	Mat mergeHeight(Mat A, Mat B);
	//merge in row
	Mat mergeWidth(Mat A, Mat B);
};

#endif