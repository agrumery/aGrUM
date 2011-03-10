#include "projectcontroller.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filecontroller.h"
#include "editcontroller.h"
#include "searchcontroller.h"
#include "viewcontroller.h"
#include "buildcontroller.h"
#include "qsciscintillaextended.h"
#include "qscilexerskool2.h"

#include "newprojectdialog.h"
#include "projectproperties.h"

#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>
#include <QSignalMapper>
#include <QTimer>
#include <QEventLoop>

#include <QDebug>


/*
 Réflections sur les interactions dans l'explorateur de projet (EdP) :

 Si il y a des fautes dans un fichier, quand on fait le parsage du projet,
 mets à jour l'icône dans l'EdP.

 CLIC DROIT
 - sur un dossier -> menu avec
   * Renommer
   * Supprimer (Confirmation)
   * Nouveau
	 + Package
	 + Classe
	 + Système
	 + Requête
   * Refactoring
	 + Renommer ("dans tout le projet")
	 + Déplacer ("dans tout le projet")
	 + Supprimer ("indique les références dans le projet, source d'erreurs")
 - sur un fichier -> menu avec
   * Renommer
   * Supprimer (confirmation)
   * Éxécuter (uniquement pour le fichier SKOOR)
   * Refactoring
	 + Renommer ("Renomme dans tout le projet")
	 + Déplacer ("dans tout le projet")
	 + Supprimer ("indique les références dans le projet, source d'erreurs")

  DRAG AND DROP
  - d'un dossier -> ne renomme PAS tout (passer par refactoring).
  - d'un fichier -> ne renomme PAS tout (passer par refactoring).

  Un fichier peut être renommé :
	1) par l'explorateur de projet;
	2) par sauvegarder sous.

  Quand un fichier est renommé :
	On change la référence dans le fichier même (projet ou pas). -> FileController
	On change la référence dans les autres fichiers que si projet. -> ProjectController

  Quand un fichier est déplacé :
	On change le package partout que si projet. -> ProjectController

	Quand on fait Refactoring -> Renommer : comme renommer normal
	Quand on fait Refactoring -> Déplacer : on comme un drag and drop ?
	Quand on fait Refactoring -> Supprimer : comme supprimer normal + on cherche dans le projet et message.
 */

/**
Réflexions sur la représentation interne du projet.

On peut utiliser SkoolReader, qui va parser les fichiers de manière automatique,
comme pour les erreurs, et ainsi récupérer les types, systèmes, interfaces, classes et attributs.

Problème : SkoolReader ne gère pas vraiment les dépendances (il ne les stock pas).
Il serait intéressant de le gérer, ne serait-ce que pour les messages d'erreurs.

Attention : en cas d'erreur dans le parsage (import non trouvé par exemple),
il faut quand même qu'on récupère le maximum d'informations.

POSSIBLITÉ 1 :
Quand on édite un fichier, on récupère les imports, et grâce au graphe de dépendance,
on sait ce à quoi peut accèder l'utilisateur à un moment donné.
Si, en parsant le fichier, on détecte une classe (ou type) qui appartient au projet
mais qui n'est pas accessible (pas d'import correspondant),
on propose à l'utilisateur d'ajouter l'import.
-> Il doit être possible de mêler ce parsage avec le parsage d'erreur
(ie, un parsage et on récupère toutes les informations).
-> À chaque parsage du fichier, on récupère les messages d'erreurs et l'architecture (= classes...).
-> Avec l'architecture, on peut savoir quels sont les imports du fichier (?), et qu'est-ce qui appartient au fichier.

Au moment où on change fichier, on sauvegarde dans le modèle du projet l'ensemble des infos
1) voir avec le fichier virtuel qui importe tout.
2) peut directement être le PRM.

Quand on parse le fichier en cours, on reconnnait les imports car
ils ne sont pas forcément dans le même package.

POSSIBILITÉ 2:
On propose tout, et si l'utilisateur utilise un composant qui n'est pas importé,
on l'ajoute.
*/

