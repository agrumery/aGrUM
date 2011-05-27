#include "searchcontroller.h"

#include "uis/mainwindow.h"
#include "ui_mainwindow.h"
#include "filecontroller.h"
#include "editcontroller.h"
#include "viewcontroller.h"
#include "projectcontroller.h"
#include "buildcontroller.h"
#include "qsciscintillaextended.h"

#include "uis/advancedsearch.h"

#include <QMessageBox>
#include <QDebug>

struct Search {
	QString search;
	QString replace;
	bool rplc;
	bool regx;
	bool cs;
	bool wo;
	bool wrap;
	bool isQuickSearch;
};

//
struct SearchController::PrivateData {
	QTabWidget * tab; // Tab Widget
	QLineEdit * qse; // Quick Search Edit
	QLineEdit * qre; // Quick Replace Edit
	QWidget * qsw; // Quick Search Edit
	QWidget * qrw; // Quick Replace Edit
	AdvancedSearch * as; // Advance Search
	QTreeWidget * sr;
	Search cs; // Current Search

	QMenu * searchOptionMenu;
	QAction * caseSensitiveOptionAction;
	QAction * wholeWordOptionAction;
	QAction * useRegexOptionAction;
};


/* ****************************************************************************** */

/**
  */
SearchController::SearchController(MainWindow * mw, QObject *parent) :
		QObject(parent),
		mw(mw),
		d(new PrivateData)
{
	d->tab = mw->ui->tabWidget;
	d->sr = mw->ui->searchDock;
	d->qse = mw->ui->quickSearchEdit;
	d->qre = mw->ui->quickReplaceEdit;
	d->qsw = mw->ui->quickSearchWidget;
	d->qrw = mw->ui->quickReplaceWidget;
	d->as = new AdvancedSearch(mw);

	d->searchOptionMenu = new QMenu(d->qsw);
	d->caseSensitiveOptionAction = new QAction(tr("Sensible à la casse"), d->searchOptionMenu);
	d->caseSensitiveOptionAction->setCheckable(true);
	d->wholeWordOptionAction = new QAction(tr("Mots entiers seulement"), d->searchOptionMenu);
	d->wholeWordOptionAction->setCheckable(true);
	d->useRegexOptionAction = new QAction(tr("Utilisation des expressions régulières"), d->searchOptionMenu);
	d->useRegexOptionAction->setCheckable(true);
	d->searchOptionMenu->addAction( d->caseSensitiveOptionAction );
	d->searchOptionMenu->addAction( d->wholeWordOptionAction );
	d->searchOptionMenu->addAction( d->useRegexOptionAction );

	// Hide quickSearch and quickReplace widgets.
	mw->ui->quickSearchWidget->hide();
	mw->ui->quickReplaceWidget->hide();

	//
	d->sr->setColumnCount(2);
	d->sr->setColumnWidth(0,80);

	//
	connect( d->qse, SIGNAL(textChanged(QString)), this, SLOT(onQuickSearchEditTextChanged(QString)) );
	connect( d->qre, SIGNAL(textChanged(QString)), this, SLOT(onQuickReplaceEditTextChanged(QString)) );
	connect( d->sr, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onResultSearchDoubleClick(QTreeWidgetItem*,int)) );

	//
	connect( mw->ui->actionQuickSearch, SIGNAL(triggered()), this, SLOT(quickSearch()) );
	connect( mw->ui->actionAdvanceSearch, SIGNAL(triggered()), this, SLOT(advanceSearch()) );
	connect( mw->ui->actionReplace, SIGNAL(triggered()), this, SLOT(replace()) );
	connect( mw->ui->actionNext, SIGNAL(triggered()), this, SLOT(next()) );
	connect( mw->ui->actionPrevious, SIGNAL(triggered()), this, SLOT(previous()) );
	connect( mw->ui->actionReplaceAndNext, SIGNAL(triggered()), this, SLOT(replaceAndNext()) );
	connect( mw->ui->actionReplaceAndPrevious, SIGNAL(triggered()), this, SLOT(replaceAndPrevious()) );
	connect( mw->ui->actionReplaceAll, SIGNAL(triggered()), this, SLOT(replaceAll()) );
	connect( mw->ui->actionSwitchMarker, SIGNAL(triggered()), this, SLOT(switchMarker()) );
	connect( mw->ui->actionFindNextMarker, SIGNAL(triggered()), this, SLOT(findNextMarker()) );
	connect( mw->ui->actionFindPreviousMarker, SIGNAL(triggered()), this, SLOT(findPreviousMarker()) );
	connect( mw->ui->actionDockVisibility, SIGNAL(triggered()), this, SLOT(hideQuickWidgets()) );

	connect( mw->ui->searchOptionsButton, SIGNAL(clicked()), this, SLOT(onSearchOptionButtonClicked()) );
	connect( d->caseSensitiveOptionAction, SIGNAL(toggled(bool)), this, SLOT(onCaseSensitiveOptionChecked(bool)) );
	connect( d->wholeWordOptionAction, SIGNAL(toggled(bool)), this, SLOT(onWholeWordOptionChecked(bool)) );
	connect( d->useRegexOptionAction, SIGNAL(toggled(bool)), this, SLOT(onUseRegexOptionChecked(bool)) );
}

