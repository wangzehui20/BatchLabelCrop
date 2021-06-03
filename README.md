# Batch Annotation and Crop Tools

This tools is implemented by Qt, which is convenient to **label ROIs** in image using .shp format and **crop these ROIs batchingly** as data(256x256) in deep learning. Moreover, it could display image and segmentation correspondingly on linked widows and may be helpful to learn **QGraphicas**(View, Scene, Item).

## Dependency

GDAL

OpenCV

Last tested successfully using `gdal 2.0.3`, `opencv 3.3.0`.

 ## Usage

### 1.Rebuild solution

New folder in directory, for example: build. `cd build` in cmd or powershell, then `cmake -G "Visual Studio 14 Win64" ..`

Then you need to update gdal and opencv to include directory and library directory in **VC++ directory**(project properties), And update opencv(gdal_i.lib, opencv_world330d.lib, opencv_world330.lib) to Linker->input->**additional dependencies**.

### 2.UI

**Display**: drag and drop single image file or both image and its corresponding segmentation files directly

**Menu**: 

* Mode: Browse or Paint. you can label regions on Paint Mode(double clicks completes painting)
* Clip: First choose image file to be clipped, then shp file corresponding images to be clipped , Finally output file
* ...

## Reference

You may need to learn about Qt firstly, [Qt 实战一二三](https://blog.csdn.net/liang19890820/article/details/50277095). Also [GDAL](https://headfirst-gdal.readthedocs.io/en/latest/read-and-write.html)(using patch-read to load larger raster data).

## Presentation

![avatar](https://github.com/wangzehui20/BatchLabelCrop/blob/main/ui.png?raw=true)