/*
Réflexions sur l'autocomplétion contextuelle.

Ajouter une methode autoCompleteFromProject.

À chaque cycle de parsage du fichier en cours :
-> On récupère les erreurs.
-> On récupère le prm, avec tous les types/valeurs/interfaces/classes/attributs/systèmes disponibles.

On peut créer une classe spécialisé dans l'autocomplétion, qui serait en étroite
coopération avec un Parser ou Scanner, et qui à un moment donné,
sait le token ou la règle attendue. Idem pour la coloration...
mais il faudra affiner la syntaxe pour séparer les différents types (classes, attributs, etc)
et vérifier que ça marche avec les règles et pas les tokens du coup.

On peut utiliser un PRMModel qui étend TreeModel. On lui couple les mots clés dans le completer.

Le modèle local au document en train d'être édité sert à
 - l'autocomplétion dans le fichier.
Le modèle global du projet sert à
 - l'autocomplétion dans le fichier pour les imports.

SOURCES POUR L'AUTOCOMPLÉTION
Skool root
 - liste des mots clés

 - arbre des package.types
 - liste des labels de tous les types

 - arbre des package.interface.attribut
 - arbre des package.interface.reference

 - arbre des package.class
 - arbre des reference.attribut pour toutes les classes
 - arbre des reference.reference.attribut pour toutes les classes
 - etc ???? !!!!!

 - arbre des package.class
 - arbre des instance.reference

=>

- liste des mots clés
- liste des labels
- arbre des package.type/interface/class/system.attribut/reference
- idem - le package du document en cours
- arbre des reference/instance.(reference.)*attribut

Skoor root
 - arbre du package.system.instance.attribut
 - liste des labels
 - liste des mots clés.


1 ) Quand le PARSAGE est fini :
 - gestion habituelle des erreurs;
 - on actualise le modèle courant (par exemple BuildController::currentDocumentModel:PRMModel*
   != ProjectController::model:PRMModel*);

2 ) Quand on appelle l'AUTOCOMPLÉTION :
 - on n'appelle plus celle de QScintilla, sauf si il n'y a pas de projet ouvert;
 - Le SkoolCompleter possède "en dur" (ou dans un fichier) la liste des mots clé,
   et, s'il existe, va chercher dans le modèle les deux arbres supplémentaires.
   Pour cela notre SkoolCompleter possède 4 QCompleter, un pour chaque source.

POUR LA COLORATION : retirer la coloration des classes, imports etc.
Reste les commentaires, les mots-clés, les chiffres.
 */

/**
  IMPLÉMENTATION

  Dans BuildController :

* Quand l'interprétation est finie, on récupère le SkoolReader, et,
  d'un coté on stocke le conteneur d'erreur, et de l'autre le PRM.

* Le prm est accessible via la méthode currentDocumentModel qui renvoie non pas
  le prm mais un PRMModel, qui fait le proxy entre le prm et le programme
  (et qui permet de récupérer toutes les informations nécessaire en Qt).

* Le modèle QPRM possède des ascesseurs pour Label/Type/Interface/Classe/System = ***
  - QStringList get***s(); // Renvoie les *** avec le chemin complet (inclut le package)
  - bool is***( const QString & );

  - QStringList getLabels( const QString & typeName ):

  - QStringList getInterfaceAttributes( const QString & interfaceName );
  - QStringList getInterfaceReferences( const QString & interfaceName );

  - QStringList getClassAttributes( const QString & className );
  - QStringList getClassReferences( const QString & className );

  - QStringList getInstances( const QString & systemName );
  - QString getInstanceClass( const QString & instanceName );

  - QString getType( const QString & name ); /// Can be an attribute, a reference or an instance

  - bool isAttribute( const QString & type, const QString & attributeName ) /// Can be a class or interface
  - bool isReference( const QString & type, const QString & referenceName ) /// Can be a class or interface
  - bool isInstance( const QString & system, const QString & instanceName )

  - QModelIndex parent( QModelIndex )
  - bool hasChildren( QModelIndex )
  - int rowCount( QModelIndex )
  - int columnCount( QModelIndex )
  - QVariant data( QModelIndex )
  - QModelIndex index(int,int,QModelIndex)

  */

// Private data
struct ProjectController::PrivateData {
	int numberMaxOfRecentsProjects;
	QMenu * recentsProjects;
	QList<QString> recentsProjectsList;
	QSignalMapper * recentsProjectsMapper;
	ProjectProperties * projectProperties;
	QMenu * fileMenu;
	QMenu * dirMenu;
};


