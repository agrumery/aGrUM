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

#include <QDebug>


/*
 Réflections sur les interactions dans l'explorateur de projet :

 CLIC DROIT
 - sur un dossier -> menu avec
   * renommer (renomme dans tout le projet)
   * supprimer (confirmation, supprime tous les fichiers et dossiers dedans, indique les références dans le projet, source d'erreurs)
   * nouveau package
   * nouveau fichier (suivant le package, crée le bon type)
 - sur un fichier -> menu avec
   * renommer (renomme dans tout le projet)
   * déplacer (change le package dans tout le projet)
   * supprimer (confirmation, indique les références dans le projet, source d'erreurs)
   * éxécuter (uniquement pour le fichier SKOOR)

  DRAG AND DROP
  - d'un dossier -> renomme tout.
  - d'un fichier -> renomme tout.

  Un fichier peut être renommé :
	1) par l'explorateur de projet;
	2) par sauvegarder sous.

  Quand un fichier est renommé :
	On change la référence dans le fichier même (projet ou pas). -> FileController
	On change la référence dans les autres fichiers que si projet. -> ProjectController

  Quand un fichier est déplacé :
	On change le package partout que si projet. -> ProjectController


 */

// Private data
struct ProjectController::PrivateData {
	int numberMaxOfRecentsProjects;
	QMenu * recentsProjects;
	QList<QString> recentsProjectsList;
	QSignalMapper * recentsProjectsMapper;
	ProjectProperties * projectProperties;
	QMenu * fileMenu;
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
	QFileInfo oldInfo(path + "/" + oldName), newInfo(path + "/" + newName);

	if ( oldName == newName )
		return;

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
		QsciScintillaExtended * sci;

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
}

/**
  When files are moved, change the document filename if it is open;
  */
void ProjectController::onFileMoved( const QString & oldFilePath, const QString & newFilePath )
{
	QFileInfo oldInfo(oldFilePath), newInfo(newFilePath);

	// On vérifie si le répertoire a changé
	if ( oldInfo.absolutePath() == newInfo.absolutePath() )
		return;

	QString oldPackage = currentProject()->rootDirectory().relativeFilePath(oldInfo.absolutePath());
	QString newPackage = currentProject()->rootDirectory().relativeFilePath(newInfo.absolutePath());
	oldPackage.replace("/",".");
	newPackage.replace("/",".");

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

		QsciScintillaExtended * sci = mw->fc->fileToDocument( filename );
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

		if ( filename == oldFilePath ) {
			// We update the filename
			sci->setFilename( newFilePath );

			// Si il y avait déjà un package
			if ( sci->findFirst("package\\s+\\w+\\s*;",true,false,false,false,true,0,0) ) {
				// On le remplace
				sci->replaceAll("package\\s+\\w+\\s*;", newPackageLine, true, true, false);

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
	if ( currentProj->isDir( index ) ) {
		;
	} else {
		QAction * a = d->fileMenu->exec(mw->ui->projectExplorator->viewport()->mapToGlobal(pos));
		if ( a == 0 )
			return;

		if ( a->data().toString() == "rename" ) {
			currentProj->setEditable(true); // setEditable is set to false when editing is finished. See onItemRenameFinished()
			mw->ui->projectExplorator->edit(index);

		} else if ( a->data().toString() == "remove" ) {
			int ret = QMessageBox::question(mw, tr("Supprimer le fichier"),
											tr("Voulez-vous vraiment supprimer définitivement le fichier\n%1 ?").arg(currentProj->fileName(index)), QMessageBox::Ok, QMessageBox::Cancel);
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
