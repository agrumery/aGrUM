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
	const QString m_title;
	QString m_filename;
	QString m_paths;
	std::string m_text;
	const bool m_isDocModified;
	gum::prm::skool::SkoolReader * m_reader;
	mutable QMutex m_mutex;

public:
	SkoolInterpretation( const QsciScintillaExtended * sci, QObject * parent );
	~SkoolInterpretation();

	const QsciScintillaExtended * document() const;
	const QString & documentTitle() const;
	void setPaths( const QList<QString> & paths );
	void addPath( const QString & path );
	void addPaths( const QList<QString> & paths );

	const gum::prm::skool::SkoolReader * reader();

protected:
	void run();
};

#endif // SKOOLINTERPRETATION_H
