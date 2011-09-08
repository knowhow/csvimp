#
# This file is part of the xTuple ERP: PostBooks Edition, a free and
# open source Enterprise Resource Planning software suite,
# Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
# It is licensed to you under the Common Public Attribution License
# version 1.0, the full text of which (including xTuple-specific Exhibits)
# is available at www.xtuple.com/CPAL.  By using this software, you agree
# to be bound by its terms.
#

TEMPLATE = app
CONFIG += qt warn_on release

include(../global.pri)

TARGET = ../csvimp

OBJECTS_DIR = tmp
MOC_DIR     = tmp
UI_DIR      = tmp

INCLUDEPATH += ../csvimpcommon ../csvimpcommon/images ../plugin \
               ../$${OPENRPT_DIR}/common ../$${OPENRPT_DIR}/MetaSQL \
               ../$${OPENRPT_BLD}/common ../$${OPENRPT_BLD}/MetaSQL
INCLUDEPATH = $$unique(INCLUDEPATH)

win32:INCLUDEPATH += .
DEPENDPATH  += $${INCLUDEPATH}

LIBS += -L../$${OPENRPT_BLD}/lib -lcommon -lMetaSQL

win32-msvc* {
  PRE_TARGETDEPS += ../$${OPENRPT_BLD}/lib/common.lib      \
                    ../$${OPENRPT_BLD}/lib/MetaSQL.lib     \

} else {
  PRE_TARGETDEPS += ../$${OPENRPT_BLD}/lib/libcommon.a      \
                    ../$${OPENRPT_BLD}/lib/libMetaSQL.a     \
}
win32:RC_FILE = application.rc

macx {
  RC_FILE = ../csvimpcommon/images/icons.icns
  QMAKE_INFO_PLIST = Info.plist
}

# Input
FORMS   = 
HEADERS = ../csvimpcommon/csvimpdata.h  \
          ../csvimpcommon/csvimpplugininterface.h

SOURCES = main.cpp      \
          ../csvimpcommon/csvimpdata.cpp

QT += sql
RESOURCES += ../csvimpcommon/csvimp.qrc
