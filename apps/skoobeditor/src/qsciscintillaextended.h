#ifndef QSCISCINTILLAEXTENDED_H
#define QSCISCINTILLAEXTENDED_H

#include <Qsci/qsciscintilla.h>

class QCompleter;

class QsciScintillaExtended : public QsciScintilla
{
	Q_OBJECT

public:
	enum MarkersId {Bookmark, Error, Package, Class};
	enum Lexer {None, Skool, Skoor};

	QsciScintillaExtended( QWidget *parent = 0);
	QsciScintillaExtended( Lexer lex, QWidget *parent = 0);
	~QsciScintillaExtended();
	QsciScintillaExtended(const QString & filename, QWidget *parent = 0);

	QString title() const;
	QString filename() const;
	Lexer lexerEnum() const;
	QString package() const;

	int currentLine() const;
	void setCurrentLine( int line );
	int currentIndex() const;
	void setCurrentIndex( int index );
	int currentPos() const;
	QPoint caretPosition() const;

	bool isComment(int line, int index) const;
	bool isComment(int position) const;
	int getStyleAt( int line, int index ) const;
	int getStyleAt( int position ) const;

	void setCompleter( QCompleter * c );
	QCompleter * completer() const;

public slots:

	void setTitle(const QString & filename);
	void setFilename(const QString & filename);
	void setLexer(Lexer lex);

	/**
	  Switch comments on the selected block.
	  Add and remove comments at the start of each selected lines,
	  or, if any selection, the current line.
	  */
	void switchComment();

	/**

	  */
	void indent();
	void unindent();

	/**
	  */
	void next( const QString & search ) const;
	void replaceAll( const QString & search, const QString & replace, bool isRegexp = false, bool isCaseSensitive = true, bool wholeWordOnly = false );

	/**
	  */
	void switchMarker(int line = -1);
	void goToNextMarker();
	void goToPreviousMarker();

	/**
	  */
	void setError( int line = -1 );
	void clearError( int line = -1 );
	void clearAllErrors();

	void setSyntaxError( int line = -1 );
	void clearSyntaxError( int line = -1 );
	void clearAllSyntaxErrors();

	/**
	  */
	void resetZoom();

	/// Autocomplete from QCompleter and its model.
	void autoCompleteFromCompleter();

signals:
	void filenameChanged(const QString & oldFilename, const QString & newFilename);

protected:
	void keyPressEvent(QKeyEvent *e);
	void focusInEvent(QFocusEvent *e);

protected slots:
	void onMarginClicked(int margin, int line, Qt::KeyboardModifiers state);
	void insertCompletion(const QString& completion);

private:
	Q_DISABLE_COPY(QsciScintillaExtended);

	struct PrivateData;
	PrivateData * d;

	void initParameters();
};

#endif // QSCISCINTILLAEXTENDED_H
