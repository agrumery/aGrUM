#ifndef SKOOLINTERPRETATION_H
#define SKOOLINTERPRETATION_H

#include <QThread>
#include <QMutex>

#include "qsciscintillaextended.h"

#include <agrum/prm/skool/SkoolReader.h>

/**
 * This class read a skool file to check its syntax.
 * It send a finished() signal when done.
 * When done, you can retrieve the SkoolReader.
 */
class SkoolInterpretation : public QThread
{
	Q_OBJECT

	const QsciScintillaExtended * m_sci;
	QString m_title;
	QString m_paths;
	std::string m_text;
	gum::prm::skool::SkoolReader * m_reader;
	mutable QMutex m_mutex;
	bool m_readerTaken;

public:
	SkoolInterpretation( QObject * parent );
	SkoolInterpretation( const QString & skoolFilename, QObject * parent);
	SkoolInterpretation( const QsciScintillaExtended * sci, QObject * parent );
	~SkoolInterpretation();

	const QsciScintillaExtended * document() const;
	QString documentTitle() const;
	void setDocument( const QString & text );
	void setPath( const QList<QString> & paths );
	void addPath( const QString & path );

	gum::prm::skool::SkoolReader * reader();

protected:
	void run();
};

#endif // SKOOLINTERPRETATION_H