/// Constructor
ProjectController::ProjectController(MainWindow * mw, QObject *parent) :
		QObject(parent),
		mw(mw),
		currentProj(0),
		d(new PrivateData)
{
	mw->ui->projectExplorator->setVisible(false);
	mw->ui->projectExplorator->setDragDropMode(QAbstractItemView::InternalMove);

	d->projectProperties = 0;
	mw->ui->actionProjectProperties->setEnabled(false);

	d->recentsProjects = new QMenu(mw);
	d->recentsProjectsMapper = new QSignalMapper(mw);

	// Construct "Recent project" menu
	mw->ui->actionRecentProject->setMenu( d->recentsProjects );
	connect(d->recentsProjectsMapper,SIGNAL(mapped(QString)),this,SLOT(openProject(QString)));

	d->fileMenu = new QMenu(mw->ui->projectExplorator);
	d->fileMenu->addAction(tr("Re&nommer"))->setData("rename");
	d->fileMenu->addAction(tr("&Supprimer"))->setData("remove");
	d->fileMenu->addAction(tr("&Éxécuter"))->setData("execute");
//	QMenu * m = d->fileMenu->addMenu(tr("&Refactoring"));
//	m->addAction(tr("Re&nommer"))->setData("refact-rename");
//	m->addAction(tr("&Déplacer"))->setData("refact-move");
//	m->addAction(tr("&Supprimer"))->setData("refact-remove");

	d->dirMenu = new QMenu(mw->ui->projectExplorator);
	d->dirMenu->addAction(tr("Re&nommer"))->setData("rename");
	d->dirMenu->addAction(tr("&Supprimer"))->setData("remove"); //-> Don't work for moment.
	QMenu * m = d->dirMenu->addMenu(tr("Ajouter"));
	m->addAction(tr("&Package"))->setData("addPackage");
//	m->addAction(tr("&Classe"))->setData("addClass");
//	m->addAction(tr("&Système"))->setData("addSystem");
//	m->addAction(tr("&Requête"))->setData("addRequest");
	m = d->dirMenu->addMenu(tr("&Refactoring"));
//	m->addAction(tr("Re&nommer"))->setData("refact-rename");
//	m->addAction(tr("&Déplacer"))->setData("refact-move");
//	m->addAction(tr("&Supprimer"))->setData("refact-remove");

	connect( mw->ui->projectExplorator, SIGNAL(clicked(QModelIndex)), this, SLOT(on_projectExplorator_clicked(QModelIndex)) );
	connect( mw->ui->projectExplorator, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_projectExplorator_doubleClicked(QModelIndex)));
	connect( mw->ui->projectExplorator, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );
	connect( mw->ui->projectExplorator->itemDelegate(), SIGNAL(closeEditor(QWidget*)), this, SLOT(onItemRenameFinished()) );

	connect( mw->ui->actionNewProject, SIGNAL(triggered()), this, SLOT(newProject()) );
	connect( mw->ui->actionOpenProject, SIGNAL(triggered()), this, SLOT(openProject()) );
	connect( mw->ui->actionNewClass, SIGNAL(triggered()), this, SLOT(createNewClassFile()) );
	connect( mw->ui->actionNewModel, SIGNAL(triggered()), this, SLOT(createNewSystemFile()) );
	connect( mw->ui->actionNewRequestFile, SIGNAL(triggered()), this, SLOT(createNewRequestFile()) );
	connect( mw->ui->actionCloseProject, SIGNAL(triggered()), this, SLOT(closeProject()) );

	QTimer::singleShot( 200, this, SLOT(triggerInit()) );
}


/// Destructor
ProjectController::~ProjectController()
{
	delete d;
}

void ProjectController::triggerInit()
{
	QSettings settings;
	settings.beginGroup("project");

	//
	d->numberMaxOfRecentsProjects = settings.value("numberMaxOfRecentsProjects",5).toInt();

	// Read the last closed projects in settings
	int size = settings.beginReadArray("recentsProjects");
	for (int i = 0; i < size ; i++) {
		settings.setArrayIndex(i);
		addToRecentsProjects( settings.value("project").toString() );
	}
	settings.endArray();

	// Reopen project don't close last time.
	QString lastProject = settings.value("last project").toString();
	if ( ! lastProject.isEmpty() ) {
		openProject( lastProject );
	} else {
		currentProj = 0;
		mw->ui->projectExplorator->setModel(currentProj);

		// Change the current directory
		QDir::setCurrent(QDir::homePath());
	}
}


