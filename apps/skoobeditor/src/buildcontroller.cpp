#include "buildcontroller.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filecontroller.h"
#include "editcontroller.h"
#include "searchcontroller.h"
#include "viewcontroller.h"
#include "projectcontroller.h"
#include "qsciscintillaextended.h"
#include "skoolinterpretation.h"
#include "skoorinterpretation.h"
#include "prmtreemodel.h"

#include <agrum/prm/skool/SkoolReader.h>
#include <agrum/core/errorsContainer.h>

using namespace gum::prm::skool;
using namespace gum::prm::skoor;

#include <QListWidget>
#include <QProcess>
#include <QMessageBox>
#include <QMutex>
#include <QDebug>

/* ************************************************************************* */

/**
  Parsing lancé par utilisateur et automatiquement.

  Quand lancé par UTILISATEUR :
  Visualisation des erreurs et warnings dans le dock.

  Quand lancé AUTOMATIQUEMENT :
  Visualisation des erreurs et warnings dans le document.

  Quand ÉXÉCUTION :
  Affichage des résultats dans le dock.

  bool isManual
  bool isExecution
  AbstractParser * parser
  Juste quand on le crée on connecte les bons signaux.
  Méthode start to parse, et une exécution.
  */

/*
  */
struct BuildController::PrivateData {
	QListWidget * msg;

	bool isExecution;
	AbstractParser * parser;
	SkoorInterpretation * skoorThread;
	SkoolInterpretation  * skoolThread;

	QMutex mutex;

	bool autoSyntaxCheck;
	QTimer timer;

	QSharedPointer<PRMTreeModel> prmModel;
};


/// Contructor
BuildController::BuildController(MainWindow * mw, QObject *parent) :
	QObject(parent),
	mw(mw),
	d(new PrivateData)
{
	d->msg = mw->ui->buildDock;
	d->isExecution = false;
	d->skoorThread = 0;
	d->skoolThread = 0;

	d->autoSyntaxCheck = false;
	d->parser = 0;

	d->timer.setSingleShot(true);
	d->timer.setInterval(1000);

	// ###########################################################
	// Set local to "C" instead local setting (Qt change it) to
	// read skool files, to use '.' instead ',' for float number.
	setlocale( LC_NUMERIC, "C" );
	// ###########################################################

	connect( d->msg, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onMsgDoubleClick(QModelIndex)) );
	connect( mw->ui->actionHideDocks, SIGNAL(triggered()), this, SLOT(hideBuildMessage()) );

	connect( mw->ui->actionBuild, SIGNAL(triggered()), this, SLOT(checkSyntax()) );
	connect( mw->ui->actionExecute, SIGNAL(triggered()), this, SLOT(execute()) );

	connect( &d->timer, SIGNAL(timeout()), this, SLOT(startAutoParsing()) );
	connect( mw->fc, SIGNAL(fileSaved(QString,QsciScintillaExtended*)), this, SLOT(startAutoParsing()) );
	connect( mw->fc, SIGNAL(fileClosed(QString)), this, SLOT(onDocumentClosed(QString)) );
	connect( mw->fc, SIGNAL(fileRenamed(QString,QString,QsciScintillaExtended*)), this, SLOT(onDocumentClosed(QString)) );

	// Must be start after project triggerInit
	QTimer::singleShot( 500, this, SLOT(triggerInit()) );
}


/// Destructor
BuildController::~BuildController()
{
	d->prmModel.clear();
	delete d;
}

void BuildController::triggerInit()
{
	// Start it in case there is only one document
	connect( mw->ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(startAutoParsing(int)) );
}

/* ************************************************************************* */

bool BuildController::isAutoSyntaxCheck() const
{
	return d->autoSyntaxCheck;
}

void BuildController::setAutoSyntaxCheck( bool isAuto )
{
	if ( isAuto == d->autoSyntaxCheck )
		return;

	d->autoSyntaxCheck = isAuto;
	if ( isAuto )
		startAutoParsing(-1);
	else {
		d->timer.stop();
		d->prmModel.clear();
	}
}

QSharedPointer<PRMTreeModel> BuildController::currentDocumentModel()
{
	return d->prmModel;
}

const QSharedPointer<PRMTreeModel> BuildController::currentDocumentModel() const
{
	return d->prmModel;
}

/**
  */
