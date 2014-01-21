#include "qscilexerskool2.h"

#include "qsciscintillaextended.h"

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscistyle.h>
#include <Qsci/qsciapis.h>

#include <QDebug>


struct QsciLexerSkool2::PrivateData {
  QRegExp commentLine;
  QRegExp commentLineDoc;
  QRegExp commentBlock;
  QRegExp commentBlockDoc;
  QRegExp function;
  QRegExp symbol;
  QRegExp id;
  QRegExp number;
  QRegExp operators;
  QRegExp keywords;
  QRegExp spaces;

  QRegExp assignation;
  QRegExp declarationReference;
  QRegExp declarationAttribute;

  QStringList waitBlock;
  QStringList keywordsList;
  QStringList typesList;

  QFont normalFont;
  QFont normalBoldFont;

  QsciAPIs* api;
};

bool isOperator ( QChar c ) {
  return c.category() == QChar::Mark_Enclosing || c.category() == QChar::Symbol_Math;
}

/// Constructor
QsciLexerSkool2::QsciLexerSkool2 ( QObject* parent ) : QsciLexerCustom ( parent ), d ( new PrivateData ) {
  d->waitBlock = QString ( "class interface system" ).split ( " ", QString::SkipEmptyParts );
  d->keywordsList = QString (
                      "boolean class default dependson extends float implements import int interface noisyOr package system type"
                    ).split ( " ", QString::SkipEmptyParts );
  d->typesList = QString ( "boolean float int" ).split ( " ", QString::SkipEmptyParts );

  d->commentLine = QRegExp ( "^//[^/]" );
  d->commentLineDoc = QRegExp ( "^///" );
  d->commentBlock = QRegExp ( "^/\\*[^\\*]" );
  d->commentBlockDoc = QRegExp ( "^/\\*\\*" );
  d->function = QRegExp ( "^([a-zA-Z_][a-zA-Z_0-9]*)\\s*\\(" );
  d->symbol = QRegExp ( "^[a-zA-Z_]\\w*" );
  d->id = QRegExp ( "^[a-zA-Z_]\\w*(\\.[a-zA-Z_]\\w*)*" );
  d->number = QRegExp ( "^\\d+(\\.\\d+)?" );
  d->operators = QRegExp ( "^(\\=|\\+|\\-|\\{|\\}|\\[|\\]|\\.|\\;|\\,|\\:|\\(|\\)|\\*)" );
  d->keywords = QRegExp ( "^(" + d->keywordsList.join ( "|" ) + ")\\s" );
  d->spaces = QRegExp ( "^\\s+" );

  d->assignation = QRegExp ( "^(([a-zA-Z_][a-zA-Z_0-9]*)(\\.([a-zA-Z_][a-zA-Z_0-9]*))*)(\\s*)((-|\\+)?\\=)" );
  d->declarationReference = QRegExp ( "^[a-zA-Z_][\\.a-zA-Z_0-9]*(\\[\\d*\\])?\\s+[a-zA-Z_][a-zA-Z_0-9]*\\s*;" );
  d->declarationAttribute = QRegExp ( "^[a-zA-Z_][\\.a-zA-Z_0-9]*(\\[\\d*\\])?\\s[^\\{\\;\\=]*\\{" );

  d->normalFont = QFont ( "Courier new", 10 );
  d->normalFont.setStyleHint ( QFont::Monospace );
  d->normalBoldFont = QFont ( d->normalFont );
  d->normalBoldFont.setBold ( true );

  d->api = new QsciAPIs ( this ); // déclaration de l'api
  d->api->load ( ":/api/skool.api" );
  d->api->prepare(); // préparation  de l'auto-completion
  setAPIs ( d->api ); // application de l'api au lexer
}


/// Destructor
QsciLexerSkool2::~QsciLexerSkool2() {
  delete d;
}



const char* QsciLexerSkool2::language() const {
  return "Skool";
}

QString QsciLexerSkool2::description ( int style ) const {
  switch ( style ) {
    case Default:
      return "Default style";

    case CommentLine:
      return "CommentLine style";

    case CommentLineDoc:
      return "CommentLineDoc style";

    case CommentBlock:
      return "CommentBlock style";

    case CommentBlockDoc:
      return "CommentBlockDoc style";

    case Keyword:
      return "Keyword style";

    case Operator:
      return "Operator style";

    case Type:
      return "Type style";

    case Function:
      return "Function style";

    case Number:
      return "Number style";

    case Error:
      return "Doesn't exist in Skool language.";

    default:
      return "unknow style";
  }
}