/* ****************************************************************************** */

/**
  If text is selected,
  set this text in the search field.
  */
void SearchController::quickSearch()
{
	QsciScintillaExtended * sci = mw->fc->currentDocument();
	if (sci == 0)
		return;

	d->cs.rplc = false;
	d->cs.cs = d->caseSensitiveOptionAction->isChecked();
	d->cs.wo = d->wholeWordOptionAction->isChecked();
	d->cs.regx = d->useRegexOptionAction->isChecked();
	d->cs.wrap = true;
	d->cs.isQuickSearch = true;

	d->qsw->show();

	if (sci->hasSelectedText()) {
		d->qse->setText(sci->selectedText());
		d->cs.search = sci->selectedText();
	}

	d->caseSensitiveOptionAction->setDisabled(false);
	d->wholeWordOptionAction->setDisabled(false);
	d->useRegexOptionAction->setDisabled(false);
	d->qse->setReadOnly(false);
	d->qse->setFocus();
	d->qse->selectAll();
}

/**
  */
void SearchController::advanceSearch()
{
	QsciScintillaExtended * sci = mw->fc->currentDocument();
	if (sci != 0 && sci->hasSelectedText())
		d->as->setSearchText(sci->selectedText());

	// Enable / Disable project radio
	d->as->setProjectSearchEnabled( mw->pc->isOpenProject() );

	// Show advance search dialog
	int resp = d->as->exec();

	// If cancel or close, return
	if ( resp == 0 || d->as->searchText().isEmpty() )
		return;

	// Complete search and replace field
	d->qse->setText( d->as->searchText() );
	d->qre->setText( d->as->replaceText() );

	//// Process search
	QList<QString> filesToSearchIn;
	QList<QsciScintillaExtended *> documentsToSearchIn;

	// DOCUMENT SEARCH
	if ( d->as->isDocumentSearch() ) {
		if ( sci == 0 )
			return;
		else
			documentsToSearchIn << sci;

	// PROJECT SEARCH
	} else if ( d->as->isProjectSearch() ) {

		Project * proj = mw->pc->currentProject();
		if (proj == 0)
			return;

		QList<QString> parents;
		parents << proj->dir();

		while ( ! parents.empty() ) {
			QString current = parents.takeFirst();

			// For each child
			foreach ( QFileInfo fileinfo, QDir(current).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot) ) {
				if ( fileinfo.isDir() )  {
					parents << fileinfo.filePath();
				} else {
					// If pass filter
					if ( ( (fileinfo.fileName().endsWith(".skool") || fileinfo.fileName().endsWith(".skl")) &&
						   (d->as->filter() == AdvancedSearch::All || d->as->filter() == AdvancedSearch::Skool) )
						||
								( (fileinfo.fileName().endsWith(".skoor") || fileinfo.fileName().endsWith(".skr")) &&
								  (d->as->filter() == AdvancedSearch::All || d->as->filter() == AdvancedSearch::Skoor) ) ) {
						sci = mw->fc->fileToDocument(fileinfo.filePath());
						if ( sci != 0 )
							documentsToSearchIn << sci;
						else
							filesToSearchIn << fileinfo.filePath();
					}
				}
			}
		}

	// OPEN DUCUMENTS SEARCH
	} else if ( d->as->isOpenFilesSearch() ) {
		int currentIndex = d->tab->currentIndex();
		int size = d->tab->count();

		// Foreach open tab
		for ( int i = 0 ; i < size ; i++ ) {
			int index = (currentIndex + i) % size; // Start with the current document, and go to right
			sci = qobject_cast<QsciScintillaExtended*>(d->tab->widget(index));

			// If pass filter
			if ( d->as->filter() == AdvancedSearch::All ||
				 (sci->lexerEnum() == QsciScintillaExtended::Skool && d->as->filter() == AdvancedSearch::Skool) ||
				 (sci->lexerEnum() == QsciScintillaExtended::Skoor && d->as->filter() == AdvancedSearch::Skoor))
				documentsToSearchIn << sci;
		}
	}

	// Init Search struct
	d->cs.rplc = false;
	d->cs.regx = d->as->isRegex();
	d->cs.cs = d->as->isCaseSensitive();
	d->cs.wo = d->as->isWholeWord();
	d->cs.wrap = false;
	d->cs.isQuickSearch = false;
	d->cs.search = d->as->searchText();
	d->cs.replace = d->as->replaceText();

	d->sr->clear();

	// For each file, search the string
	// and show result in the search result tree view.
	parseFiles( filesToSearchIn, d->as->isReplaceAll() );
	parseDocuments( documentsToSearchIn, d->as->isReplaceAll() );

	// If not match
	if ( d->sr->topLevelItemCount() == 0 ) {
		QMessageBox::information(mw, tr("Résultat de la recherche"), tr("Aucun résultat trouvé."));
		return;
	}

	// Show result
	if ( ! d->as->isReplaceAll() ) {
		d->caseSensitiveOptionAction->setChecked( d->cs.cs );
		d->wholeWordOptionAction->setChecked( d->cs.wo );
		d->useRegexOptionAction->setChecked( d->cs.regx );
		d->caseSensitiveOptionAction->setDisabled(true);
		d->wholeWordOptionAction->setDisabled(true);
		d->useRegexOptionAction->setDisabled(true);
		d->qse->setReadOnly(true);
		d->qre->setReadOnly(true);
		d->qsw->show();
		d->qrw->show();
		mw->vc->setSearchDockVisibility(true);
		if ( d->sr->model()->rowCount() == 1 )
			d->sr->expandAll();
	}
}

