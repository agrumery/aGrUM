#include "qscilexerskool.h"

#include <Qsci/qsciapis.h>
#include <QRegExp>
#include <QDebug>

// The ctor.
QsciLexerSkool::QsciLexerSkool(QObject * parent) : QsciLexerJava(parent)
{
	// Courier new 10pt
	QFont font("monospace");
	font.setPointSize(10);
	font.setStyleHint(QFont::Monospace);
	setFont(font,QsciLexerJava::Default);
	setFont(font,QsciLexerJava::Comment);
	setFont(font,QsciLexerJava::CommentLine);
	setFont(font,QsciLexerJava::Number);
	setFont(font,QsciLexerJava::Operator);
	setFont(font,QsciLexerJava::Identifier);
	setFont(font,QsciLexerJava::GlobalClass);
	font.setBold(true);
	setFont(font,QsciLexerJava::CommentDoc);
	setFont(font,QsciLexerJava::CommentLineDoc);
	setFont(font,QsciLexerJava::Keyword);
	setFont(font,QsciLexerJava::Operator);

	// Color
	setColor(Qt::blue, QsciLexerJava::CommentDoc);
	setColor(Qt::blue, QsciLexerJava::CommentLineDoc);
	setColor(Qt::darkGreen, QsciLexerJava::CommentLine);
	setColor(Qt::darkGreen, QsciLexerJava::Comment);
	setColor(QColor(255,170,0), QsciLexerJava::Number);

	QsciAPIs *api = new QsciAPIs(this); // déclaration de l'api
	api->load(":/api/skool.api");
	api->prepare(); // préparation  de l'auto-completion
	setAPIs(api); // application de l'api au lexer

}

// The dtor.
QsciLexerSkool::~QsciLexerSkool()
{
}

// Returns the language name.
const char * QsciLexerSkool::language () const
{
	return "Skool";
}

// Returns the set of keywords.
const char * QsciLexerSkool::keywords (int set) const
{
	if (set != 1)
		return 0;

	return "boolean class default dependson extends float implements import int interface noisyOr package system type";
}
