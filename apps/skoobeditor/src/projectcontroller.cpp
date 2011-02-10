#include "projectcontroller.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filecontroller.h"
#include "editcontroller.h"
#include "searchcontroller.h"
#include "viewcontroller.h"
#include "buildcontroller.h"
#include "qsciscintillaextended.h"

#include "newprojectdialog.h"
#include "projectproperties.h"

#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSignalMapper>
#include <QTimer>

#include <QDebug>

// Private data
struct ProjectController::PrivateData {
	int numberMaxOfRecentsProjects;
	QMenu * recentsProjects;
	QList<QString> recentsProjectsList;
	QSignalMapper * recentsProjectsMapper;
	ProjectProperties * projectProperties;
};


/// Constructor
ProjectController::ProjectController(MainWindow * mw, QObject *parent) :
		QObject(parent),
		mw(mw),
		currentProj(0),
		d(new PrivateData)
{
	mw->ui->projectExplorator->setVisible(false);

	d->projectProperties = 0;
	mw->ui->actionProjectProperties->setEnabled(false);

	d->recentsProjects = new QMenu(mw);
	d->recentsProjectsMapper = new QSignalMapper(mw);

	// Construct "Recent project" menu
	mw->ui->actionRecentProject->setMenu( d->recentsProjects );
	connect(d->recentsProjectsMapper,SIGNAL(mapped(QString)),this,SLOT(openProject(QString)));

	connect( mw->ui->projectExplorator, SIGNAL(clicked(QModelIndex)), this, SLOT(on_projectExplorator_clicked(QModelIndex)) );
	connect( mw->ui->projectExplorator, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_projectExplorator_doubleClicked(QModelIndex)));

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

	if (projectpath.isEmpty())
		return;

	QDir qDir(projectpath);
	if ( ! qDir.exists(qDir.dirName()+".skoop")) {
		int rep = QMessageBox::warning(mw,tr("Attention"),tr("Ce répertoire ne contient pas de fichier projet.\nLe créer ?"),
							 QMessageBox::Cancel, QMessageBox::Ok);
		if ( rep == QMessageBox::Cancel )
			return;
	}

	if ( currentProj )
		closeProject();

	currentProj = new Project(qDir.absolutePath(),this);
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