/**
  Return the current project
  */
Project * ProjectController::currentProject() const
{
	return currentProj;
}


/**
  Return true if a project is open, false otherwise.
  */
bool ProjectController::isOpenProject() const
{
	return currentProj != 0;
}


/**
  Create a new project.
  Open a dialog and ask the name and the directory of the project.
  */
void ProjectController::newProject()
{
	NewProjectDialog dial(mw);

	// If user cancel, do nothing
	if ( ! dial.exec() )
		return;

	if ( currentProj ) {
		currentProj->close();
		currentProj->deleteLater();
		currentProj = 0;
	}

	currentProj = new Project(dial.projectDir(),this);
	connect( currentProj, SIGNAL(fileRenamed(QString,QString,QString)), this, SLOT(onFileRenamed(QString,QString,QString)) );
	connect( currentProj, SIGNAL(fileMoved(QString,QString)), this, SLOT(onFileMoved(QString,QString)) );

	d->projectProperties = new ProjectProperties(currentProj, mw);
	mw->ui->actionProjectProperties->setEnabled(true);
	connect( mw->ui->actionProjectProperties, SIGNAL(triggered()), d->projectProperties, SLOT(exec()) );

	// We change current directory to the project directory
	QDir::setCurrent(dial.projectDir());

	// Create empty files
	//createNewClassFile();

	// Show project arborescence
	mw->ui->actionProjectExploratorVisibility->setEnabled(true);
	mw->ui->projectExplorator->setModel(currentProj);
	mw->ui->projectExplorator->setRootIndex( currentProj->root() );
	mw->vc->setProjectExploratorVisibility(true);
	mw->ui->projectExplorator->expandAll();

	// Enable new specific file creation
	mw->ui->actionNewClass->setEnabled(true);
	mw->ui->actionNewModel->setEnabled(true);
	mw->ui->actionNewRequestFile->setEnabled(true);

	saveProjectsState();
}


/**
  Create an empty skool file with a class architecture,
  and ask the filename.
  */
void ProjectController::createNewClassFile()
{
	// We add a new skool empty file,
	QsciScintillaExtended * sci = mw->fc->newDocument(tr("Nouveau skool"), QsciScintillaExtended::Skool);
	sci->setLexer(QsciScintillaExtended::Skool);

	// give it the focus,
	mw->ui->tabWidget->setCurrentWidget(sci);
	sci->setFocus();

	if ( mw->fc->saveAsFile(sci, currentProj->dir() + tr("/classes/empty_file.skool") ) ) {

		// and prefill it.
		QFileInfo info(sci->filename());
		QString className = info.baseName();
		QString packageName = QDir(currentProj->dir()).relativeFilePath(info.path()).replace("/",".");

		sci->setText("package "+ packageName +";\n\n//\nclass "+ className + " {\n}\n");
		sci->setCursorPosition(2,6);

		mw->fc->saveFile(sci);
	} else {
		mw->fc->closeFile( mw->ui->tabWidget->indexOf(sci) );
	}
}

/**
  Create an empty skoor file with a request architecture,
  and ask the filename.
  */
void ProjectController::createNewRequestFile()
{
	// We add a new skoor empty file,
	QsciScintillaExtended * sci = mw->fc->newDocument(tr("Nouveau skoor"), QsciScintillaExtended::Skoor);
	sci->setLexer(QsciScintillaExtended::Skoor);

	// give it the focus,
	mw->ui->tabWidget->setCurrentWidget(sci);
	sci->setFocus();

	if ( mw->fc->saveAsFile(sci, currentProj->dir() + tr("/requests/empty_file.skoor") ) ) {

		// and prefill it.
		sci->setText("\n//\nrequest "+ QFileInfo(sci->filename()).baseName() + " {\n}\n");
		sci->setCursorPosition(2,8);

		mw->fc->saveFile(sci);
	} else {
		mw->fc->closeFile( mw->ui->tabWidget->indexOf(sci) );
	}
}

/**
  Create an empty skool file with a system architecture,
  and ask the filename.
  */
