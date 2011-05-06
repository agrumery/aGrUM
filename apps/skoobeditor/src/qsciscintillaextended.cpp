#include "qsciscintillaextended.h"

#include "qscilexerskoor.h"
#include "qscilexerskool2.h"

#include <Qsci/qsciapis.h>
#include <QFileInfo>
#include <QAction>
#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QDebug>

//
struct QsciScintillaExtended::PrivateData {
	QString title;
	QString filename;
	QCompleter * completer;
};


void QsciScintillaExtended::initParameters()
{
	d = new PrivateData;
	d->completer = 0;

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
	setMarkerBackgroundColor(QColor(255,100,100), Error);
	markerDefine(Invisible,Package);
	markerDefine(Invisible,Block);
	markerDefine(Invisible,SyntaxError);

	indicatorDefine(SquiggleIndicator, SyntaxError);
	setIndicatorForegroundColor(Qt::red, SyntaxError);

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
	initParameters();
}


/**
  */
QsciScintillaExtended::QsciScintillaExtended(Lexer lex, QWidget *parent) :
		QsciScintilla(parent)
{
	initParameters();

	setLexer(lex);
}


/**
  */
QsciScintillaExtended::QsciScintillaExtended(const QString & filename, QWidget *parent) :
		QsciScintilla(parent)
{
	initParameters();

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

	setTitle(info.fileName());

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

/// Return current file package name,
/// or an empty string if it's not define.
/// \note Work only with QsciLexerSkool2
QString QsciScintillaExtended::package()
{
	/*
	 To know in what package is this document,
	 we search for "Package" marker, which is set be the
	 QsciLexerSkool2 during styling.
	 */

	static QRegExp regex("package\\s+([a-zA-Z_][\\w_]*(\\.[a-zA-Z_][\\w_]*)*)[^\\w_]");
	int line = markerFindNext(0, 1 << Package);

	// If not found
	if ( line == -1 )
		return QString();

	// If package was moved or delete, update state.
	while ( line < lines() && ! text(line).contains(regex) ) {
		markerDelete(line, Package);
		line = markerFindNext(line + 1, 1 << Package);
	}

	if ( line >= lines() )
		return QString();
	else
		return regex.cap(1);
}

/// Return current block style (interface,class or system) and name,
/// or empty strings if any block is define.
/// \note Work only with QsciLexerSkool2
QPair<QString,QString> QsciScintillaExtended::block()
{
	/*
	 To know in what package is this document,
	 we search for "Block" marker, which is set be the
	 QsciLexerSkool2 during styling.
	 */

	static QRegExp regex("(interface|class|system)\\s+([a-zA-Z_][\\w_]*(\\.[a-zA-Z_][\\w_]*)*)[^\\w_]");
	int line = markerFindPrevious( currentLine(), 1 << Block );
	if ( line == -1 ) {
		line = markerFindNext( currentLine(), 1 << Block );
		// If not found
		if ( line == -1 )
			return QPair<QString,QString>(QString(),QString());
	}

	// If package was moved or delete, update state.
	while ( line >= 0 && ! text(line).contains(regex) ) {
		markerDelete(line, Block);
		line = markerFindPrevious(line - 1, 1 << Block);
	}

	if ( line < 0 )
		return QPair<QString,QString>(QString(),QString());
	else
		return QPair<QString,QString>(regex.cap(1),regex.cap(2));
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
  */
int QsciScintillaExtended::currentPos() const
{
	return SendScintilla(SCI_GETCURRENTPOS);
}

QPoint QsciScintillaExtended::caretPosition() const
{
	int x,y,pos = currentPos();
	x = SendScintilla(SCI_POINTXFROMPOSITION,0,pos);
	y = SendScintilla(SCI_POINTYFROMPOSITION,0,pos);
	return QPoint(x,y);
}

/**
  */
bool QsciScintillaExtended::isComment(int position) const
{
	if ( lexerEnum() == Skoor ) {
		int style = getStyleAt(position);
		return style == QsciLexerSkoor::Comment || style == QsciLexerSkoor::CommentLine ||
			style == QsciLexerSkoor::CommentDoc || style == QsciLexerSkoor::CommentLineDoc;
	} else if ( lexerEnum() == Skool )
		return qobject_cast<QsciLexerSkool2 *>(lexer())->isComment( getStyleAt(position) );
	else
		return false;
}

/// Overload method.
bool QsciScintillaExtended::isComment(int line, int index) const
{
	return isComment( positionFromLineIndex(line,index) );
}

/**
  */
int QsciScintillaExtended::getStyleAt( int position ) const
{
	return QsciScintilla::SendScintilla(SCI_GETSTYLEAT,position);
}
/// Overload method.
int QsciScintillaExtended::getStyleAt( int line, int index ) const
{
	return getStyleAt( positionFromLineIndex(line,index) );
}


/// Set Qt completer for autoCompletion.
void QsciScintillaExtended::setCompleter( QCompleter * c )
{
	if (d->completer)
		QObject::disconnect(d->completer, 0, this, 0);

	d->completer = c;

	if (!d->completer)
		return;

	d->completer->setWidget(this);
	d->completer->setCompletionMode(QCompleter::PopupCompletion);
	d->completer->setCaseSensitivity(Qt::CaseInsensitive);
	QObject::connect(d->completer, SIGNAL(activated(QString)),
					 this, SLOT(insertCompletion(QString)));
//	QObject::connect(d->completer, SIGNAL(highlighted(QString)),
//					 this, SLOT(insertCompletion(QString)));
}

/// Retrieve completer.
QCompleter * QsciScintillaExtended::completer() const
{
	return d->completer;
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
	// don't set the indicator on whitespace
	const QString lineText = text(line);
	int start = 0, len = lineText.length();
	int i = 0;
	while ( i < len && isspace( lineText.at(i++).toLatin1() ) )
		start++;

	fillIndicatorRange(line, start, line, len-start, SyntaxError);
	markerAdd( line, SyntaxError );

}


/**
  */
void QsciScintillaExtended::clearSyntaxError( int line )
{
	if ( line == -1 )
		line = currentLine();
	clearIndicatorRange(line, 0, line, lineLength(line), SyntaxError);
	markerDelete(line, SyntaxError);
}


/**
  */
void QsciScintillaExtended::clearAllSyntaxErrors()
{
	int line = -1;
	while ( (line = markerFindNext(line+1, 1<<SyntaxError)) != -1 )
		clearSyntaxError(line);
}


/**
  */
void QsciScintillaExtended::resetZoom()
{
	zoomTo(0);
}


/// Autocomplete from QCompleter and its model.
void QsciScintillaExtended::autoCompleteFromCompleter()
{
	if ( ! d->completer )
		return;

	QString lineText = text( currentLine() );
	int caret = currentIndex(), start = caret - 1;

	// Search start of prefix.
	while ( start >= 0 && ( lineText[start].isLetterOrNumber() ||
							lineText[start] == '_' ||
							lineText[start] == '.' ||
							lineText[start] == '[' ||
							lineText[start] == ']' ) ) start--;
	start++;
	QString prefix = lineText.mid(start, caret - start);

	if ( prefix.size() == 0 ) {
		d->completer->popup()->hide();
		return;
	}

	d->completer->setCompletionPrefix( prefix );
	int count = d->completer->completionCount();

	QRect cr;
	if ( count == 0 )
		return;
	else if ( count == 1 && ! d->completer->popup()->isVisible() ) {
		// Insert directly the result
		d->completer->setCurrentRow(0);
		insertCompletion( d->completer->currentCompletion() );
	} else {
		// Compute popup size.
		cr = QRect( caretPosition(), QSize( d->completer->popup()->sizeHintForColumn(0) +
											d->completer->popup()->verticalScrollBar()->sizeHint().width(),
											textHeight(currentLine()) ) );
		// Select first result
		d->completer->popup()->setCurrentIndex(d->completer->completionModel()->index(0, 0));
		// Show popup
		d->completer->complete(cr);
	}
}

/**
  */
void QsciScintillaExtended::onMarginClicked(int, int line, Qt::KeyboardModifiers)
{
	switchMarker(line);
}

/**
  */
void QsciScintillaExtended::insertCompletion(const QString& completion)
{
	if (d->completer->widget() != this)
		return;
	int extra = completion.length() - d->completer->completionPrefix().length();
	insert( completion.right(extra) );
	setCurrentIndex( currentIndex() + extra );
}

void QsciScintillaExtended::focusInEvent(QFocusEvent *e)
{
	if (d->completer)
		d->completer->setWidget(this);
	QsciScintilla::focusInEvent(e);
}

void QsciScintillaExtended::keyPressEvent(QKeyEvent *e)
{
	if (d->completer && d->completer->popup()->isVisible()) {
		// The following keys are forwarded by the completer to the widget
	   switch (e->key()) {
	   case Qt::Key_Enter:
	   case Qt::Key_Return:
	   case Qt::Key_Escape:
	   case Qt::Key_Tab:
	   case Qt::Key_Backtab:
			e->ignore();
			return; // let the completer do default behavior
	   case Qt::Key_Right:
			QsciScintilla::keyPressEvent(e);
			autoCompleteFromCompleter();
			return;
	   case Qt::Key_Left:
			QsciScintilla::keyPressEvent(e);
			autoCompleteFromCompleter();
			return;
	   default:
		   break;
	   }
	   if ( e->modifiers() == Qt::ControlModifier )
		   return;

	   QsciScintilla::keyPressEvent(e);
	   autoCompleteFromCompleter();
	   return;
	}

	QsciScintilla::keyPressEvent(e);
}