QColor QsciLexerSkool2::defaultColor ( int style ) const {
  switch ( style ) {
    case Default:
    case Function:
      return Qt::black;

    case CommentLine:
    case CommentBlock:
      return Qt::darkGreen;

    case CommentBlockDoc:
    case CommentLineDoc:
      return Qt::blue;

    case Number:
      return QColor ( 255, 170, 0 );

    case Type:
      return Qt::darkMagenta;

    case Error:
      return Qt::red;

    default:
      return Qt::black;
  }
}


QFont  QsciLexerSkool2::defaultFont ( int style ) const {
  switch ( style ) {
    case Default:
    case CommentBlock:
    case CommentLine:
    case Type:
    case Number:
      return d->normalFont;

    case CommentBlockDoc:
    case CommentLineDoc:
    case Keyword:
    case Operator:
    case Function:
      return d->normalBoldFont;

    default:
      return d->normalFont;
  }
}


QColor QsciLexerSkool2::defaultPaper ( int ) const {
  return Qt::white;
}

const char* QsciLexerSkool2::keywords ( int set ) const {
  return d->keywordsList.join ( " " ).toUtf8().constData();
}

/**
  */
void QsciLexerSkool2::styleText ( int start, int end ) {
  if ( !editor() || end <= start )
    return;

  int utf8Len = end - start;

  char* chars = ( char* ) malloc ( utf8Len + 1 );

  if ( chars == NULL )
    return;

  editor()->SendScintilla ( QsciScintilla::SCI_GETTEXTRANGE,
                            start, end, chars );

  const QString source = QString::fromUtf8 ( chars );

  free ( chars );

  int style, previousStyle, lineNb = 0, level = QsciScintilla::SC_FOLDLEVELBASE;

  bool isWaitingBlock = false, isWaitingPackage = false;

  bool startInComment = false, isNewLine = true, newLineJustMatch = false, startBlock = false;

  QString line;

  // If we don't start at the beggining of the document,
  // we retrieve previous context.
  if ( start != 0 ) {
    previousStyle = editor()->SendScintilla ( QsciScintilla::SCI_GETSTYLEAT, start - 1 );
    startInComment = ( ( previousStyle == CommentBlock ) || ( previousStyle == CommentBlockDoc ) );

    // We get line number corresponding to start position.
    editor()->lineIndexFromPosition ( start, &lineNb, &style ); // style get index but it is unneeded

    // We get the previous level
    level = editor()->SendScintilla ( QsciScintilla::SCI_GETFOLDLEVEL, lineNb - 1 ); // start != 0 => lineNb != 0

    if ( level & QsciScintilla::SC_FOLDLEVELHEADERFLAG ) { // If previous line is a start of block comment
      level -= QsciScintilla::SC_FOLDLEVELHEADERFLAG; // Remove header flag
      level++; // Increment level
    }
  }

  // Le texte est encodé en Utf8 dans Scintilla, donc 1 char ne vaut pas forcément 1 caractère.
  // Comme je fais mes tests sur des QString, qui cache cette information,
  // les longueurs sur lesquelles il faut appliquer le style sont en caractère, alors que Scintilla
  // attend des longueurs en char.
  // Je suis donc obligé d'utiliser deux compteurs : un pour la source en QString -et donc en caractère-,
  // et l'autre pour Scintilla -en char.
  for ( int i = 0, len = 0, iUtf8 = 0, lenUtf8 = 0 ; i < source.size() ; i += len, iUtf8 += lenUtf8 ) {

    // On récupère la ligne dans la variable line, avec le retour chariot.
    len = source.indexOf ( "\n", i ) - i + 1;

    if ( len < 1 )
      line = source.mid ( i, source.length() );
    else
      line = source.mid ( i, len );

    // Usefull if line == '\0', at the end of the file.
//  if (line.isEmpty())
//   break;

    // Space or unprintable char
    if ( ! startInComment && line.contains ( d->spaces ) ) {
      if ( d->spaces.cap ( 0 ).contains ( "\n" ) ) { // match \n
        isNewLine = true;

        if ( ! startBlock )
          editor()->SendScintilla ( QsciScintilla::SCI_SETFOLDLEVEL, lineNb, level );

        //lineNb++;
        startBlock = false;
      }

      newLineJustMatch = true;

      len = d->spaces.cap ( 0 ).length();
      style = Default;

      // Continue CommentBlock
    } else if ( startInComment ) { // Continue CommentBlock

      style = previousStyle;
      len = line.indexOf ( "*/" ) + 2;

      if ( len < 2 ) {
        len = line.length();
        editor()->SendScintilla ( QsciScintilla::SCI_SETFOLDLEVEL, lineNb, level );
        //lineNb++;
      } else {
        level--;
        editor()->SendScintilla ( QsciScintilla::SCI_SETFOLDLEVEL, lineNb, level );
        startInComment = false;
      }

      // CommentLine
    } else if ( line.contains ( d->commentLine ) ) { // CommentLine
      style = CommentLine;
      len = line.length();
      editor()->SendScintilla ( QsciScintilla::SCI_SETFOLDLEVEL, lineNb, level );
      //lineNb++;

      // CommentLineDoc
    } else if ( line.contains ( d->commentLineDoc ) ) { // CommentLineDoc
      style = CommentLineDoc;
      len = line.length();
      editor()->SendScintilla ( QsciScintilla::SCI_SETFOLDLEVEL, lineNb, level );
      //lineNb++;

      // CommentBlock
    } else if ( line.contains ( d->commentBlock ) ) { // CommentBlock

      style = CommentBlock;
      len = line.indexOf ( "*/" ) + 2;

      if ( len < 2 ) {
        len = line.length(); // With \n

        if ( ! startBlock ) // Si on a pas déjà commencé un bloc
          editor()->SendScintilla ( QsciScintilla::SCI_SETFOLDLEVEL, lineNb,
                                    level | QsciScintilla::SC_FOLDLEVELHEADERFLAG );

        //lineNb++;
        level++;

        startInComment = true;
      }

      // CommentBlockDoc
    } else if ( line.contains ( d->commentBlockDoc ) ) { // CommentBlockDoc

      style = CommentBlockDoc;
      len = line.indexOf ( "*/" ) + 2;

      if ( len < 2 ) {
        len = line.length(); // With \n
        editor()->SendScintilla ( QsciScintilla::SCI_SETFOLDLEVEL, lineNb,
                                  level | QsciScintilla::SC_FOLDLEVELHEADERFLAG );
        //lineNb++;
        level++;
        startInComment = true;
      }

      // Number
    } else if ( line.contains ( d->number ) ) { // #^\d+(\.\d+)?#
      style = Number;
      len = d->number.cap ( 0 ).length();

      // Operator
    } else if ( line.contains ( d->operators ) ) { // #^|\=|\+\=|\-\=|\{|\}|\[|\]|\.|\;|\,|\(|\)#
      style = Operator;
      len = d->operators.cap ( 0 ).length();

      if ( d->operators.cap ( 0 ) == "{" ) {
        if ( ! startBlock ) // Si on avait pas déjà commencé un bloc
          editor()->SendScintilla ( QsciScintilla::SCI_SETFOLDLEVEL, lineNb,
                                    level | QsciScintilla::SC_FOLDLEVELHEADERFLAG );

        level++;

        startBlock = true;

      } else if ( d->operators.cap ( 0 ) == "}" ) {
        level--;
        startBlock = false; // au cas où la { et la } sont sur la même ligne.

      }

      // Keyword
    } else if ( line.contains ( d->keywords ) ) { // #^(type|class|import|package|...)\s#
      style = Keyword;
      len = d->keywords.cap ( 0 ).length();

      //

      if ( d->keywords.cap ( 1 ) == "package" ) {
        isWaitingPackage = true;
        isWaitingBlock = false;
      } else if ( d->waitBlock.contains ( d->keywords.cap ( 1 ) ) ) {
        isWaitingBlock = true;
        isWaitingPackage = false;
      } else {
        isWaitingBlock = false;
        isWaitingPackage = false;
      }

      // Symbol
    } else if ( line.contains ( d->id ) ) { // #^[a-zA-Z_]([a-zA-Z0-9_])*#
      len = d->id.cap ( 0 ).length();
      style = Default;

      //

      if ( isWaitingPackage ) {
        style = Type;
        editor()->markerAdd ( lineNb, QsciScintillaExtended::Package );
      } else if ( isWaitingBlock ) {
        style = Type;
        editor()->markerAdd ( lineNb, QsciScintillaExtended::Block );
      }

      isWaitingPackage = false;

      isWaitingBlock = false;

      // Sinon
    } else { // Sais pas ce que c'est
      style = Error;
      len = line.length();
    }

    //
    if ( newLineJustMatch )
      newLineJustMatch = false;
    else
      isNewLine = false;

    //
    if ( style != Keyword ) {
      isWaitingBlock = false;
      isWaitingPackage = false;
    }

    // If we reach end of line, increment compter
    if ( len == line.length() )
      lineNb++;

    // len was calculated in QString
    // we must translate it in Utf8 bytes length.
    lenUtf8 = line.mid ( 0, len ).toUtf8().length();

    Q_ASSERT ( lenUtf8 > 0 );

    Q_ASSERT ( start + iUtf8 + lenUtf8 <= end );

    startStyling ( start + iUtf8 );

    setStyling ( lenUtf8, style );

    previousStyle = style;
  }

  // Utile pour la fin de fichier, si il n'y a pas de retour à la ligne
  editor()->SendScintilla ( QsciScintilla::SCI_SETFOLDLEVEL, lineNb, level );
}


/**
  */
void QsciLexerSkool2::addToApi ( const QString& entry ) {
  d->api->add ( entry );
}