void BuildController::checkSyntax( const QString & filename)
{
	// Open file, check it, and close it if it is not already open.
	if ( ! mw->fc->isOpenFile(filename) ) {
		int currentIndex = mw->ui->tabWidget->currentIndex();
		// If can't open file, do nothing
		if ( ! mw->fc->openFile(filename) ) return;
		checkSyntax();
		mw->fc->closeFile();
		mw->ui->tabWidget->setCurrentIndex(currentIndex);

	} else {
		checkSyntax( mw->fc->fileToDocument(filename) );
	}
}

/**
  */
void BuildController::checkSyntax( QsciScintillaExtended * sci )
{
	if ( sci == 0 ) {
		if ( ! mw->fc->hasCurrentDocument() )
			return;
		else
			sci = mw->fc->currentDocument();
	}

	if ( sci->lexerEnum() == QsciScintillaExtended::Skoor )
		return execute( sci->filename(), true );


	d->msg->clear();
	d->msg->addItem( tr("Vérifie la syntaxe de '%1' ...").arg( sci->title() ) );
	d->msg->item(0)->setTextColor(Qt::blue);

	// Erase all error marks in editors
	foreach( QsciScintillaExtended * sci, mw->fc->openDocuments() )
		sci->clearAllErrors();

	if ( d->skoolThread != 0 ) {
		disconnect( d->skoolThread, SIGNAL(finished()), this, SLOT(onSyntaxCheckFinished()) );
		connect( d->skoolThread, SIGNAL(finished()), this, SLOT(deleteLater()) );
		d->skoolThread = 0;
	}

	d->skoolThread = new SkoolInterpretation(sci, this);

	// Set paths
	if ( mw->pc->isOpenProject() )
		d->skoolThread->addClassPaths( mw->pc->currentProject()->paths() );

	connect( d->skoolThread, SIGNAL(finished()), this, SLOT(onSyntaxCheckFinished()) );
	d->isExecution = false;
	d->skoolThread->parse();

	mw->vc->setBuildDockVisibility(true);
}


/**
  */
void BuildController::onSyntaxCheckFinished()
{
	// If there was a problem
	if ( mw->fc->currentDocument() != d->skoolThread->document() )
		return;

	const gum::ErrorsContainer & errors = d->skoolThread->errors();

	const QString filename = d->skoolThread->document()->filename();

	for ( int i = 0, size = errors.count() ; i < size ; i++ ) {
		const gum::ParseError & e = errors.getError(i);

		QString s, errFilename, line;
		errFilename = QString::fromStdString(e.filename);
		line = QString::number(e.line);
		s = QString::fromStdString(e.toString());

		// Handle error filename problem (file not saved, etc)
		if ( errFilename.isEmpty() ) {
			errFilename = filename;
			s.prepend( errFilename );
		} else if ( errFilename == "anonymous buffer" ) {
			errFilename = filename;
			s.replace( "anonymous buffer", filename );
		}

		QListWidgetItem * item = new QListWidgetItem( s, d->msg) ;
		item->setData( Qt::UserRole, errFilename );
		item->setData( Qt::UserRole + 1, line );
	}


	if ( errors.count() == 0 ) {
		d->msg->addItem(tr("Syntaxe vérifiée. Il n'y a pas d'erreur."));
		d->msg->item(1)->setTextColor(Qt::blue);
	} else {
		d->msg->addItem(tr("Syntaxe vérifiée. Il y a %1 erreurs.").arg(errors.count()));
		d->msg->item(0)->setTextColor(Qt::red);
		d->msg->item(d->msg->count() - 1)->setTextColor(Qt::red);
	}

	d->skoolThread->deleteLater();
	d->skoolThread = 0;
}


/**
  */
void BuildController::execute( const QString & filename, bool checkSyntaxOnly )
{
	// Open file, check it, and close it if it is not already open.
	if ( ! mw->fc->isOpenFile(filename) ) {
		int currentIndex = mw->ui->tabWidget->currentIndex();
		// If can't open file, do nothing
		if ( ! mw->fc->openFile(filename) ) return;
		execute( 0, checkSyntaxOnly );
		mw->fc->closeFile();
		mw->ui->tabWidget->setCurrentIndex(currentIndex);

	} else {
		execute( mw->fc->fileToDocument(filename), checkSyntaxOnly );
	}
}


/**
  */
