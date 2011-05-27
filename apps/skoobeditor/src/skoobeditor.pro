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

SOURCES += \
    controllers/buildcontroller.cpp \
    controllers/editcontroller.cpp \
    controllers/filecontroller.cpp \
    controllers/projectcontroller.cpp \
    controllers/searchcontroller.cpp \
    controllers/viewcontroller.cpp \
    uis/advancedsearch.cpp \
	uis/mainwindow.cpp \
	uis/newprojectdialog.cpp \
	uis/projectproperties.cpp \
	uis/properties.cpp \
	lexers/qscilexerskool2.cpp \
	lexers/qscilexerskoor.cpp \
	parsers/abstractparser.cpp \
	parsers/skoolinterpretation.cpp \
	parsers/skoorinterpretation.cpp \
	main.cpp\
	prmcompleter.cpp \
	prmtreemodel.cpp \
	project.cpp \
	qcommandlineedit.cpp \
	qkeysequencewidget.cpp \
	qsciscintillaextended.cpp

HEADERS  += \
    controllers/buildcontroller.h \
    controllers/editcontroller.h \
    controllers/filecontroller.h \
    controllers/projectcontroller.h \
    controllers/searchcontroller.h \
    controllers/viewcontroller.h \
    uis/advancedsearch.h \
    uis/mainwindow.h \
    uis/newprojectdialog.h \
    uis/projectproperties.h \
    uis/properties.h \
    lexers/qscilexerskool2.h \
    lexers/qscilexerskoor.h \
    parsers/abstractparser.h \
    parsers/skoolinterpretation.h \
    parsers/skoorinterpretation.h \
    prmcompleter.h \
    prmtreemodel.h \
    project.h \
    qcommandlineedit.h \
    qkeysequencewidget.h \
    qsciscintillaextended.h

FORMS    += \
    uis/advancedsearch.ui \
    uis/mainwindow.ui \
    uis/newprojectdialog.ui \
    uis/projectproperties.ui
    uis/properties.ui \

RESOURCES += \
	../rsrc/resources.qrc