void ProjectController::createNewSystemFile()
{
	QsciScintillaExtended * sci = mw->fc->newDocument(tr("Nouveau skool"), QsciScintillaExtended::Skool);
	sci->setLexer(QsciScintillaExtended::Skool);

	// give it the focus,
	mw->ui->tabWidget->setCurrentWidget(sci);
	sci->setFocus();

	if ( mw->fc->saveAsFile(sci, currentProj->dir() + tr("/systems/empty_file.skool") ) ) {
		// and prefill it.
		QFileInfo info(sci->filename());
		QString systemName = info.baseName();
		QString packageName = QDir(currentProj->dir()).relativeFilePath(info.path()).replace("/",".");

		sci->setText("package "+ packageName +";\n\n//\nsystem "+ systemName + " {\n}\n");
		sci->setCursorPosition(2,7);

		mw->fc->saveFile(sci);
	} else {
		mw->fc->closeFile( mw->ui->tabWidget->indexOf(sci) );
	}
}

/**
  Open the project projectpath,
  or open a dialog to choose the project directory if projectpath is empty.
  */
void ProjectController::openProject(QString projectpath)
{
	if ( projectpath.isEmpty() )
		// Retrieve the project dir
		projectpath = QFileDialog::getExistingDirectory(mw,
														tr("Sélectionnez le répertoire du projet"),
														QDir::homePath());

	QDir qDir(projectpath);

	// TODO : Warning are not ?
	if ( ! qDir.exists() )
		return;

	if ( ! qDir.exists(qDir.dirName()+".skoop")) {
		int rep = QMessageBox::warning(mw,tr("Attention"),tr("Ce répertoire ne contient pas de fichier projet.\nLe créer ?"),
							 QMessageBox::Cancel, QMessageBox::Ok);
		if ( rep == QMessageBox::Cancel )
			return;
	}

	if ( currentProj )
		closeProject();

	currentProj = new Project(qDir.absolutePath(),this);
	connect( currentProj, SIGNAL(fileRenamed(QString,QString,QString)), this, SLOT(onFileRenamed(QString,QString,QString)) );
	connect( currentProj, SIGNAL(fileMoved(QString,QString)), this, SLOT(onFileMoved(QString,QString)) );

	d->projectProperties = new ProjectProperties(currentProj, mw);
	mw->ui->actionProjectProperties->setEnabled(true);
	connect( mw->ui->actionProjectProperties, SIGNAL(triggered()), d->projectProperties, SLOT(exec()) );

	// We change current directory to the project directory
	QDir::setCurrent(currentProj->dir());

	removeOfRecentsProjects(qDir.absolutePath());

	// Show project arborescence
	mw->ui->actionProjectExploratorVisibility->setEnabled(true);
	mw->ui->projectExplorator->setModel(currentProj);
	mw->ui->projectExplorator->setRootIndex( currentProj->root() );
	mw->vc->setProjectExploratorVisibility(true);
	//return;

	// Enable new specific file creation
	mw->ui->actionNewClass->setEnabled(true);
	mw->ui->actionNewModel->setEnabled(true);
	mw->ui->actionNewRequestFile->setEnabled(true);

	saveProjectsState();

	//
	QTimer::singleShot(200, mw->ui->projectExplorator, SLOT(expandAll()) );
}


/**
  Close the current project.
  */
void ProjectController::closeProject()
{
	if ( currentProj ) {
		addToRecentsProjects(currentProj->dir());

		// Tests because can be delete by MainWindow
		disconnect( mw->ui->actionProjectProperties, SIGNAL(triggered()), d->projectProperties, SLOT(exec()) );
		d->projectProperties->deleteLater();
		d->projectProperties = 0;

		currentProj->close();
		currentProj->deleteLater();
		currentProj = 0;

		mw->ui->actionProjectProperties->setEnabled(false);
		QDir::setCurrent(QDir::homePath());
	}
	mw->ui->projectExplorator->setModel(currentProj);
	mw->vc->setProjectExploratorVisibility(false);

	// Disable new specific file creation
	mw->ui->actionNewClass->setEnabled(false);
	mw->ui->actionNewModel->setEnabled(false);
	mw->ui->actionNewRequestFile->setEnabled(false);
	mw->ui->actionProjectExploratorVisibility->setEnabled(false);

	saveProjectsState();
}


