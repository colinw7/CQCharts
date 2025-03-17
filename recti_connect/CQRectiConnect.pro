APPNAME = CQRectiConnect

include($$(MAKE_DIR)/qt_app.mk)

SOURCES += \
CQRectiConnect.cpp

HEADERS += \
CQRectiConnect.h

INCLUDEPATH += \
$(INC_DIR)/CAStar \
$(INC_DIR)/CDisplayRange2D \
$(INC_DIR)/CMath \
$(INC_DIR)/COS \

PRE_TARGETDEPS = \

unix:LIBS += \
