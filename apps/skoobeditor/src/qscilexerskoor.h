#ifndef QSCILEXERSKOOR_H
#define QSCILEXERSKOOR_H

#include <QObject>

#include <Qsci/qscilexerjava.h>


//! \brief The QsciLexerSkoor class encapsulates the Scintilla Skoor lexer.
class QsciLexerSkoor : public QsciLexerJava
{
	Q_OBJECT

public:
	//! Construct a QsciLexerSkoor with parent \a parent.  \a parent is
	//! typically the QsciScintilla instance.
	QsciLexerSkoor(QObject * parent = 0);

	//! Destroys the QsciLexerSkoor instance.
	virtual ~QsciLexerSkoor();

	//! Returns the name of the language.
	const char * language () const;

	//! Returns the set of keywords for the keyword set \a set recognised
	//! by the lexer as a space separated string.
	const char * keywords (int set) const;
};

#endif // QSCILEXERSKOOR_H