/**
	This method change a filename, a package in all file in the project.
	It change package in the file when it is moved,
	class or system name when it is renamed,
	and imports in others files in both cases.

	If \a fromFilePath is a file, it can be :
	- renamed, if only filename change in \a toFilePath;
	- moved, if only path change in \a toFilePath;
	- moved and renamed, if both path and filename change;
	- removed, if \a toFilePath is an empty string;

	If \a fromFilePath is a directory, it can be :
	- renamed, if only dirname change in \a toFilePath;
	- moved, if only path change in \a toFilePath;
	- moved and renamed, if both path and dirname change;
	- removed, if \a toFilePath is an empty string;

	This method do nothing if fromFilePath is not a valid file in the project.
*/
void ProjectController::refactor( const QString & fromFilePath, const QString & toFilePath )
{
	qWarning() << "Project::refactor() was called, but is not yet implemented.\nFor moment, this method do nothing.";
	return;

// ############################### onFileRenamed ###############################

	QFileInfo oldInfo(fromFilePath), newInfo(toFilePath);

	if ( fromFilePath == toFilePath )
		return;

	if ( newInfo.isDir() ) {
		//// On fait comme si tous les fichiers enfants avait été bougé.
		//foreach( QString file, childs ) {
		//QFileInfo info(file);
		//onFileMoved( oldPath + info.fileName(), info.filePath() )
		//}
		return;
	}

	// Si l'extension est différente message
	if (oldInfo.suffix() != newInfo.suffix()) {
		QMessageBox::warning( mw, tr("Attention !"), tr("%1 a changé d'extension !").arg(oldInfo.filePath()) );
		return;
	}

	QString oldPackage = currentProject()->rootDirectory().relativeFilePath(oldInfo.absolutePath());
	QString newPackage = currentProject()->rootDirectory().relativeFilePath(newInfo.absolutePath());
	oldPackage.replace("/",".");
	newPackage.replace("/",".");

	foreach ( QString filename, currentProj->files() ) {
		QsciScintillaExtended * sci = 0;

		if ( filename == newInfo.filePath() ) {
			sci = mw->fc->fileToDocument( oldInfo.filePath() );
		} else
			sci = mw->fc->fileToDocument( filename );

		// If file is not open, we open it.
		if ( sci == 0 ) {
			// We open the file
			QFile file(filename);

			if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
				continue;

			// We charge the file,
			sci = new QsciScintillaExtended(filename);
			QString text(file.readAll());
			file.close();
			sci->setText(text);
			sci->setModified(false);
		}

		if ( filename == newInfo.filePath() )
			// We change his internal filename
			sci->setFilename( newInfo.filePath() );

		// Si oui on change le nom de la classe ou système dans le fichier
		sci->replaceAll(oldInfo.baseName(), newInfo.baseName(), false, true, true);

		// Si le document n'est pas un document ouvert,
		if ( mw->ui->tabWidget->indexOf(sci) == -1 ) {
			// We delete it
			mw->fc->saveFile(sci);
			sci->deleteLater();
		}
	}

// ############################### onFileMoved #################################

	// On vérifie si le répertoire a changé
	if ( oldInfo.absolutePath() == newInfo.absolutePath() )
		return;

	// If it's a directory,
	// we move all its children recursively.
	if ( newInfo.isDir() ) {
		qDebug() << "D\tMove" << oldInfo.filePath() << "to" << newInfo.filePath();
		QModelIndex parent = currentProj->index( toFilePath );

		// Wait until dir is loaded. WARNING !!!!! with loop !!!!!!
		QEventLoop loop(this);
		connect( currentProj, SIGNAL(directoryLoaded(QString)), &loop, SLOT(quit()) );
		if ( currentProj->hasChildren(parent) && currentProj->rowCount(parent) == 0 && currentProj->canFetchMore(parent) ) {
			currentProj->fetchMore(parent);
			loop.exec();
		}

		for ( int i = 0 ; i < currentProj->rowCount(parent) ; i++ ) {
			QFileInfo info(parent.child(i,0).data(QFileSystemModel::FilePathRole).toString());
			onFileMoved(fromFilePath+"/"+info.fileName(), info.filePath());
		}
		return;
	}

	qDebug() << "F\tHas moved" << newInfo.filePath() << "from" << oldPackage << "to" << newPackage;

	QString oldImportLine;
	if ( ! oldPackage.isEmpty() )
		oldImportLine = "import\\s+"+oldPackage+"\\."+oldInfo.baseName()+"\\s*;";
	else
		oldImportLine = "import\\s+"+oldInfo.baseName()+"\\s*;";

	QString newPackageLine, newImportLine;
	if ( ! newPackage.isEmpty() ) {
		newPackageLine = "package "+newPackage+";";
		newImportLine = "import "+newPackage+"."+newInfo.baseName()+";";
	} else
		newImportLine = "import "+newInfo.baseName()+";";

	foreach ( QString filename, currentProj->files() ) {

		QsciScintillaExtended * sci = 0;
		if ( filename == toFilePath )
			sci = mw->fc->fileToDocument( fromFilePath );
		else if ( filename == fromFilePath )
			qDebug() << "BUGGGGGGGGGGGGGGGGGGGG";
		else
			sci = mw->fc->fileToDocument( filename );

		if ( sci == 0 ) {
			// We open the file
			QFile file(filename);

			if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
				continue;

			// We charge the file,
			sci = new QsciScintillaExtended(filename);
			QString text(file.readAll());
			file.close();
			sci->setText(text);
			sci->setModified(false);
		}

		if ( filename == toFilePath ) {
			// We update the filename
			sci->setFilename( toFilePath );

			// Si il y avait déjà un package
			if ( sci->findFirst("package\\s+[\\.\\w]+\\s*;",true,false,false,false,true,0,0) ) {
				// On le remplace
				sci->replaceAll("package\\s+[\\.\\w]+\\s*;", newPackageLine, true, true, false);

			// Sinon, si il n'y avait pas de déclaration de package et qu'il y en a une nouvelle
			} else if ( ! newPackage.isEmpty() ){
				// On cherche la fin des commentaires de début de fichier
				int i = 0;
				while (  i < sci->lines() && sci->isComment(i,0) )
					i++;

				if ( i < sci->lines() )
					sci->insertAt("\n"+newPackageLine+"\n",i,0);
				else
					sci->insertAt("\n"+newPackageLine+"\n\n",0,0);
			}
		} else
			sci->replaceAll(oldImportLine, newImportLine, true, true, false);

		// Si le document n'est pas un document ouvert,
		if ( mw->ui->tabWidget->indexOf(sci) == -1 ) {
			// We delete it
			mw->fc->saveFile(sci);
			sci->deleteLater();
		}
	}
}


