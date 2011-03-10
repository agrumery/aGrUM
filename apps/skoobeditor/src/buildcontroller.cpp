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
#include <agrum/prm/skoor/SkoorInterpreter.h>
#include <agrum/core/errorsContainer.h>

using namespace gum::prm::skool;
using namespace gum::prm::skoor;

#include <QListWidget>
#include <QProcess>
#include <QMessageBox>
#include <QMutex>
#include <QDebug>

/* ************************************************************************* */

/*
  */
struct BuildController::PrivateData {
	QListWidget * msg;

	bool isExecution;
	SkoorInterpretation * skoorThread;
	SkoolInterpretation  * skoolThread;

	QMutex mutex;

	bool autoSyntaxCheck;
	SkoorInterpretation * skoorSyntaxThread;
	SkoolInterpretation  * skoolSyntaxThread;
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
	d->skoorSyntaxThread = 0;
	d->skoolSyntaxThread = 0;

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

	connect( &d->timer, SIGNAL(timeout()), this, SLOT(startAutoSyntaxCheckThread()) );
	connect( mw->fc, SIGNAL(fileSaved(QString,QsciScintillaExtended*)), this, SLOT(startAutoSyntaxCheckThread()) );

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
	connect( mw->ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(startAutoSyntaxCheckThread(int)) );
	setAutoSyntaxCheck(true);
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
		startAutoSyntaxCheckThread(-1);
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

	// and set text
	d->skoolThread->setDocument( sci->text() );

	// Set paths
	if ( ! sci->filename().isEmpty() )
		d->skoolThread->addPath( QFileInfo(sci->filename()).path() );

	if ( mw->pc->isOpenProject() )
		d->skoolThread->setPath( mw->pc->currentProject()->paths() );

	connect( d->skoolThread, SIGNAL(finished()), this, SLOT(onSyntaxCheckFinished()) );
	d->skoolThread->start();
	d->isExecution = false;

	mw->vc->setBuildDockVisibility(true);
}


/**
  */
