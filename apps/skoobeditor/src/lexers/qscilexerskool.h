#ifndef QSCILEXERSKOOL_H
#define QSCILEXERSKOOL_H

#include <QObject>

#include <Qsci/qscilexerjava.h>


//! \brief The QsciLexerSkool class encapsulates the Scintilla Skool lexer.

class QsciLexerSkool : public QsciLexerJava {
    Q_OBJECT

  public:
    //! Construct a QsciLexerSkool with parent \a parent.  \a parent is
    //! typically the QsciScintilla instance.
    QsciLexerSkool( QObject * parent = 0 );

    //! Destroys the QsciLexerSkool instance.
    virtual ~QsciLexerSkool();

    //! Returns the name of the language.
    const char * language() const;

    //! Returns the set of keywords for the keyword set \a set recognised
    //! by the lexer as a space separated string.
    const char * keywords( int set ) const;
};

#endif // QSCILEXERSKOOL_H
