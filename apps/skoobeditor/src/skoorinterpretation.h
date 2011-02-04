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

	QString m_filename;
	QList<QString> m_paths;
	const bool m_syntaxMode;
	std::string m_text;
	gum::prm::skoor::SkoorInterpreter * m_interpreter;
	mutable QMutex m_mutex;

public:
	SkoorInterpretation( QObject * parent, bool syntaxMode = true );
	SkoorInterpretation( const QString & filename, QObject * parent, bool syntaxMode = true );
	~SkoorInterpretation();

	bool isSyntaxMode() const;
	QString filename() const;
	void setDocument( const QString & text );
	void setPath( const QList<QString> & paths );
	void addPath( const QString & path );

	const gum::prm::skoor::SkoorInterpreter * interpreter();

protected:
	void run();
};


#endif // SKOORINTERPRETATION_H