void BuildController::onSyntaxCheckFinished()
{
	// If there was a problem
	if ( d->skoolThread->reader() == 0 || mw->fc->currentDocument() != d->skoolThread->document() )
		return;

	const gum::ErrorsContainer & errors = d->skoolThread->reader()->getErrorsContainer();
	const QString title = d->skoolThread->documentTitle();

	for ( int i = 0, size = errors.count() ; i < size ; i++ ) {
		const gum::ParseError & e = errors.getError(i);

		QString s, errFilename, line;
		errFilename = QString::fromStdString(e.filename);
		line = QString::number(e.line);
		s = QString::fromStdString(e.toString());

		// Handle error filename problem (file not saved, etc)
		if ( errFilename.isEmpty() ) {
			errFilename = title;
			s.prepend( errFilename );
		} else if ( errFilename == "anonymous buffer" ) {
			errFilename = title;
			s.replace( "anonymous buffer", title );
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

	d->msg->scrollToBottom();
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
		disconnect( d->skoorThread, SIGNAL(finished()), this, SLOT(onInterpretationFinished()) );
		//d->skoorThread->deleteLater();
		d->skoorThread = 0;
	}

	// Create thread and set text
	d->skoorThread = new SkoorInterpretation( sci, this, checkSyntaxOnly );
	d->skoorThread->setDocument( sci->text() );

	// Set paths
	if ( ! sci->filename().isEmpty() )
		d->skoorThread->addPath( QFileInfo(sci->filename()).path() );

	if ( mw->pc->isOpenProject() && mw->pc->currentProject()->isInside(sci->filename()) )
		d->skoorThread->setPath( mw->pc->currentProject()->paths() );

	// Connect and start thread.
	connect( d->skoorThread, SIGNAL(finished()), this, SLOT(onInterpretationFinished()) );
	d->skoorThread->start();
	d->isExecution = ! checkSyntaxOnly;

	// Show build dock
	mw->vc->setBuildDockVisibility(true);
}


/**
  */
void BuildController::onInterpretationFinished()
{
	const SkoorInterpreter * interpreter = d->skoorThread->interpreter();
	if ( interpreter == 0 || mw->fc->currentDocument() != d->skoorThread->document() )
		return;

	// If there was a problem
	if ( interpreter->errors() == 0 ) {

		if ( d->skoorThread->isSyntaxMode() )
			d->msg->addItem(tr("Syntaxe vérifiée. Il n'y a pas d'erreur."));
		else {
			// We show all results
			const vector< pair<string,QueryResult> > & results = interpreter->results();
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

	} else { //  interpreter->errors() != 0

		QString documentTitle = d->skoorThread->documentTitle();

		for ( int i = 0, size = interpreter->errors() ; i < size ; i++ ) {
			const gum::ParseError & err = interpreter->getError(i);

			QString errFilename = QString::fromStdString( err.filename );
			QString line = QString::number( err.line );
			QString s = QString::fromStdString( err.toString() );

			if ( errFilename.isEmpty() ) {
				errFilename = documentTitle;
				s.prepend( errFilename + ":" );
			}

			QListWidgetItem * item = new QListWidgetItem(s, d->msg) ;
			item->setData( Qt::UserRole, errFilename );
			item->setData( Qt::UserRole + 1, line );
		}

		if ( d->skoorThread->isSyntaxMode() )
			d->msg->addItem(tr("Syntaxe vérifiée. Il y a %1 erreurs.").arg(interpreter->errors()));
		else
			d->msg->addItem(tr("Éxécution interrompue. Il y a %1 erreurs.").arg(interpreter->errors()));
		d->msg->item(0)->setTextColor(Qt::red);
		d->msg->item(d->msg->count() - 1)->setTextColor(Qt::red);

		d->isExecution = false;

	} //  interpreter->errors() != 0

	d->skoorThread->deleteLater();
	d->skoorThread = 0;

	//d->msg->scrollToBottom();
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
void BuildController::startAutoSyntaxCheckThread(int i)
{
	if ( ! d->autoSyntaxCheck )
		return;

	// Stop timer if running, to prevent multiple thread.
	d->timer.stop();

	// If current document changed, delete the previous thread
	if ( i != -2 ) {

		//////////////////////////////////////////////////////////////

		// If a previous skoor thread is still running
		if ( d->skoorSyntaxThread ) {
			// We reconnect it to self delete
			disconnect( d->skoorSyntaxThread, SIGNAL(finished()), this, SLOT(onSkoorSyntaxThreadFinished()) );
			connect( d->skoorSyntaxThread, SIGNAL(finished()), d->skoorSyntaxThread, SLOT(deleteLater()) );

		// If previous skoor thread is not running, we delete it
		}
		if ( d->skoorSyntaxThread && d->skoorSyntaxThread->isFinished()  )
			d->skoorSyntaxThread->deleteLater();

		d->skoorSyntaxThread = 0;

		//////////////////////////////////////////////////////////////

		// If a previous skool thread is still running
		if (  d->skoolSyntaxThread ) {
			// We reconnect it to self delete
			disconnect( d->skoolSyntaxThread, SIGNAL(finished()), this, SLOT(onSkoolSyntaxThreadFinished()) );
			connect( d->skoolSyntaxThread, SIGNAL(finished()), d->skoolSyntaxThread, SLOT(deleteLater()) );

		// If previous skool thread is not running, we delete it
		}
		if ( d->skoolSyntaxThread && d->skoolSyntaxThread->isFinished() )
			d->skoolSyntaxThread->deleteLater();

		d->skoolSyntaxThread = 0;
	}

	if ( ! mw->fc->hasCurrentDocument() )
		return;

	// Check if we are always on the same document
	if ( d->skoorSyntaxThread != 0 &&
		 d->skoorSyntaxThread->document() != mw->fc->currentDocument() )
		return;
	else if ( d->skoolSyntaxThread != 0 &&
		 d->skoolSyntaxThread->document() != mw->fc->currentDocument() )
		return;

	QString filename = mw->fc->currentDocument()->filename();

	// Create a new thread
	if ( mw->fc->currentDocument()->lexerEnum() == QsciScintillaExtended::Skoor ) {
		// Create new document and connect it
		if ( d->skoorSyntaxThread == 0 ) {
			d->skoorSyntaxThread = new SkoorInterpretation( mw->fc->currentDocument(), this );
			connect( d->skoorSyntaxThread, SIGNAL(finished()), this, SLOT(onSkoorSyntaxThreadFinished()) );

			if ( mw->pc->isOpenProject() && mw->pc->currentProject()->isInside(filename) )
				d->skoorSyntaxThread->setPath( mw->pc->currentProject()->paths() );

			// Set paths
			if ( ! filename.isEmpty() )
				d->skoorSyntaxThread->addPath( QFileInfo(filename).path() );
		}

		// Set text
		d->skoorSyntaxThread->setDocument( mw->fc->currentDocument()->text() );

		// Start it
		d->skoorSyntaxThread->start(QThread::LowPriority);

	} else if ( mw->fc->currentDocument()->lexerEnum() == QsciScintillaExtended::Skool ) {
		if ( d->skoolSyntaxThread == 0 ) {
			// Create new document and connect it
			d->skoolSyntaxThread = new SkoolInterpretation( mw->fc->currentDocument(), this );
			connect( d->skoolSyntaxThread, SIGNAL(finished()), this, SLOT(onSkoolSyntaxThreadFinished()) );

			// Set paths
			if ( ! filename.isEmpty() )
				d->skoolSyntaxThread->addPath( QFileInfo(filename).path() );

			if ( mw->pc->isOpenProject() && mw->pc->currentProject()->isInside(filename) )
				d->skoolSyntaxThread->setPath( mw->pc->currentProject()->paths() );
		}

		// Set text
		d->skoolSyntaxThread->setDocument( mw->fc->currentDocument()->text() );

		// Start it
		d->skoolSyntaxThread->start(QThread::LowPriority);
	}
}

/**
  */
void BuildController::onSkoorSyntaxThreadFinished()
{
	if ( d->skoorSyntaxThread == 0 )
		return;

	const SkoorInterpreter * interpreter = d->skoorSyntaxThread->interpreter();
	if ( interpreter == 0 )
		return;

	//
	if ( interpreter->errors()== 0 ) {
		d->prmModel.clear();
		d->prmModel = QSharedPointer<PRMTreeModel>(new PRMTreeModel( interpreter->prm(), this ));
	}

	mw->fc->currentDocument()->clearAllSyntaxErrors();

	for ( int i = 0, size = interpreter->errors() ; i < size ; i++ ) {
		const gum::ParseError & err = interpreter->getError(i);
		if ( err.filename.empty() || mw->fc->currentDocument() == d->skoorSyntaxThread->document() ) // => == fichier skoor
			mw->fc->currentDocument()->setSyntaxError(err.line - 1);
	}

	if ( ! d->timer.isActive() )
		d->timer.start();
}


/**
  */
void BuildController::onSkoolSyntaxThreadFinished()
{
	if ( d->skoolSyntaxThread == 0 || d->skoolSyntaxThread->reader() == 0 ||
		 ! mw->fc->hasCurrentDocument() )
		return;

	gum::ErrorsContainer errors = d->skoolSyntaxThread->reader()->getErrorsContainer();

	if ( errors.count() == 0 ) {
		d->prmModel.clear();
		d->prmModel = QSharedPointer<PRMTreeModel>(new PRMTreeModel( d->skoolSyntaxThread->reader()->prm(), this ));
	}

	mw->fc->currentDocument()->clearAllSyntaxErrors();

	for ( int i = 0, size = errors.count() ; i < size ; i++ ) {
		const gum::ParseError & err = errors.getError(i);
		if ( err.filename.empty() || mw->fc->currentDocument() == d->skoolSyntaxThread->document() ) // => == fichier skool
			mw->fc->currentDocument()->setSyntaxError(err.line - 1);
	}

	if ( ! d->timer.isActive() )
		d->timer.start();
}