void BuildController::execute( QsciScintillaExtended * sci, bool checkSyntaxOnly )
{
	// Check document
	if ( sci == 0 ) {
		if ( ! mw->fc->hasCurrentDocument() )
			return;
		else
			sci = mw->fc->currentDocument();
	}

	// Create temp file to execute some class or system files.
//	if ( sci->lexerEnum() == QsciScintillaExtended::Skool ) {
//		// If file contains a system declaration
//		if ( sci->text().contains(QRegExp("^\s*system\s+\w+\s+{")) ) {

//		// Else if file contains a class definition
//		} else if ( sci->text().contains(QRegExp("^\s*class\s+\w+\s+.*{")) ) {

//		}
//		return;
//	}

	// Clear build dock and inform start of process
	d->msg->clear();
	if ( ! checkSyntaxOnly )
		d->msg->addItem( tr("Éxécution de '%1' ...").arg(sci->title()) );
	else
		d->msg->addItem(tr("Vérifie la syntaxe de '%1' ...").arg(sci->title()));
	d->msg->item(0)->setTextColor(Qt::blue);

	// Erase all error marks in editors
	foreach( QsciScintillaExtended * sci, mw->fc->openDocuments() )
		sci->clearAllErrors();

	// Delete previous thread
	if ( d->skoorThread != 0 ) {
		disconnect( d->skoorThread, 0, this, 0 );
		connect( d->skoorThread, SIGNAL(finished()), d->skoorThread, SLOT(deleteLater()) );
		d->skoorThread = 0;
	}

	// Create thread and set text
	d->skoorThread = new SkoorInterpretation( sci, this );
	d->skoorThread->setSyntaxMode(checkSyntaxOnly);

	// Set classpaths
	if ( mw->pc->isOpenProject() && mw->pc->currentProject()->isInside(sci->filename()) )
		d->skoorThread->addClassPaths( mw->pc->currentProject()->paths() );

	// Connect and start thread.
	connect( d->skoorThread, SIGNAL(finished()), this, SLOT(onInterpretationFinished()) );
	d->skoorThread->parse();
	d->isExecution = ! checkSyntaxOnly;

	// Show build dock
	mw->vc->setBuildDockVisibility(true);
}


/**
  */
void BuildController::onInterpretationFinished()
{
	if ( mw->fc->currentDocument() != d->skoorThread->document() )
		return;

	const gum::ErrorsContainer & errors = d->skoorThread->errors();
	const QString filename = d->skoorThread->document()->filename();

	// On affiche les errors et les warnings
	for ( int i = 0, size = errors.count() ; i < size ; i++ ) {
		const gum::ParseError & err = errors.getError(i);

		QString errFilename = QString::fromStdString( err.filename );
		QString line = QString::number( err.line );
		QString s = QString::fromStdString( err.toString() );

		if ( errFilename.isEmpty() ) {
			errFilename = filename;
			s.prepend( errFilename + ":" );
		} else if ( errFilename == "anonymous buffer" ) {
			errFilename = filename;
			s.replace( "anonymous buffer", filename );
		}

		QListWidgetItem * item = new QListWidgetItem(s, d->msg) ;
		item->setData( Qt::UserRole, errFilename );
		item->setData( Qt::UserRole + 1, line );
	}

	// If there was no errors (just warnings)
	if ( errors.error_count == 0 ) {

		if ( d->skoorThread->isSyntaxMode() )
			d->msg->addItem(tr("Syntaxe vérifiée. Il n'y a pas d'erreur."));
		else {
			// We show all results
			const vector< pair<string,QueryResult> > & results = d->skoorThread->results();
			for ( size_t i = 0 ; i < results.size() ; i++ ) {
				const QString & query = QString::fromStdString( results[i].first );
				d->msg->addItem( tr("%1").arg(query) );
				d->msg->item(d->msg->count() - 1)->setTextColor(Qt::darkYellow);
				const QueryResult & result = results[i].second;
				for ( size_t j = 0 ; j < result.size() ; j++ ) {
					float val = result[j].second;
					d->msg->addItem( tr("%1 : %2").arg( QString::fromStdString(result[j].first) ).arg(val) );
				}
			}

			d->msg->addItem(tr("Éxécution terminée."));
		}
		d->msg->item(d->msg->count() - 1)->setTextColor(Qt::blue);

	} else { //  errors->error_count != 0

		if ( d->skoorThread->isSyntaxMode() )
			d->msg->addItem(tr("Syntaxe vérifiée. Il y a %1 erreurs.").arg(errors.error_count));
		else
			d->msg->addItem(tr("Éxécution interrompue. Il y a %1 erreurs.").arg(errors.error_count));
		d->msg->item(0)->setTextColor(Qt::red);
		d->msg->item(d->msg->count() - 1)->setTextColor(Qt::red);

		d->isExecution = false;

	} //  errors->error_count != 0

	d->skoorThread->deleteLater();
	d->skoorThread = 0;
}


