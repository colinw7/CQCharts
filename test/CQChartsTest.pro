TEMPLATE = app

TARGET = CQChartsTest

DEPENDPATH += .

QT += widgets

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
CQChartsTest.cpp \
CQAppWindow.cpp \

HEADERS += \
CQChartsTest.h \
CQAppWindow.h \

DESTDIR     = .
OBJECTS_DIR = .

INCLUDEPATH += \
../include \
. \
../../CQUtil/include \
../../CQGradientPalette/include \
../../CExpr/include \
../../CJson/include \
../../CImageLib/include \
../../CFont/include \
../../CMath/include \
../../CFile/include \
../../CStrUtil/include \
../../CUtil/include \
../../COS/include \

unix:LIBS += \
-L../lib \
-L../../CQGradientPalette/lib \
-L../../CQPropertyTree/lib \
-L../../CQUtil/lib \
-L../../CJson/lib \
-L../../CImageLib/lib \
-L../../CFont/lib \
-L../../CUtil/lib \
-L../../CExpr/lib \
-L../../CConfig/lib \
-L../../CFile/lib \
-L../../CFileUtil/lib \
-L../../CMath/lib \
-L../../COS/lib \
-L../../CRegExp/lib \
-L../../CStrUtil/lib \
-lCQCharts -lCQGradientPalette -lCQPropertyTree -lCQUtil \
-lCJson -lCExpr -lCImageLib -lCFont \
-lCConfig -lCFile -lCFileUtil -lCMath -lCUtil \
-lCOS -lCRegExp -lCStrUtil \
-lpng -ljpeg -ltre
