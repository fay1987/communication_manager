TEMPLATE = vclib
CONFIG +=qt release thread
QT -= gui

TARGET = pr_modbus_101
DESTDIR = $(PDT_ROOT)/lib
DLLDESTDIR = $(PDT_ROOT)/dll

DEFINES += UNICODE QT_CORE_LIB QT_THREAD_SUPPORT _MODBUS_COSTOMIZE_
MOC_DIR=$(PDT_ROOT)/tmp/$$TARGET/moc
OBJECTS_DIR=$(PDT_ROOT)/tmp/$$TARGET/obj

INCLUDEPATH=$(PDT_ROOT)/include . $(ACE_ROOT)

win32:LIBS +=-L$(PDT_ROOT)/lib -L$(ACE_ROOT)/lib -lACE -lpr_platapi -lpr_dac -lpr_dacctrl -lpr_utility -lpr_dactool -lpr_rdbop -lpr_rdb
unix :LIBS +=-L$(PDT_ROOT)/lib -lACE -lplat_api -ldac -ldacctrl -lutl -ltool -lrdbop -lrdb -liedshm -liedinf

HEADERS += ../src/modbus_101.h
             


SOURCES += ../src/modbus_101.cpp