/**
  */
bool SearchController::next(QsciScintillaExtended * sci)
{
	bool result = false;

	if ( sci == 0 )
		sci = mw->fc->currentDocument();

	if ( sci != 0 && ! d->cs.search.isEmpty() && (! d->cs.isQuickSearch || ! d->qse->text().isEmpty()) ) {
		result = sci->findFirst( d->cs.search, d->cs.regx, d->cs.cs, d->cs.wo, d->cs.wrap, true);

		if ( d->cs.isQuickSearch && ! result ) {
			//QMessageBox::information(mw,tr("SkoolEditor - Recherche"),tr("Impossible de trouver le texte recherché."));
			d->qse->setStyleSheet("background-color: red");
			sci->setSelection(0,0,0,0);
			d->qse->setFocus();
		} else
			d->qse->setStyleSheet("");
	} else if (d->cs.isQuickSearch)
		result =  false;

	return result;
}

/**
  */
bool SearchController::previous(QsciScintillaExtended * sci)
{
	bool result;

	if ( sci == 0 )
		sci = mw->fc->currentDocument();

	if ( sci != 0 &&  ! d->cs.search.isEmpty() && (! d->cs.isQuickSearch || ! d->qse->text().isEmpty()) ) {
		// Search just before the previous result.
		result = sci->findFirst( d->cs.search, d->cs.regx, d->cs.cs, d->cs.wo, d->cs.wrap, false, sci->currentLine(), sci->currentIndex() - 1);

		if ( d->cs.isQuickSearch && ! result ) {
			d->qse->setStyleSheet("background-color: red");
			sci->setSelection(0,0,0,0);
			d->qse->setFocus();
		} else if (d->cs.isQuickSearch)
			d->qse->setStyleSheet("");
	} else
		result = false;

	return result;
}

