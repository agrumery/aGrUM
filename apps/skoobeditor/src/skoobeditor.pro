#-------------------------------------------------
#
# Project created by QtCreator 2010-11-05T11:42:49
#
#-------------------------------------------------

TEMPLATE = app

QT     += core gui

CONFIG  += qt thread resources uic windows x11 exceptions stl debug_and_release warn_on

win32 {
	# Pour la dll d'agrum
	LIBS += -L$${PWD}/../bin/
	INCLUDEPATH += $${PWD}/../../../src/
}

!win32:debug {
	LIBS += -lqscintilla2 -lagrum-dbg
} else {
	LIBS += -lqscintilla2 -lagrum
}

TARGET = skoobeditor
DESTDIR = $${PWD}/../bin/Release

SOURCES += main.cpp\
		mainwindow.cpp \
    filecontroller.cpp \
    qsciscintillaextended.cpp \
    editcontroller.cpp \
	searchcontroller.cpp \
    advancedsearch.cpp \
    viewcontroller.cpp \
    projectcontroller.cpp \
    newprojectdialog.cpp \
    project.cpp \
    qscilexerskoor.cpp \
    buildcontroller.cpp \
    properties.cpp \
    qkeysequencewidget.cpp \
    qscilexerskool2.cpp \
	skoolinterpretation.cpp \
    skoorinterpretation.cpp \
    projectproperties.cpp \
    prmtreemodel.cpp \
    prmcompleter.cpp \
	abstractparser.cpp
    
HEADERS  += mainwindow.h \
    filecontroller.h \
    qsciscintillaextended.h \
    editcontroller.h \
    searchcontroller.h \
    advancedsearch.h \
    viewcontroller.h \
    projectcontroller.h \
    newprojectdialog.h \
    project.h \
    qscilexerskoor.h \
    buildcontroller.h \
    properties.h \
    qkeysequencewidget.h \
    qscilexerskool2.h \
    syntaxchecker.h \
	skoolinterpretation.h \
    skoorinterpretation.h \
    projectproperties.h \
    prmtreemodel.h \
    prmcompleter.h \
	abstractparser.h

FORMS    += mainwindow.ui \
    advancedsearch.ui \
    newprojectdialog.ui \
    properties.ui \
    projectproperties.ui

RESOURCES += \
	../rsrc/resources.qrc
