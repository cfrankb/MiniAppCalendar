# -------------------------------------------------
# Project created by QtCreator 2009-09-27T21:31:15
# -------------------------------------------------
TARGET = MiniAppCalendar
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    document.cpp \
    month.cpp \
    memodialog.cpp \
    dlgabout.cpp
HEADERS += mainwindow.h \
    document.h \
    month.h \
    memodialog.h \
    dlgabout.h
FORMS += mainwindow.ui \
    dlgabout.ui
TRANSLATIONS    = translations/translation_fr.ts
RESOURCES += MiniAppCalendar.qrc
RC_FILE = MiniAppCalendar.rc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
