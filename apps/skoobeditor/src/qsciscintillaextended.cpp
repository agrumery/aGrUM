#include "qsciscintillaextended.h"

#include "qscilexerskool.h"
#include "qscilexerskoor.h"
#include "qscilexerskool2.h"

#include <Qsci/qsciapis.h>
#include <QFileInfo>
#include <QTabWidget>
#include <QTabBar>
#include <QAction>
#include <QDebug>

//
struct QsciScintillaExtended::PrivateData {
	QTabWidget * tabWidget;
	QString title;
	QString filename;
	QList<int> syntaxErrorOnLine;
	QList<QString> classesImport;
};


void QsciScintillaExtended::initParameters(QWidget * parent)
{
	d = new PrivateData;
	d->tabWidget = qobject_cast<QTabWidget *>(parent); // return 0 if parent is not a QTabWidget
	if (d->tabWidget != 0)
		connect(this, SIGNAL(modificationChanged(bool)), this, SLOT(onModificationChanged(bool)));

	setTitle(tr("Sans titre")); // Default file path show in the window title bar.

	setMarginWidth(0,"8888");

	setMarginSensitivity(1,true);
	setMarginMarkerMask(1, 1 << Bookmark);

	setFolding(BoxedTreeFoldStyle);

	QFont font("monospace");
	font.setPointSize(10);
	font.setStyleHint(QFont::Monospace);
	setFont(font);

	setUtf8(true);
	setEdgeMode(QsciScintilla::EdgeLine);
	setEdgeColumn(80);
	setAutoIndent(true);
	setIndentationGuides(true);
	setTabWidth(4);

	// Auto auto-complete start from 4 letters and search in document and api.
	//setAutoCompletionSource(AcsAll);
	//setAutoCompletionThreshold(4);
	setAutoCompletionShowSingle(true);

	// Init markers
	markerDefine(RightArrow,Bookmark);
	setMarkerBackgroundColor(QColor(0,196,0), Bookmark);
	markerDefine(Background,Error);
	setMarkerBackgroundColor(QColor(255,100,100), Error);;

	// TODO : A modifier avec les nouvelles fonctions de QScintilla 2.4.6
	SendScintilla( (unsigned int) SCI_INDICSETSTYLE,
						(unsigned int) Error, (unsigned int) INDIC_SQUIGGLE);
	SendScintilla( (unsigned int) SCI_INDICSETFORE,
						(unsigned int) Error, 0x0000FF);

	// Auto-completion
	QAction * action = new QAction(this);
	action->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Enter));
	connect( action, SIGNAL(triggered()), this, SLOT(switchComment()) );
	addAction(action);

	//
	connect( this, SIGNAL(marginClicked(int,int,Qt::KeyboardModifiers)), this, SLOT(onMarginClicked(int,int,Qt::KeyboardModifiers)) );

	// Disable several default key bindings that Scintilla provides.
	// They are handle by QAction signals/slots.
	SendScintilla(SCI_CLEARCMDKEY, 'Z' + (SCMOD_CTRL << 16));
	SendScintilla(SCI_CLEARCMDKEY, 'Y' + (SCMOD_CTRL << 16));
	SendScintilla(SCI_CLEARCMDKEY, 'A' + (SCMOD_CTRL << 16));
	SendScintilla(SCI_CLEARCMDKEY, 'X' + (SCMOD_CTRL << 16));
	SendScintilla(SCI_CLEARCMDKEY, 'C' + (SCMOD_CTRL << 16));
	SendScintilla(SCI_CLEARCMDKEY, 'V' + (SCMOD_CTRL << 16));
}

/**
  */
QsciScintillaExtended::QsciScintillaExtended(QWidget *parent) :
		QsciScintilla(parent)
{
	initParameters(parent);
}


/**
  */
QsciScintillaExtended::QsciScintillaExtended(Lexer lex, QWidget *parent) :
		QsciScintilla(parent)
{
	initParameters(parent);

	setLexer(lex);
}


/**
  */
QsciScintillaExtended::QsciScintillaExtended(const QString & filename, QWidget *parent) :
		QsciScintilla(parent)
{
	initParameters(parent);

	setFilename(filename);
}


/**
  */
QsciScintillaExtended::~QsciScintillaExtended()
{
	delete d;
}


/**
  */
QString QsciScintillaExtended::title() const
{
	return d->title;
}


/**
  */
void QsciScintillaExtended::setTitle(const QString & title)
{
	d->title = title;
	setWindowFilePath(d->title);
}



/**
  */
QString QsciScintillaExtended::filename() const
{
	return d->filename;
}


/**
  */
void QsciScintillaExtended::setFilename(const QString & filename)
{
	if ( filename == d->filename )
		return;

	QFileInfo info(filename);
	QString old = d->filename;
	d->filename = filename;

	if (info.suffix() == "skool" || info.suffix() == "skl")
		setLexer(Skool);
	else if (info.suffix() == "skoor" || info.suffix() == "skr")
		setLexer(Skoor);

	setTitle(d->filename);

	emit filenameChanged(old, filename);
}


