#include "editcontroller.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filecontroller.h"
#include "searchcontroller.h"
#include "viewcontroller.h"
#include "projectcontroller.h"
#include "buildcontroller.h"
#include "qsciscintillaextended.h"
#include "prmcompleter.h"

#include <Qsci/qscilexer.h>
#include <QDebug>

//
struct EditController::PrivateData {
	PRMCompleter * completer;
	QSharedPointer<PRMTreeModel2> prmModel;
};

// Constructor
EditController::EditController( MainWindow * mw, QObject * parent) :
	QObject(parent),
	mw(mw),
	d(new PrivateData),
	pr(new Properties(mw,mw))
{
	d->completer = new PRMCompleter(this);
	d->completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

	connect( mw->ui->actionUndo, SIGNAL(triggered()), this, SLOT(undo()) );
	connect( mw->ui->actionRedo, SIGNAL(triggered()), this, SLOT(redo()) );
	connect( mw->ui->actionCut, SIGNAL(triggered()), this, SLOT(cut()) );
	connect( mw->ui->actionCopy, SIGNAL(triggered()), this, SLOT(copy()) );
	connect( mw->ui->actionPaste, SIGNAL(triggered()), this, SLOT(paste()) );
	connect( mw->ui->actionRemove, SIGNAL(triggered()), this, SLOT(remove()) );
	connect( mw->ui->actionSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()) );
	connect( mw->ui->actionSwitchComment, SIGNAL(triggered()), this, SLOT(switchComment()) );
	connect( mw->ui->actionIncreaseIndentation, SIGNAL(triggered()), this, SLOT(increaseIndentation()) );
	connect( mw->ui->actionDecreaseIndentation, SIGNAL(triggered()), this, SLOT(decreaseIndentation()) );
	connect( mw->ui->actionEditPreferences, SIGNAL(triggered()), this, SLOT(editPreferences()) );
	connect( mw->ui->actionAutoComplete, SIGNAL(triggered()), this, SLOT(autoComplete()) );

	// Must be start after project triggerInit
	QTimer::singleShot( 500, this, SLOT(triggerInit()) );
}


// Destructor
EditController::~EditController()
{
	delete d;
}

void EditController::triggerInit()
{
	connect( mw->bc, SIGNAL(currentDocumentModelChanged()), this, SLOT(onCurrentDocumentModelChanged()) );
}

QCompleter * EditController::completer() const
{
	return d->completer;
}

void EditController::undo()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->undo();
}

void EditController::redo()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->redo();
}

void EditController::cut()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->cut();
}

void EditController::copy()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->copy();
}

void EditController::paste()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->paste();
}

void EditController::remove()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->removeSelectedText();
}

void EditController::selectAll()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->selectAll();
}

void EditController::switchComment()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->switchComment();
}

void EditController::increaseIndentation()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->indent();
}

void EditController::decreaseIndentation()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->unindent();
}

void EditController::autoComplete()
{
	if ( ! mw->fc->hasCurrentDocument() )
		return;

	if ( ! d->prmModel.isNull() ) {
		mw->fc->currentDocument()->setCompleter(d->completer);
		mw->fc->currentDocument()->autoCompleteFromCompleter();
	} else
		mw->fc->currentDocument()->autoCompleteFromAll();
}

/**
  */
void EditController::editPreferences()
{
	pr->exec();
}

void EditController::onCurrentDocumentModelChanged()
{
	QSharedPointer<PRMTreeModel2> newPRMModel = mw->bc->currentDocumentModel();

	// If doesn't changed, don't changed completer.
	if ( newPRMModel.isNull() || d->prmModel == newPRMModel )
		return;

	QSharedPointer<PRMTreeModel2> oldPRMModel = d->prmModel;
	d->prmModel.clear();
	d->prmModel = newPRMModel;
	//d->prmModel->setCurrentPackage(mw->fc->currentDocument()->package());
	//d->prmModel->setCurrentBlock(mw->fc->currentDocument()->block().second);
	//d->prmModel->addKeywords( QString( mw->fc->currentDocument()->lexer()->keywords(1) ).split(QChar(' ')) );
	d->prmModel->sort(0);
	d->completer->setModel( d->prmModel.data() );

	emit completerChanged();
}
