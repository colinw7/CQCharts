CDEBUG = -g
LDEBUG = -g

PROG = CDotParseTest

CPP_SRC = \
CDotParseTest.cpp \
CDotParse.cpp \

EXTRA_CFLAGS = \
-I$(INC_DIR)/CFileParse \

LIBS = \
-lCFileParse -lCFile -lCOS -lCStrParse -lCStrUtil -lCPrintF

EXTRA_DEPS = \
$(LIB_DIR)/libCFileParse.a \

include $(MAKE_DIR)/bin.mk
