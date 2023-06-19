TEMPLATE = app

TARGET = CQChartsSankeyView

QT += widgets svg xml opengl

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++17 \
-DCQCHARTS_FOLDED_MODEL \

MOC_DIR = .moc

SOURCES += \
main.cpp \
CQChartsSankeyView.cpp \

HEADERS += \
CQChartsSankeyView.h \

DESTDIR     = .
OBJECTS_DIR = .

GIT_DIR = ../../..

INCLUDEPATH += \
. \
../../include \
$$GIT_DIR/CQPropertyView/include \
$$GIT_DIR/CQBaseModel/include \
$$GIT_DIR/CQColors/include \
$$GIT_DIR/CQUtil/include \
$$GIT_DIR/CQPerfMonitor/include \
$$GIT_DIR/CImageLib/include \
$$GIT_DIR/CFont/include \
$$GIT_DIR/CReadLine/include \
$$GIT_DIR/CFile/include \
$$GIT_DIR/CMath/include \
$$GIT_DIR/CStrUtil/include \
$$GIT_DIR/CUtil/include \
$$GIT_DIR/COS/include \
/usr/include/tcl \

PRE_TARGETDEPS = \
$(LIB_DIR)/libCQCharts.a \

unix:LIBS += \
-L../../lib \
-L$$GIT_DIR/CQPropertyView/lib \
-L$$GIT_DIR/CQModelView/lib \
-L$$GIT_DIR/CQBaseModel/lib \
-L$$GIT_DIR/CQColors/lib \
-L$$GIT_DIR/CQCustomCombo/lib \
-L$$GIT_DIR/CQUtil/lib \
-L$$GIT_DIR/CJson/lib \
-L$$GIT_DIR/CCsv/lib \
-L$$GIT_DIR/CTsv/lib \
-L$$GIT_DIR/CXML/lib \
-L$$GIT_DIR/CQPerfMonitor/lib \
-L$$GIT_DIR/CQDot/lib \
-L$$GIT_DIR/CQDot/graphviz/lib \
-L$$GIT_DIR/CConfig/lib \
-L$$GIT_DIR/CCommand/lib \
-L$$GIT_DIR/CImageLib/lib \
-L$$GIT_DIR/CFont/lib \
-L$$GIT_DIR/CMath/lib \
-L$$GIT_DIR/CReadLine/lib \
-L$$GIT_DIR/CFileUtil/lib \
-L$$GIT_DIR/CFile/lib \
-L$$GIT_DIR/CUtil/lib \
-L$$GIT_DIR/CRegExp/lib \
-L$$GIT_DIR/CStrUtil/lib \
-L$$GIT_DIR/COS/lib \
-lCQCharts -lCQPropertyView -lCQModelView -lCQBaseModel -lCQColors \
-lCQCustomCombo -lCQUtil -lCQPerfMonitor -lCQGraphViz -lCGraphViz -lCJson -lCCsv -lCTsv \
-lCXML -lCCommand -lCConfig -lCImageLib -lCFont -lCMath \
-lCReadLine -lCFileUtil -lCFile -lCRegExp \
-lCUtil -lCStrUtil -lCOS \
-lreadline -lpng -ljpeg -ltre -ltcl \
-lGLU -lglut
