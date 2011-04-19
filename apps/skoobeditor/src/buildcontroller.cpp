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
	QListWidget * buildList;
	QListWidget * execList;
	QLineEdit * commandLine;
	QPushButton * commandButton;

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
	d->buildList = mw->ui->buildDock;
	d->execList = mw->ui->execDock;
	d->commandButton = mw->ui->validCommandButton;
	d->commandLine = mw->ui->commandLineEdit;
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

	connect( d->buildList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onMsgDoubleClick(QModelIndex)) );
	connect( mw->ui->actionDockVisibility, SIGNAL(triggered()), this, SLOT(hideBuildMessage()) );

	connect( mw->ui->actionBuild, SIGNAL(triggered()), this, SLOT(checkSyntax()) );
	connect( mw->ui->actionExecute, SIGNAL(triggered()), this, SLOT(execute()) );
	connect( d->commandButton, SIGNAL(clicked()), this, SLOT(onCommandValided()) );

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

	if ( sci->lexerEnum() == QsciScintillaExtended::Skool && sci->block().first == "class" )
		executeClass(sci);

	else if ( sci->lexerEnum() == QsciScintillaExtended::Skool && sci->block().first == "system" )
		executeSystem(sci);

	else if ( sci->lexerEnum() == QsciScintillaExtended::Skoor ) {
		// Clear exec dock and inform start of process
		d->execList->addItem("");
		d->execList->addItem( tr("Éxécution de '%1' ...").arg(sci->title()) );
		int i = d->execList->count() - 1;
		d->execList->item(i)->setTextColor(Qt::blue);
		i--;
		while ( i >= 0 && d->execList->item(i)->textColor() != Qt::gray )
			d->execList->item(i--)->setTextColor(Qt::gray);

		// Erase all error marks in editors
		foreach( QsciScintillaExtended * sci, mw->fc->openDocuments() )
			sci->clearAllErrors();

		startParsing(false,true);

		// Show exec dock
		d->execList->scrollToBottom();
		mw->vc->setExecuteDockVisibility(true);

	} else
		return;

}

void BuildController::createNewParser()
{
	QsciScintillaExtended * sci = mw->fc->currentDocument();
	if ( sci == 0 )
		throw "Can not create new parser, no document opened !";

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
	if ( sci->lexerEnum() == QsciScintillaExtended::Skoor )
		// Create new document and connect it
		d->parser = new SkoorInterpretation( sci, this );

	else if ( sci->lexerEnum() == QsciScintillaExtended::Skool )
		// Create new document and connect it
		d->parser = new SkoolInterpretation( sci, this );
	else
		throw "Invalid lexerEnum ! " + QString::number(sci->lexerEnum());

	// Set paths
	QString filename = sci->filename();
	if ( mw->pc->isOpenProject() && mw->pc->currentProject()->isInside(filename) )
		d->parser->addClassPaths( mw->pc->currentProject()->paths() );
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

	QsciScintillaExtended * sci = mw->fc->currentDocument();

	// Stop timer if running, to prevent multiple thread.
	d->timer.stop();

	// If a previous thread is still running, wait again
	if ( isAuto && d->parser && d->parser->isRunning() ) {
		d->timer.start();
		return;
	}


	//////////////////////////////////////////////////////////////

	//  create a new thread if there is not or isExecution is true or thread is still running or document change
	//if ( ! d->parser || isExecution || d->parser->isRunning() || d->parser->document() != sci )
	try {
		createNewParser();
	} catch (const QString & err ) {
		qWarning() << err;
		return;
	}

	//////////////////////////////////////////////////////////////

	if ( isExecution )
		connect( d->parser, SIGNAL(finished()), this, SLOT(onExecutionFinished()) );
	else
		connect( d->parser, SIGNAL(finished()), this, SLOT(onParsingFinished()) );

	// Start it
	d->isAuto = isAuto;
	d->parser->setSyntaxMode( ! isExecution );
	d->parser->parse(QThread::LowPriority);

	if ( ! isAuto ) {
		d->buildList->addItem("");
		d->buildList->addItem( tr("Vérifie la syntaxe de '%1' ...").arg( sci->title() ) );

		int i = d->buildList->count() - 1;
		d->buildList->item(i)->setTextColor(Qt::blue);
		i--;
		while ( i >= 0 && d->buildList->item(i)->textColor() != Qt::gray )
			d->buildList->item(i--)->setTextColor(Qt::gray);

		// Erase all error marks in editors
		foreach( QsciScintillaExtended * doc, mw->fc->openDocuments() )
			doc->clearAllErrors();

		d->buildList->scrollToBottom();
	}
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

			QListWidgetItem * item = new QListWidgetItem(s, d->buildList) ;
			item->setData( Qt::UserRole, errFilename );
			item->setData( Qt::UserRole + 1, line );
		}
	}

	if ( d->autoSyntaxCheck && ! d->timer.isActive() )
		d->timer.start();

	if ( ! d->isAuto ) {
		if ( errors.count() == 0 ) {
			d->buildList->addItem(tr("Syntaxe vérifiée. Il n'y a pas d'erreur."));
			d->buildList->item(d->buildList->count() - 1)->setTextColor(Qt::blue);

		} else {
			d->buildList->addItem(tr("Syntaxe vérifiée. Il y a %1 erreurs.").arg(errors.count()));
			int i = d->buildList->count() - 1;
			d->buildList->item(i)->setTextColor(Qt::red);
			i--;
			while ( i >= 0 && d->buildList->item(i)->textColor() != Qt::gray )
				i--;
			d->buildList->item(i+1)->setTextColor(Qt::red);
		}
		d->buildList->scrollToBottom();
	}
}


