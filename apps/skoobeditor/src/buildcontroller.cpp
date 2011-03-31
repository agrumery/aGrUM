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

	bool autoSyntaxCheck;
	bool isAuto;
	AbstractParser * parser;
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
	d->isAuto = false;
	d->parser = 0;

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

	connect( &d->timer, SIGNAL(timeout()), this, SLOT(startParsing()) );
	connect( mw->fc, SIGNAL(fileSaved(QString,QsciScintillaExtended*)), this, SLOT(startParsing()) );
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
	connect( mw->ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(startParsing()) );
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
		startParsing(-1);
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
	if ( mw->fc->isOpenFile(filename) ) {
		checkSyntax( mw->fc->fileToDocument(filename) );

	} else {
		int currentIndex = mw->ui->tabWidget->currentIndex();
		// If can't open file, do nothing
		if ( ! mw->fc->openFile(filename) ) return;
		checkSyntax();
		mw->fc->closeFile();
		mw->ui->tabWidget->setCurrentIndex(currentIndex);
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

	d->msg->clear();
	d->msg->addItem( tr("Vérifie la syntaxe de '%1' ...").arg( sci->title() ) );
	d->msg->item(0)->setTextColor(Qt::blue);

	// Erase all error marks in editors
	foreach( QsciScintillaExtended * sci, mw->fc->openDocuments() )
		sci->clearAllErrors();

	startParsing(false,false);

	mw->vc->setBuildDockVisibility(true);
}


/**
  */
void BuildController::execute( const QString & filename )
{
	// Open file, check it, and close it if it is not already open.
	if ( mw->fc->isOpenFile(filename) ) {
		execute( mw->fc->fileToDocument(filename) );

	} else {
		int currentIndex = mw->ui->tabWidget->currentIndex();
		// If can't open file, do nothing
		if ( ! mw->fc->openFile(filename) ) return;
		execute(); // => currentDocument
		mw->fc->closeFile();
		mw->ui->tabWidget->setCurrentIndex(currentIndex);
	}
}


/**
  */
void BuildController::execute( QsciScintillaExtended * sci )
{
	// Check document
	if ( sci == 0 ) {
		if ( ! mw->fc->hasCurrentDocument() )
			return;
		else
			sci = mw->fc->currentDocument();
	}

	// Clear build dock and inform start of process
	d->msg->clear();
	d->msg->addItem( tr("Éxécution de '%1' ...").arg(sci->title()) );
	d->msg->item(0)->setTextColor(Qt::blue);

	// Erase all error marks in editors
	foreach( QsciScintillaExtended * sci, mw->fc->openDocuments() )
		sci->clearAllErrors();

	startParsing(false,true);

	// Show build dock
	mw->vc->setBuildDockVisibility(true);
}

/**
  Ce slot est appelé quand on change de document ou quand la correction est fini.
  */
void BuildController::startParsing( bool isAuto, bool isExecution )
{
	/**
	startParsing( bool isAuto = true, bool isExecution = false )
	return if no currentDocument
	*/
	if ( ! mw->fc->hasCurrentDocument() )
		return;

	// Stop timer if running, to prevent multiple thread.
	d->timer.stop();

	//////////////////////////////////////////////////////////////

	//  create a new thread if there is not or isExecution is true or thread is still running or document change
	//if ( ! d->parser || isExecution || d->parser->isRunning() || d->parser->document() != mw->fc->currentDocument() ) {

		// If a previous thread is still running
		// We reconnect it to self delete
		if ( d->parser ) {
			disconnect( d->parser, 0, this, 0 );
			connect( d->parser, SIGNAL(finished()), d->parser, SLOT(deleteLater()) );

			if ( d->parser->isFinished()  )
				d->parser->deleteLater();
		}

		d->parser = 0;

		// Create a new thread
		if ( mw->fc->currentDocument()->lexerEnum() == QsciScintillaExtended::Skoor )
			// Create new document and connect it
			d->parser = new SkoorInterpretation( mw->fc->currentDocument(), this );

		else if ( mw->fc->currentDocument()->lexerEnum() == QsciScintillaExtended::Skool )
			// Create new document and connect it
			d->parser = new SkoolInterpretation( mw->fc->currentDocument(), this );
		else
			return;

		// Set paths
		QString filename = mw->fc->currentDocument()->filename();
		if ( mw->pc->isOpenProject() && mw->pc->currentProject()->isInside(filename) )
			d->parser->addClassPaths( mw->pc->currentProject()->paths() );
	//}

	//////////////////////////////////////////////////////////////

	disconnect( d->parser, 0, this, 0 );
	if ( isExecution )
		connect( d->parser, SIGNAL(finished()), this, SLOT(onExecutionFinished()) );
	else
		connect( d->parser, SIGNAL(finished()), this, SLOT(onParsingFinished()) );

	// Start it
	d->isAuto = isAuto;
	d->parser->setSyntaxMode( ! isExecution );
	d->parser->parse(QThread::LowPriority);
}