/* ****************************************************************************** */

/**
  */
void SearchController::replace()
{
	QsciScintillaExtended * sci = mw->fc->currentDocument();
	if (sci == 0)
		return;

	quickSearch();

	d->cs.rplc = true;
	d->qrw->show();
	d->qre->setReadOnly(false);
	d->qre->setFocus();
}

/**
  */
bool SearchController::replaceAndNext(QsciScintillaExtended * sci)
{
	bool result = false;

	if ( sci == 0 )
		sci = mw->fc->currentDocument();

	if ( sci != 0 && ! d->cs.search.isEmpty() && (! d->cs.isQuickSearch || ! d->cs.search.isEmpty()) ) {
		if (sci->hasSelectedText())
			sci->replace(d->cs.replace);
		result = next();
	}

	return result;
}

/**
  */
bool SearchController::replaceAndPrevious(QsciScintillaExtended * sci)
{
	bool result = false;

	if ( sci == 0 )
		sci = mw->fc->currentDocument();

	if ( sci != 0 && ! d->cs.search.isEmpty() && (! d->cs.isQuickSearch || ! d->cs.search.isEmpty()) ) {
		if (sci->hasSelectedText())
			sci->replace(d->cs.replace);
		result = previous();
	}

	return result;
}

/**
  */
void SearchController::replaceAll(QsciScintillaExtended * sci)
{
	if ( sci == 0 )
		sci = mw->fc->currentDocument();
	if ( sci != 0 && (! d->cs.isQuickSearch || !d->cs.search.isEmpty()) )
		sci->replaceAll( d->cs.search, d->cs.replace );
}

/* ****************************************************************************** */

/**
  */
void SearchController::switchMarker()
{
	QsciScintillaExtended * sci = mw->fc->currentDocument();
	if ( sci != 0 ) {
		sci->switchMarker();
	}
}

/**
  */
void SearchController::findNextMarker()
{
	QsciScintillaExtended * sci = mw->fc->currentDocument();
	if ( sci != 0 ) {
		sci->goToNextMarker();
	}
}

/**
  */
void SearchController::findPreviousMarker()
{
	QsciScintillaExtended * sci = mw->fc->currentDocument();
	if ( sci != 0 ) {
		sci->goToPreviousMarker();
	}
}

/* ****************************************************************************** */

/**
  */
void SearchController::hideQuickWidgets()
{
	d->qsw->hide();
	d->qrw->hide();
	mw->vc->setSearchDockVisibility(false);
	d->qre->clear();
	d->qse->clear();
	d->caseSensitiveOptionAction->setDisabled(false);
	d->wholeWordOptionAction->setDisabled(false);
	d->useRegexOptionAction->setDisabled(false);
	d->caseSensitiveOptionAction->setChecked(false);
	d->wholeWordOptionAction->setChecked(false);
	d->useRegexOptionAction->setChecked(false);
}

/* ****************************************************************************** */

void SearchController::onQuickSearchEditTextChanged(QString)
{
	d->cs.search = d->qse->text();
	// Move the cursor index to the beggining of the previous result to rematch it if possible
	// even if we delete a caracter
	mw->fc->currentDocument()->setCurrentIndex( mw->fc->currentDocument()->currentIndex() - d->cs.search.length() - 1);
	next();
}

void SearchController::onQuickReplaceEditTextChanged(QString)
{
	d->cs.replace = d->qre->text();
}

