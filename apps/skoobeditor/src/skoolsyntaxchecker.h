#ifndef SKOOLSYNTAXCHECKER_H
#define SKOOLSYNTAXCHECKER_H

#include <QThread>
#include <QMutex>

#include <agrum/prm/skool/SkoolReader.h>

/**
 * This class read a skool file to check its syntax.
 * It send a finished() signal when done.
 * When done, you can retrieve the SkoolReader.
 */
class SkoolSyntaxChecker : public QThread
{
	Q_OBJECT

	QString m_filename;
	QString m_paths;
	std::string m_text;
	gum::prm::skool::SkoolReader * m_reader;
	mutable QMutex m_mutex;

public:
	SkoolSyntaxChecker( QObject * parent );
	SkoolSyntaxChecker( const QString & skoolFilename, QObject * parent);
	~SkoolSyntaxChecker();

	QString filename() const;
	void setDocument( const QString & text );
	void setPath( const QList<QString> & paths );
	void addPath( const QString & path );

	const gum::prm::skool::SkoolReader * reader();

protected:
	void run();
};

#endif // SKOOLSYNTAXCHECKER_H