/**
  Add the project the "recentsProjects" list.
  */
void ProjectController::addToRecentsProjects( const QString & projetPath)
{
	if ( projetPath.isEmpty() || d->recentsProjectsList.contains(projetPath) )
		return;

	d->recentsProjectsList.append(projetPath);

	QAction * action = new QAction( QDir(projetPath).dirName(), mw );
	action->setData(projetPath);
	connect(action, SIGNAL(triggered()), d->recentsProjectsMapper, SLOT(map()));
	d->recentsProjectsMapper->setMapping(action, projetPath);

	if ( d->recentsProjects->actions().isEmpty() )
		d->recentsProjects->addAction( action );
	else
		d->recentsProjects->insertAction(d->recentsProjects->actions().first(), action );

	//
	if ( d->recentsProjects->actions().size() > d->numberMaxOfRecentsProjects )
		d->recentsProjects->removeAction(d->recentsProjects->actions().last());
}



/**
  Remove the projects of the "recentsProjects" list.
  */
void ProjectController::removeOfRecentsProjects( const QString & projetPath )
{
	if ( projetPath.isEmpty() || ! d->recentsProjectsList.contains(projetPath) )
		return;

	d->recentsProjectsList.removeOne(projetPath);

	QAction * a = qobject_cast<QAction *>(d->recentsProjectsMapper->mapping(projetPath));
	if (a != 0) {
		d->recentsProjectsMapper->removeMappings(a);
		d->recentsProjects->removeAction(a);
	}

}


/**
  */
void ProjectController::saveProjectsState()
{
	QSettings settings;
	settings.beginGroup("project");

	if ( currentProj ) {
		settings.setValue("last project",currentProj->dir());
		currentProj->close();
	} else
		settings.setValue("last project","");

	// Save the last closed projects in settings
	int size = d->recentsProjects->actions().size();
	settings.beginWriteArray("recentsProjects",size);
	for (int i = 0 ; i < size ; i++) {
		settings.setArrayIndex(i);
		settings.setValue("project",d->recentsProjects->actions().at(i)->data());
	}
	settings.endArray();
}


/**
  Switch to this file if it is open.
  */
