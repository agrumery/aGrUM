#include "viewcontroller.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filecontroller.h"
#include "editcontroller.h"
#include "searchcontroller.h"
#include "projectcontroller.h"
#include "buildcontroller.h"
#include "qsciscintillaextended.h"

#include <QtGlobal>
#include <QDebug>

//
struct ViewController::PrivateData {
	Qt::WindowStates oldWindowStates;
	QAction * fullScreenAction;
	QAction * docksVisibilityAction;

	QAction * statusBarVisibility;
	QAction * toolBarVisibility;
	QAction * projectExploratorVisibility;
	QAction * searchDockVisibility;
	QAction * buildDockVisibility;

	QAction * lineNumbersVisibility;
	QAction * bookmarksVisibility;
	QAction * foldMargingVisibility;
	QAction * indentationGuidesVisibility;
	QAction * unprintableCharactersVisibility;
};


ViewController::ViewController(MainWindow * mw, QObject *parent) :
	QObject(parent),
	d(new PrivateData),
	mw(mw)
{
	d->fullScreenAction = mw->ui->actionFullScreen;
	d->docksVisibilityAction = mw->ui->actionDocksVisibility;

	d->statusBarVisibility = mw->ui->actionStatusBarVisibility;
	d->toolBarVisibility = mw->ui->actionToolBarVisibility;
	d->projectExploratorVisibility = mw->ui->actionProjectExploratorVisibility;
	d->searchDockVisibility = mw->ui->actionSearchDockVisibility;
	d->buildDockVisibility = mw->ui->actionBuildDockVisibility;

	d->lineNumbersVisibility = mw->ui->actionLineNumbersVisibility;
	d->bookmarksVisibility = mw->ui->actionBookmarksVisibility;
	d->foldMargingVisibility = mw->ui->actionFoldMargingVisibility;
	d->indentationGuidesVisibility = mw->ui->actionIndentationGuidesVisibility;
	d->unprintableCharactersVisibility = mw->ui->actionUnprintableCharactersVisibility;

	// Warning !!! Some connections are done in the .ui file !!!

	connect( d->fullScreenAction, SIGNAL(toggled(bool)), this, SLOT(setFullScreen(bool)) );
	connect( d->docksVisibilityAction, SIGNAL(toggled(bool)), this, SLOT(setDocksVisibility(bool)) );

	connect( d->projectExploratorVisibility, SIGNAL(toggled(bool)), mw->ui->projectExplorator, SLOT(setVisible(bool)) );
	connect( d->searchDockVisibility, SIGNAL(toggled(bool)), mw->ui->advanceSearchResults, SLOT(setVisible(bool)) );
	connect( d->buildDockVisibility, SIGNAL(toggled(bool)), mw->ui->buildDock, SLOT(setVisible(bool)) );

	connect( d->lineNumbersVisibility, SIGNAL(toggled(bool)), this, SLOT(setLineNumbersVisibility(bool)) );
	connect( d->bookmarksVisibility, SIGNAL(toggled(bool)), this, SLOT(setBookmarksVisibility(bool)) );
	connect( d->foldMargingVisibility, SIGNAL(toggled(bool)), this, SLOT(setFoldMargingVisibility(bool)) );
	connect( d->indentationGuidesVisibility, SIGNAL(toggled(bool)), this, SLOT(setIndentationGuidesVisibility(bool)) );
	connect( d->unprintableCharactersVisibility, SIGNAL(toggled(bool)), this, SLOT(setUnprintableCharactersVisibility(bool)) );

	connect( mw->ui->actionIncreaseZoom, SIGNAL(triggered()), this, SLOT(increaseZoom()) );
	connect( mw->ui->actionDecreaseZoom, SIGNAL(triggered()), this, SLOT(decreaseZoom()) );
	connect( mw->ui->actionNormalZoom, SIGNAL(triggered()), this, SLOT(normalZoom()) );

}

/* ************************************************************************* */

bool ViewController::isFullScreen() const
{
	return d->fullScreenAction->isChecked();
}

bool ViewController::isDocksVisible() const
{
	return d->docksVisibilityAction->isChecked();
}

bool ViewController::isStatusBarVisible() const
{
	return d->statusBarVisibility->isChecked();
}

bool ViewController::isToolBarVisible() const
{
	return d->toolBarVisibility->isChecked();
}

bool ViewController::isProjectExploratorVisible() const
{
	return d->projectExploratorVisibility->isChecked();
}

bool ViewController::isSearchDockVisible() const
{
	return d->searchDockVisibility->isChecked();
}