void BuildController::onParsingFinished()
{
	if ( d->parser == 0 || d->parser->document() != mw->fc->currentDocument() )
		return;

	const gum::ErrorsContainer & errors = d->parser->errors();

	if ( errors.count() == 0 ) {
		d->prmModel.clear();
		d->prmModel = d->parser->prm();
	}

	if ( d->isAuto )
		mw->fc->currentDocument()->clearAllSyntaxErrors();

	for ( int i = 0, size = errors.count() ; i < size ; i++ ) {
		const gum::ParseError & err = errors.getError(i);
		const QString & filename = d->parser->document()->filename();
		QString errFilename = QString::fromStdString( err.filename );

		if ( d->isAuto && (errFilename.isEmpty() || errFilename.contains(filename,Qt::CaseSensitive)) )
			mw->fc->currentDocument()->setSyntaxError(err.line - 1);

		else if ( ! d->isAuto ) {

			int line = err.line;
			QString s = QString::fromStdString( err.toString() );

			if ( errFilename.isEmpty() ) {
				errFilename = filename;
				s.prepend( errFilename + ":" );
			} else if ( errFilename == "anonymous buffer" ) {
				errFilename = filename;
				s.replace( "anonymous buffer", filename );
			} else if ( errFilename.endsWith(".bak") ) {
				s.replace( errFilename, filename );
				errFilename = filename;
			}

			QListWidgetItem * item = new QListWidgetItem(s, d->msg) ;
			item->setData( Qt::UserRole, errFilename );
			item->setData( Qt::UserRole + 1, line );
		}

	}

	if ( d->autoSyntaxCheck && ! d->timer.isActive() )
		d->timer.start();

	if ( ! d->isAuto && d->parser->isSyntaxMode() ) {
		if ( errors.count() == 0 ) {
			d->msg->addItem(tr("Syntaxe vérifiée. Il n'y a pas d'erreur."));
			d->msg->item(1)->setTextColor(Qt::blue);

		} else {
			d->msg->addItem(tr("Syntaxe vérifiée. Il y a %1 erreurs.").arg(errors.count()));
			d->msg->item(0)->setTextColor(Qt::red);
			d->msg->item(d->msg->count() - 1)->setTextColor(Qt::red);
		}
	} else if ( ! d->isAuto ) {
		if ( errors.count() == 0 ) {
			d->msg->addItem(tr("Éxécution terminée. Résultats :"));
			d->msg->item(d->msg->count() - 1)->setTextColor(Qt::blue);

		} else {
			d->msg->addItem(tr("Éxécution interrompue. Il y a %1 erreurs.").arg(errors.count()));
			d->msg->item(0)->setTextColor(Qt::red);
			d->msg->item(d->msg->count() - 1)->setTextColor(Qt::red);
		}
	}
}


/**
  */
void BuildController::onExecutionFinished()
{
	if ( mw->fc->currentDocument() != d->parser->document() )
		return;

	// We show errors and warnings.
	onParsingFinished();

	// If there was errors or it's not an execution, return.
	if ( d->parser->errors().error_count != 0 || d->parser->isSyntaxMode() )
		return;

	// Else we show all results
	SkoorInterpretation * skoorParser = qobject_cast<SkoorInterpretation *>( d->parser );
	if ( skoorParser == 0 )
		return;

	const vector< pair<string,QueryResult> > & results = skoorParser->results();
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
	if ( ! index.isValid() || ! index.data( Qt::UserRole ).isValid() )
		return;

	QString filename = index.data( Qt::UserRole ).toString();
	int line = index.data( Qt::UserRole + 1 ).toInt() - 1; // lines start at 0 in scintilla

	// Open the file (if filename is empty take the current document)
	if ( ! mw->fc->openFile(filename) )
		return;

	// Go to the line
	QsciScintillaExtended * sci = mw->fc->currentDocument();
	if ( sci == 0 )
		return;
	sci->setCursorPosition(line, 0);

	sci->setError(line);
	sci->setFocus();
}

void BuildController::onDocumentClosed( const QString & filename )
{
	QFile::remove( filename + ".bak" );
}