/**
  */
void BuildController::onExecutionFinished()
{
	if ( mw->fc->currentDocument() != d->parser->document() )
		return;

	// Else we show all results
	SkoorInterpretation * skoorParser = qobject_cast<SkoorInterpretation *>( d->parser );
	if ( skoorParser == 0 )
		return;

	// We show errors and warnings.
	onParsingFinished();

	// If there was errors or it's not an execution, return.
	if ( skoorParser->errors().count() != 0 ) {
		d->execList->addItem(tr("Éxécution interrompue. Il y a des erreurs."));
		int i = d->execList->count() - 1;
		d->execList->item(i)->setTextColor(Qt::red);
		i--;
		while ( i >= 0 && d->execList->item(i)->textColor() != Qt::gray )
			i--;
		d->execList->item(i+1)->setTextColor(Qt::red);
		d->execList->scrollToBottom();

		mw->vc->setBuildDockVisibility(true);
		return;
	}

	const vector< pair<string,QueryResult> > & results = skoorParser->results();
	for ( size_t i = 0 ; i < results.size() ; i++ ) {
		const QString & query = QString::fromStdString( results[i].first );
		d->execList->addItem( tr("%1").arg(query) );
		d->execList->item(d->execList->count() - 1)->setTextColor(Qt::darkYellow);
		const QueryResult & result = results[i].second;
		for ( size_t j = 0 ; j < result.size() ; j++ ) {
			float val = result[j].second;
			d->execList->addItem( tr("%1 : %2").arg( QString::fromStdString(result[j].first) ).arg(val) );
		}
	}

	d->execList->addItem(tr("Éxécution terminée."));
	d->execList->item(d->execList->count() - 1)->setTextColor(Qt::blue);
	d->execList->scrollToBottom();
}


/**
  */
void BuildController::hideBuildMessage()
{
	// Erase all error marks in editors
	foreach( QsciScintillaExtended * sci, mw->fc->openDocuments() )
		sci->clearAllErrors();

	mw->vc->setDockVisibility(false);
}


/**
 */
void BuildController::onMsgDoubleClick(QModelIndex index)
{
	// Do anything if it is not an error message.
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

void BuildController::executeClass( QsciScintillaExtended * sci )
{
	qDebug() << "execute class" << sci->block().second;
}

void BuildController::executeSystem( QsciScintillaExtended * sci )
{
	qDebug() << "execute system" << sci->block().second;
}

void BuildController::onCommandValided()
{
	// Create a new parser if necessary
	if ( d->parser == 0 || d->parser->document() != mw->fc->currentDocument() )
		createNewParser();

	// If a previous request was send, wait a few second and return if too long
	if ( d->parser != 0 && d->parser->isRunning() )
		if ( ! d->parser->wait(500) )
			return;

	SkoorInterpretation * parser = qobject_cast<SkoorInterpretation *>(d->parser);
	if ( parser == 0 ) {
		qWarning() << "in onCommandValided() : Error : parser == 0.";
		return;
	}

	disconnect( d->parser, 0, this, 0 );
	connect( d->parser, SIGNAL(finished()), this, SLOT(onCommandParsed()) );
	connect( d->parser, SIGNAL(finished()), this, SLOT(onExecutionFinished()) );
	d->parser->setSyntaxMode(false);

	const QString & command = d->commandLine->text().simplified();

	// Clear exec dock and inform start of process
	d->execList->addItem("");
	d->execList->addItem( tr("Éxécution de '%1' ...").arg(command) );
	int i = d->execList->count() - 1;
	d->execList->item(i)->setTextColor(Qt::blue);
	i--;
	while ( i >= 0 && d->execList->item(i)->textColor() != Qt::gray )
		d->execList->item(i--)->setTextColor(Qt::gray);

	// Erase all error marks in editors
	foreach( QsciScintillaExtended * sci, mw->fc->openDocuments() )
		sci->clearAllErrors();

	// Show exec dock
	d->execList->scrollToBottom();
	mw->vc->setExecuteDockVisibility(true);

	parser->parseCommand( command, QThread::NormalPriority );
	d->commandLine->clear();
}

void BuildController::onCommandParsed()
{
	SkoorInterpretation * parser = qobject_cast<SkoorInterpretation *>(d->parser);
	if ( parser == 0 ) {
		qWarning() << "in onCommandParsed() : Error : parser == 0.";
		return;
	}

	if ( parser->errors().count() == 0 ) {
		QsciScintillaExtended * sci = mw->fc->currentDocument();
		if ( sci->findFirst("}",false,false,false,true,false,0,0) ) {
			QString indent(sci->indentation(sci->currentLine()-1),QChar(' '));
			sci->replace( indent + parser->command() + "\n}");
		}
	}
}