QsciScintillaExtended::Lexer QsciScintillaExtended::lexerEnum() const
{
	if ( lexer() == 0 )
		return None;
	else if ( QString("Skool") == lexer()->language() )
		return Skool;
	else if ( lexer()->language() == QString("Skoor") )
		return Skoor;
	else
		return None;
}


void QsciScintillaExtended::setLexer(QsciScintillaExtended::Lexer lex)
{
	QsciScintilla::setLexer(0);

	if (lex == Skool)
		QsciScintilla::setLexer( new QsciLexerSkool2(this) );
	else if (lex == Skoor)
		QsciScintilla::setLexer( new QsciLexerSkoor(this) );
}


/**
  */
int QsciScintillaExtended::currentLine() const
{
	int line, index;
	getCursorPosition(&line,&index);
	return line;
}


/**
  */
void QsciScintillaExtended::setCurrentLine( int line )
{
	int l, index;
	getCursorPosition(&l,&index);
	setCursorPosition(line,index);
}


/**
  */
int QsciScintillaExtended::currentIndex() const
{
	int line, index;
	getCursorPosition(&line,&index);
	return index;
}

/**
  */
void QsciScintillaExtended::setCurrentIndex( int index )
{
	int line, i;
	getCursorPosition(&line,&i);
	setCursorPosition(line,index);
}

/**
  Switch comments on the selected block.
  Add and remove comments at the start of each selected lines,
  or, if any selection, the current line.
  */
void QsciScintillaExtended::switchComment()
{
	int cursorLine, cursorIndex;
	getCursorPosition(&cursorLine, &cursorIndex);

	// If there is no selection
	if ( ! hasSelectedText() ) {

		// if the line is commented, we decommented it.
		QRegExp reg("^(\\s*)//");
		if ( text(cursorLine).indexOf(reg) != -1 ) {

			setSelection( cursorLine, reg.cap(1).size(), cursorLine, reg.cap(0).size() );
			removeSelectedText();

			// update cursor index due to shift
			cursorIndex -= 2;

		// else we comment it.
		} else {

			insertAt("//",cursorLine,0);
			// update cursor index due to shift
			cursorIndex += 2;
		}

		// Update cursor position
		setCursorPosition(cursorLine, cursorIndex);

	// else if this is a selection
	} else {

		int lineFrom, lineTo, indexFrom, indexTo;

		getSelection(&lineFrom,&indexFrom,&lineTo,&indexTo);

		beginUndoAction();
		// For each line of the selection
		for ( int line = lineFrom ; line <= lineTo ; line++ ) {
			QRegExp reg("^(\\s*)//");
			// if the line is commented, we decommented it.
			if ( text(line).indexOf(reg) != -1 ) {
				// if the line is commented, we decommented it.
				setSelection( line, reg.cap(1).size(), line, reg.cap(0).size() );
				removeSelectedText();

				// update selection and cursor due to the shift
				if ( line == lineFrom )
					indexFrom -= 2;
				else if ( line == lineTo )
					indexTo -= 2;

				if ( line == cursorLine )
					cursorIndex -= 2;

			// else we comment it.
			} else {
				insertAt("//",line,0);

				// update selection and cursor due to the shift
				if ( line == lineFrom )
					indexFrom += 2;
				else if ( line == lineTo )
					indexTo += 2;

				if ( line == cursorLine )
					cursorIndex += 2;

			}
		}

		// Update selection.
		// If the selection is inversed,
		// reverse ***From and ****To.
		if (cursorLine == lineFrom)
			setSelection(lineTo,indexTo,lineFrom,indexFrom);
		else
			setSelection(lineFrom,indexFrom,lineTo,indexTo);

		endUndoAction();
	}

}


/**

  */
void QsciScintillaExtended::indent()
{
	int line,index;
	getCursorPosition(&line,&index);

	// If there is no selection
	if ( ! hasSelectedText() ) {
		int line,index;
		getCursorPosition(&line,&index);
		QsciScintilla::indent(line);

	// else if this is a selection
	} else {
		int lineFrom,indexFrom,lineTo,indexTo;
		getSelection(&lineFrom,&indexFrom,&lineTo,&indexTo);

		beginUndoAction();
		for ( int line = lineFrom ; line <= lineTo ; line++ ) {
			QsciScintilla::indent(line);
		}
		endUndoAction();
	}
}

void QsciScintillaExtended::unindent()
{
	int line,index;
	getCursorPosition(&line,&index);

	// If there is no selection
	if ( ! hasSelectedText() ) {
		int line,index;
		getCursorPosition(&line,&index);
		QsciScintilla::unindent(line);

	// else if this is a selection
	} else {
		int lineFrom,indexFrom,lineTo,indexTo;
		getSelection(&lineFrom,&indexFrom,&lineTo,&indexTo);

		beginUndoAction();
		for ( int line = lineFrom ; line <= lineTo ; line++ ) {
			QsciScintilla::unindent(line);
		}
		endUndoAction();
	}
}