bool ProjectController::on_projectExplorator_clicked( QModelIndex index )
{
	QString filename = index.data(QFileSystemModel::FilePathRole).toString();

	if ( mw->fc->isOpenFile(filename) )
		mw->ui->tabWidget->setCurrentWidget( mw->fc->fileToDocument(filename));
	else
		return false;

	return true;

}


/**
  Open the file or switch to it if it is already open.
  */
bool ProjectController::on_projectExplorator_doubleClicked( QModelIndex index )
{
	// Return false if index is a dir
	if (currentProj->isDir(index))
		return false;
	// Else open the file
	else
		return mw->fc->openFile(index.data(QFileSystemModel::FilePathRole).toString());
}

/**
  When files are renamed, change the document filename if it is open;
  */
void ProjectController::onFileRenamed( const QString & path, const QString & oldName, const QString & newName )
{
	QsciScintillaExtended * sci = mw->fc->fileToDocument( path + "/" + oldName );
	if ( sci != 0 )
		sci->setFilename( path + "/" + newName );
}

/**
  When files are moved, change the document filename if it is open;
  */
void ProjectController::onFileMoved( const QString & oldFilePath, const QString & newFilePath )
{
	QsciScintillaExtended * sci = mw->fc->fileToDocument( oldFilePath );
	if ( sci != 0 )
		sci->setFilename( newFilePath );
}


/**
  Propose a menu when users ask for it.
  Users can
	- remove a package or a file,
	- add a package or a file,
	- rename a package or a file.
	- execute a skoor file.
  */
void ProjectController::onCustomContextMenuRequested( const QPoint & pos )
{
	QModelIndex index = mw->ui->projectExplorator->indexAt(pos);

	// If it's a dir
	if ( index.isValid() && currentProj->isDir( index ) ) {
		QAction * a = d->dirMenu->exec(mw->ui->projectExplorator->viewport()->mapToGlobal(pos));
		if ( a == 0 )
			return;

		if ( a->data().toString() == "rename" ) {
			// setEditable is set to false when editing is finished. See onItemRenameFinished()
			currentProj->setEditable(true);
			mw->ui->projectExplorator->edit(index);

		} else if ( a->data().toString() == "remove" ) {
			int ret = QMessageBox::question(mw, tr("Supprimer le package"),
				tr("Voulez-vous vraiment supprimer définitivement le package %1 ainsi que tout son contenu ?").
				arg(currentProj->fileName(index)), QMessageBox::Ok, QMessageBox::Cancel);
			if ( ret != QMessageBox::Ok )
				return;

			currentProj->rmdirRec(index);

		} else if ( a->data().toString() == "addPackage" ) {
			QModelIndex newPackage = currentProj->mkdir(index,"new_package");
			currentProj->setEditable(true); // setEditable is set to false when editing is finished. See onItemRenameFinished()
			mw->ui->projectExplorator->edit(newPackage);

		} else if ( a->data().toString() == "addClass" ) {
			//createNewClassFile();
		} else if ( a->data().toString() == "addSystem" ) {
			//createNewSystemFile();
		} else if ( a->data().toString() == "addRequest" ) {
			//createNewRequestFile();
		}

	} else if ( index.isValid() ) {
		QAction * a = d->fileMenu->exec(mw->ui->projectExplorator->viewport()->mapToGlobal(pos));
		if ( a == 0 )
			return;

		if ( a->data().toString() == "rename" ) {
			currentProj->setEditable(true); // setEditable is set to false when editing is finished. See onItemRenameFinished()
			mw->ui->projectExplorator->edit(index);

		} else if ( a->data().toString() == "remove" ) {
			int ret = QMessageBox::question(mw, tr("Supprimer le fichier"),
				tr("Voulez-vous vraiment supprimer définitivement le fichier\n%1 ?").
				arg(currentProj->fileName(index)), QMessageBox::Ok, QMessageBox::Cancel);
			if ( ret != QMessageBox::Ok )
				return;

			currentProj->remove(index);

		} else if ( a->data().toString() == "execute" && currentProj->fileInfo(index).suffix() == "skoor" ) {
			mw->bc->execute( index.data(Project::FilePathRole).toString() );
		}
	}
}


/**
  This slot is called when user has called "rename" in the project explorator,
  and the editing is finished.
  We reset the model to non-editable.
  \see onCustomContextMenuRequested()
  */
void ProjectController::onItemRenameFinished()
{
	currentProj->setEditable(false);
}
