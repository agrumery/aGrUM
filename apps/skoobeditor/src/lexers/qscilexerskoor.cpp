#include "qscilexerskoor.h"

#include <Qsci/qsciapis.h>
#include <QDebug>

// The ctor.
QsciLexerSkoor::QsciLexerSkoor( QObject * parent ) : QsciLexerJava( parent ) {
  QsciAPIs *api = new QsciAPIs( this ); // déclaration de l'api
  api->load( ":/api/skoor.api" );
  api->prepare(); // préparation  de l'auto-completion
  setAPIs( api ); // application de l'api au lexer

  // Courier new 10pt
  QFont font( "courier new" );
  font.setPointSize( 10 );
  font.setStyleHint( QFont::Monospace );
  setFont( font,QsciLexerJava::Default );
  setFont( font,QsciLexerJava::Comment );
  setFont( font,QsciLexerJava::CommentLine );
  setFont( font,QsciLexerJava::Number );
  setFont( font,QsciLexerJava::Identifier );
  setFont( font,QsciLexerJava::GlobalClass );
  font.setBold( true );
  setFont( font,QsciLexerJava::CommentDoc );
  setFont( font,QsciLexerJava::CommentLineDoc );
  setFont( font,QsciLexerJava::Keyword );
  setFont( font,QsciLexerJava::Operator );

  // Color
  setColor( Qt::blue, QsciLexerJava::CommentDoc );
  setColor( Qt::blue, QsciLexerJava::CommentLineDoc );
  setColor( Qt::darkGreen, QsciLexerJava::CommentLine );
  setColor( Qt::darkGreen, QsciLexerJava::Comment );
  setColor( QColor( 255,170,0 ), QsciLexerJava::Number );
}

// The dtor.
QsciLexerSkoor::~QsciLexerSkoor() {
}

// Returns the language name.
const char * QsciLexerSkoor::language() const {
  return "Skoor";
}

// Returns the set of keywords.
const char * QsciLexerSkoor::keywords( int set ) const {
  if ( set != 1 )
    return "";

  return "? as default engine grd_engine import package query request unobserve";
}