bool ViewController::isBuildDockVisible() const
{
	return d->buildDockVisibility->isChecked();
}

bool ViewController::isLineNumbersVisible() const
{
	return d->lineNumbersVisibility->isChecked();
}

bool ViewController::isBookmarksVisible() const
{
	return d->bookmarksVisibility->isChecked();
}

bool ViewController::isFoldMargingVisible() const
{
	return d->foldMargingVisibility->isChecked();
}

bool ViewController::isIndentationGuidesVisible() const
{
	return d->indentationGuidesVisibility->isChecked();
}

bool ViewController::isUnprintableCharactersVisible() const
{
	return d->unprintableCharactersVisibility->isChecked();
}

/* ************************************************************************* */

void ViewController::setFullScreen(bool checked)
{
	if ( checked != d->fullScreenAction->isChecked() )
		return;

	if (checked) {
		d->oldWindowStates = mw->windowState();
		mw->showFullScreen();
	} else {
		mw->setWindowState(d->oldWindowStates);
	}

	d->fullScreenAction->setChecked(checked);
}

void ViewController::setDocksVisibility(bool visible)
{
	d->docksVisibilityAction->setChecked(visible);
	setStatusBarVisibility(visible);
	setToolBarVisibility(visible);
	setProjectExploratorVisibility(visible);
}

void ViewController::setStatusBarVisibility(bool visible)
{
	d->statusBarVisibility->setChecked(visible);
}

void ViewController::setToolBarVisibility(bool visible)
{
	d->toolBarVisibility->setChecked(visible);
}

void ViewController::setProjectExploratorVisibility(bool visible)
{
	d->projectExploratorVisibility->setChecked(visible && mw->pc->isOpenProject());
}

void ViewController::setSearchDockVisibility(bool visible)
{
	d->searchDockVisibility->setChecked(visible);
}

void ViewController::setBuildDockVisibility(bool visible)
{
	d->buildDockVisibility->setChecked(visible);
}

void ViewController::setLineNumbersVisibility( bool visible )
{
	if ( visible != d->lineNumbersVisibility->isChecked() )
		return;

	if ( visible )
		foreach ( QsciScintillaExtended * sci, mw->fc->openDocuments() )
			sci->setMarginWidth(0,"8888");
	else
		foreach ( QsciScintillaExtended * sci, mw->fc->openDocuments() )
			sci->setMarginWidth(0,0);

	d->lineNumbersVisibility->setChecked(visible);
}

void ViewController::setBookmarksVisibility( bool visible )
{
	if ( visible != d->bookmarksVisibility->isChecked() )
		return;

	if ( visible )
		foreach ( QsciScintillaExtended * sci, mw->fc->openDocuments() )
			sci->setMarginWidth(1,16);
	else
		foreach ( QsciScintillaExtended * sci, mw->fc->openDocuments() )
			sci->setMarginWidth(1,0);

	d->bookmarksVisibility->setChecked(visible);
}

void ViewController::setFoldMargingVisibility( bool visible )
{
	if ( visible != d->foldMargingVisibility->isChecked() )
		return;

	if ( visible )
		foreach ( QsciScintillaExtended * sci, mw->fc->openDocuments() )
			sci->setFolding(QsciScintillaExtended::BoxedTreeFoldStyle);
	else
		foreach ( QsciScintillaExtended * sci, mw->fc->openDocuments() )
			sci->setFolding(QsciScintillaExtended::NoFoldStyle);

	d->foldMargingVisibility->setChecked(visible);
}

void ViewController::setIndentationGuidesVisibility( bool visible )
{
	if ( visible != d->indentationGuidesVisibility->isChecked() )
		return;

	foreach ( QsciScintillaExtended * sci, mw->fc->openDocuments() )
		sci->setIndentationGuides(visible);

	d->indentationGuidesVisibility->setChecked(visible);
}

void ViewController::setUnprintableCharactersVisibility( bool visible )
{
	if ( visible != d->unprintableCharactersVisibility->isChecked() )
		return;

	foreach ( QsciScintillaExtended * sci, mw->fc->openDocuments() ) {
		sci->setWhitespaceVisibility((QsciScintillaExtended::WhitespaceVisibility) visible);
		sci->setEolVisibility(visible);
	}

	d->unprintableCharactersVisibility->setChecked(visible);
}

void ViewController::increaseZoom()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->zoomIn();
}

void ViewController::decreaseZoom()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->zoomOut();
}

void ViewController::normalZoom()
{
	if (mw->fc->hasCurrentDocument())
		mw->fc->currentDocument()->resetZoom();
}
