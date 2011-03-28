#ifndef SKOORINTERPRETATION_H
#define SKOORINTERPRETATION_H

#include <QThread>
#include <QMutex>
#include <QList>
#include "qsciscintillaextended.h"

#include <agrum/prm/skoor/SkoorInterpreter.h>

/**
 * This class read a skoor and interprete it.
 * If \a syntaxMode is true, any calcul will be done.
 * It send a finished() signal when done.
 * When done, you can retrieve the SkoorInterpreter.
 */
class SkoorInterpretation : public QThread
{
	Q_OBJECT

	const QsciScintillaExtended * m_sci;
	const QString m_title;
	QString m_filename;
	QList<QString> m_paths;
	const bool m_syntaxMode;
	const bool m_isDocModified;
	std::string m_text;
	gum::prm::skoor::SkoorInterpreter * m_interpreter;
	mutable QMutex m_mutex;

public:
	SkoorInterpretation( const QsciScintillaExtended * sci, QObject * parent, bool syntaxMode = true );
	~SkoorInterpretation();

	bool isSyntaxMode() const;
	const QsciScintillaExtended * document() const;
	const QString & documentTitle() const;
	void setPaths( const QList<QString> & paths );
	void addPath( const QString & path );
	void addPaths( const QList<QString> & paths );

	const gum::prm::skoor::SkoorInterpreter * interpreter();

protected:
	void run();
};


#endif // SKOORINTERPRETATION_H