void SearchController::onResultSearchDoubleClick(QTreeWidgetItem * item, int)
{
	QString filename;
	if ( item->text(0).isEmpty() ) // => top-level item
		filename = item->text(1);
	else
		filename = item->parent()->text(1);

	QsciScintillaExtended * sci = mw->fc->fileToDocument(filename);
	// If file is open
	if ( sci == 0 ) {
		if ( ! mw->fc->openFile( filename ) )
			return;
		else
			sci = mw->fc->currentDocument();
	} else
		d->tab->setCurrentWidget(sci);


	if ( ! item->text(0).isEmpty() ) {
		int line = item->text(0).toInt();
		sci->setCursorPosition(line,0);
	}

}

/**
  */
void SearchController::parseFiles( const QList<QString> & filePaths, bool isReplaceAll )
{
	QList<QsciScintillaExtended*> list;

	// open files in QsciScintillaExtended
	// and had them to the list
	foreach ( QString fpath, filePaths ) {

		// We open the file
		QFile file(fpath);

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			continue;

		// We charge the file,
		QsciScintillaExtended * sci = new QsciScintillaExtended(fpath);
		QString text(file.readAll());
		file.close();
		sci->setText(text);
		sci->setModified(false);

		list << sci;
	}

	parseDocuments(list,isReplaceAll);

	// close files
	foreach ( QsciScintillaExtended * sci, list ) {

		// if replaceAll is true
		// modification may be done
		// so save them
		if ( isReplaceAll ) {
			mw->fc->saveFile(sci);
		}

		sci->deleteLater();
	}
}

/**
 */
void SearchController::parseDocuments( const QList<QsciScintillaExtended *> & documents, bool isReplaceAll )
{
	foreach(QsciScintillaExtended * sci, documents ) {
		int colorCpt = 0;
		int searchMatch = 0;

		if (isReplaceAll) {
			replaceAll(sci);
			continue;
		}

		// Start from the beginning
		sci->setCursorPosition(0,0);

		// Init
		bool firstFind = true;
		QTreeWidgetItem * parent = 0, * child = 0;
		int lineFrom, indexFrom, lineTo, indexTo;

		// while there is a next
		while ( next(sci) ) {
			searchMatch++;

			//
			if ( firstFind ) {
				firstFind = false;
				// new row for this document
				parent = new QTreeWidgetItem();
				parent->setBackgroundColor(0,QColor(255,200,0));
				parent->setBackgroundColor(1,QColor(255,200,0));
				d->sr->addTopLevelItem(parent);
			}

			// Add the next() result, number line and text line
			child = new QTreeWidgetItem(parent);
			sci->getSelection(&lineFrom, &indexFrom, & lineTo, &indexTo);
			child->setText(0, QString::number(lineFrom));
			child->setText(1, sci->text(lineFrom).simplified() );
			if (colorCpt++ % 2 != 0) {
				child->setBackgroundColor(0, QColor(230,230,230));
				child->setBackgroundColor(1, QColor(230,230,230));
			}

		}

		if (parent)
			parent->setText(1, tr("%1 (%2)").arg(sci->title()).arg(searchMatch) );
	}
}


void SearchController::onSearchOptionButtonClicked()
{
	d->searchOptionMenu->popup( d->qsw->mapToGlobal( mw->ui->searchOptionsButton->pos() ) );
}

void SearchController::onCaseSensitiveOptionChecked(bool checked)
{
	d->cs.cs = checked;
	mw->fc->currentDocument()->setCurrentIndex( mw->fc->currentDocument()->currentIndex() - d->cs.search.length() - 1);
	next();
}

void SearchController::onWholeWordOptionChecked(bool checked)
{
	d->cs.wo = checked;
	mw->fc->currentDocument()->setCurrentIndex( mw->fc->currentDocument()->currentIndex() - d->cs.search.length() - 1);
	next();
}

void SearchController::onUseRegexOptionChecked(bool checked)
{
	d->cs.regx = checked;
	mw->fc->currentDocument()->setCurrentIndex( mw->fc->currentDocument()->currentIndex() - d->cs.search.length() - 1);
	next();
}