void QsciScintillaExtended::next( const QString &  ) const
{

}

void QsciScintillaExtended::replaceAll( const QString & search, const QString & replaceBy, bool isRegexp, bool isCaseSensitive, bool wholeWordOnly)
{
	if ( search.isEmpty() )
		return;

	beginUndoAction();
	if ( findFirst(search, isRegexp, isCaseSensitive, wholeWordOnly, false, true, 0, 0, false) ) {
		replace(replaceBy);
		// while (findNext())  // => bug (search MyClass, replaceBy MyClass2 => don't find the last O_o)
		while (findFirst(search, isRegexp, isCaseSensitive, wholeWordOnly, false, true, -1, -1, false))
			replace(replaceBy);
	}
	endUndoAction();
}


/**
  */
void QsciScintillaExtended::switchMarker(int line)
{
	if (line == -1) {
		int index;
		getCursorPosition(&line, &index);
	}

	if ( ! (markersAtLine(line) & (1 << Bookmark)) )
		markerAdd(line, Bookmark);
	else
		markerDelete(line, Bookmark);
}


/**
  */
void QsciScintillaExtended::goToNextMarker()
{
	int line, index;
	getCursorPosition(&line, &index);
	line = markerFindNext(line+1, 1 << Bookmark);

	// It don't find a marker
	if (line == -1)
		// We try to search from the beginning of the document
		line = markerFindNext(0, 1 << Bookmark);

	if (line != -1)
		setCursorPosition( line, 0 );
}


/**
  */
void QsciScintillaExtended::goToPreviousMarker()
{
	int line, index;
	getCursorPosition(&line, &index);
	line = markerFindPrevious(line-1, 1 << Bookmark);

	// It don't find a marker
	if (line == -1)
		// We try to search from the end of the document
		line = markerFindPrevious(length()-1, 1 << Bookmark);

	if (line != -1)
		setCursorPosition( line, 0 );
}


/**
  */
void QsciScintillaExtended::setError( int line )
{
	if ( line == -1 )
		line = currentLine();
	markerAdd(line, Error);
}


/**
  */
void QsciScintillaExtended::clearError( int line )
{
	if ( line == -1 )
		line = currentLine();
	markerDelete(line, Error);
}


/**
  */
void QsciScintillaExtended::clearAllErrors()
{
	markerDeleteAll(Error);
}



/**
  */
void QsciScintillaExtended::setSyntaxError( int line )
{
	if ( line == -1 )
		line = currentLine();

	//
	int start = positionFromLineIndex(line, 0), len = lineLength(line);
	/* don't set the indicator on whitespace */
	const QString lineText = text(line);
	int i = 0;
	while ( i < lineText.length() && isspace( lineText.at(i++).toLatin1() ) ) {
		start++;
		len--;
	}

	SendScintilla( (unsigned int) SCI_SETINDICATORCURRENT, (long unsigned int) Error, (long int) 0 );
	SendScintilla( (unsigned int) SCI_INDICATORFILLRANGE, (long unsigned int) start, (long int) len );

	d->syntaxErrorOnLine << line;
}


/**
  */
void QsciScintillaExtended::clearSyntaxError( int line )
{
	if ( line == -1 )
		line = currentLine();

	//
	int start = positionFromLineIndex(line, 0), len = lineLength(line);
	// don't set the indicator on whitespace
	const QString lineText = text(line);
	int i = 0;
	while ( i < lineText.length() && isspace( lineText.at(i++).toLatin1() ) ) {
		start++;
		len--;
	}

	SendScintilla( (unsigned int) SCI_INDICATORCLEARRANGE, (long unsigned int) start, (long int) len );

	d->syntaxErrorOnLine.removeAll(line);
}


/**
  */
void QsciScintillaExtended::clearAllSyntaxErrors()
{
	foreach( int line, d->syntaxErrorOnLine )
		clearSyntaxError(line);
}


/**
  */
void QsciScintillaExtended::resetZoom()
{
	zoomTo(0);
}


/**
  */
void QsciScintillaExtended::onModificationChanged(bool isModified)
{
	if ( ! d->tabWidget )
		return;

	int index = d->tabWidget->indexOf(this);

	if (isModified && ! d->tabWidget->tabText(index).endsWith("*")) {
		QString newText = d->tabWidget->tabText(index) + "*";
		d->tabWidget->setTabText(index,newText);

	} else if (! isModified && d->tabWidget->tabText(index).endsWith("*")) {
		QString newText = d->tabWidget->tabText(index);
		newText.chop(1);
		d->tabWidget->setTabText(index,newText);
	}

	emit modificationChanged(index);
}


/**
  */
void QsciScintillaExtended::onMarginClicked(int, int line, Qt::KeyboardModifiers)
{
	switchMarker(line);
}
