#设置cmake版本号
cmake_minimum_required(VERSION 3.15)

#设置工程名称
project(BatchLabelCrop)
 
#设置工程包含当前目录，非必须
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#复制资源文件夹到build中
file(COPY "./Resources" DESTINATION "./")
 
#打开全局moc,设置自动生成moc文件，一定要设置
set(CMAKE_AUTOMOC ON)
#打开全局uic，非必须
set(CMAKE_AUTOUIC ON)
#打开全局rcc，非必须
#set(CMAKE_AUTORCC ON)

#查找需要的Qt库文件
FIND_PACKAGE(Qt5 COMPONENTS Widgets REQUIRED)

#查找当前文件夹中的所有源代码文件，也可以通过Set命令将所有文件设置为一个变量
FILE(GLOB SRC_FILES "./*.cpp")
#查找设置当前文件夹中所有的头文件
FILE(GLOB HEAD_FILES "./*.h")
#查找设置当前文件夹中所有的ui文件
FILE(GLOB UI_FILES "./*.ui")

#通过Ui文件生成对应的头文件，一定要添加
qt5_wrap_ui(WRAP_FILES ${UI_FILES})

#添加资源文件，非必须，一旦采用，注意修改相应的qrc文件名
#set(RCC_FILES ImageProcessing.qrc)

#将ui文件和生成文件整理在一个文件夹中，非必须
source_group("Ui" FILES ${UI_FILES} ${WRAP_FILES} )

#创建工程文件
add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEAD_FILES} ${RCC_FILES} ${WRAP_FILES})

#添加Qt5依赖项
TARGET_LINK_LIBRARIES(BatchLabelCrop ${Qt5Widgets_LIBRARIES})