/**
  */
void BuildController::hideBuildMessage()
{
	// Erase all error marks in editors
	foreach( QsciScintillaExtended * sci, mw->fc->openDocuments() )
		sci->clearAllErrors();

	d->msg->clear();
	mw->vc->setBuildDockVisibility(false);
}


/**
 */
void BuildController::onMsgDoubleClick(QModelIndex index)
{
	// Do anything if it is the first or the last message
	if ( d->isExecution || ! index.isValid() ||
		 index.row() == 0 || index.row() == d->msg->count() - 1)
		return;

	QString filename = index.data( Qt::UserRole ).toString();
	int line = index.data( Qt::UserRole + 1 ).toInt() - 1; // lines start at 0 in scintilla

	// Open the file (if filename is empty take the current document)
	if ( ! filename.isEmpty() && ! mw->fc->openFile(filename) )
		return;

	// Go to the line
	QsciScintillaExtended * sci = mw->fc->currentDocument();
	if ( sci == 0 )
		return;
	sci->setCursorPosition(line, 0);

	sci->setError(line);
	sci->setFocus();
}

/**
  Ce slot est appelé quand on change de document ou quand la correction est fini.
  Le nouvel index est passé en argument.
  -1 si il n'y a plus de document,
  -2 si la correction est fini.
  */
void BuildController::startAutoParsing(int i)
{
	if ( ! d->autoSyntaxCheck )
		return;

	// Stop timer if running, to prevent multiple thread.
	d->timer.stop();

	//////////////////////////////////////////////////////////////

	// Delete the previous threads

	// If a previous skoor thread is still running
	if ( d->parser ) {
		// We reconnect it to self delete
		disconnect( d->parser, 0, this, 0 );
		connect( d->parser, SIGNAL(finished()), d->parser, SLOT(deleteLater()) );
	}
	if ( d->parser && d->parser->isFinished()  )
		d->parser->deleteLater();

	d->parser = 0;

	//////////////////////////////////////////////////////////////

	if ( ! mw->fc->hasCurrentDocument() )
		return;

	QString filename = mw->fc->currentDocument()->filename();

	// Create a new thread
	if ( mw->fc->currentDocument()->lexerEnum() == QsciScintillaExtended::Skoor )
		// Create new document and connect it
		d->parser = new SkoorInterpretation( mw->fc->currentDocument(), this );

	else if ( mw->fc->currentDocument()->lexerEnum() == QsciScintillaExtended::Skool )
		// Create new document and connect it
		d->parser = new SkoolInterpretation( mw->fc->currentDocument(), this );

	connect( d->parser, SIGNAL(finished()), this, SLOT(onAutoParsingFinished()) );

	// Set paths
	if ( mw->pc->isOpenProject() && mw->pc->currentProject()->isInside(filename) )
		d->parser->addClassPaths( mw->pc->currentProject()->paths() );

	// Start it
	d->parser->parse(QThread::LowPriority);
}


void BuildController::onAutoParsingFinished()
{
	if ( d->parser == 0 || d->parser->document() != mw->fc->currentDocument() )
		return;

	const gum::ErrorsContainer & errors = d->parser->errors();

	if ( errors.count() == 0 ) {
		d->prmModel.clear();
		d->prmModel = d->parser->prm();
	}

	mw->fc->currentDocument()->clearAllSyntaxErrors();

	for ( int i = 0, size = errors.count() ; i < size ; i++ ) {
		const gum::ParseError & err = errors.getError(i);
		if ( err.filename.empty() || mw->fc->currentDocument() == d->parser->document() ) // => == fichier skool
			mw->fc->currentDocument()->setSyntaxError(err.line - 1);
	}

	if ( ! d->timer.isActive() )
		d->timer.start();

	if ( ! d->timer.isActive() )
		d->timer.start();
}

void BuildController::onDocumentClosed( const QString & filename )
{
	QFile::remove( filename + ".bak" );
}
