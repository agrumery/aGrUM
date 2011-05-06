#include <QtGui/QApplication>
#include <QTextCodec>
#include <QDebug>

#include <agrum/core/exceptions.h>
#include <iostream>

#include "mainwindow.h"


/**
################################################################################
##                                                                            ##
##                                 SKOOBEDITOR                                ##
##                                                                            ##
################################################################################

L'application est composée de la fenêtre principale, et de classes de contrôle.
La fenêtre principale ne gère que l'aide et le "À propos". Elle fait surout le lien
entre les contrôleurs.
Chaque contrôleur gère les opérations de son menu et d'autres qui y sont associées.
 - Le FileController gère toutes les opérations en rapport avec les fichiers.
 - Le Editcontroller gère les opérations d'éditions du document courant
ainsi que les préférences de l'application.
 - Le SearchController gère les opérations de recherche et de remplacement,
dans le document courant ou le projet en cours, ainsi que les marqueurs.
 - Le ViewController gère les opérations de visibilités des différents éléments.
 - Le ProjectController gère tout ce qui est en rapport avec le projet.
 - Le BuildController gère tout ce qui est en rapport avec l'interprétation
et l'exécution des fichiers, en passant par la correction syntaxique
et l'auto-complétion contextuelle.

*/

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
} catch ( const QString & err ) {
	std::cerr << err.data() << std::endl;
	return 3;
}
