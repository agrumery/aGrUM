#include <QtGui/QApplication>
#include <QTextCodec>

#include <agrum/core/exceptions.h>
#include <iostream>

#include "mainwindow.h"

int main(int argc, char *argv[]) try {

	QApplication a(argc, argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QCoreApplication::setOrganizationName("Lip6");
	QCoreApplication::setApplicationName("SkoobEditor");

	MainWindow w;
	w.showMaximized();

	return a.exec();

} catch ( const gum::Exception & err ) {
	std::cerr << err.getContent() << std::endl;
	return 1;
} catch ( const std::string & err ) {
	std::cerr << err << std::endl;
	return 2;
}
