#include "skoolsyntaxchecker.h"

#include <QDebug>

using namespace gum::prm::skool;

SkoolSyntaxChecker::SkoolSyntaxChecker( QObject * parent ) :
		QThread(parent), m_sci(0), m_reader(0)
{
}

SkoolSyntaxChecker::SkoolSyntaxChecker( const QString & skoolFilename, QObject * parent) :
		QThread(parent), m_sci(0), m_title(skoolFilename), m_reader(0)
{
}

SkoolSyntaxChecker::SkoolSyntaxChecker( const QsciScintillaExtended * sci, QObject * parent ) :
		QThread(parent), m_sci(sci), m_title(sci->title()), m_reader(0)
{
}

SkoolSyntaxChecker::~SkoolSyntaxChecker()
{
	// Wait the run methods ends.
	wait();
	delete m_reader;
}


/// Warning : this document may be 0 and it may no longer exists (-> segfault).
const QsciScintillaExtended * SkoolSyntaxChecker::document() const
{
	QMutexLocker locker(&m_mutex);
	return m_sci;
}

QString SkoolSyntaxChecker::documentTitle() const
{
	QMutexLocker locker(&m_mutex);
	return m_title;
}

void SkoolSyntaxChecker::setDocument( const QString & text )
{
	QMutexLocker locker(&m_mutex);
	m_text = text.toStdString();
}

void SkoolSyntaxChecker::setPath( const QList<QString> & paths )
{
	QMutexLocker locker(&m_mutex);
	foreach ( QString path, paths )
		m_paths += path + "/;";
}

void SkoolSyntaxChecker::addPath( const QString & path )
{
	QMutexLocker locker(&m_mutex);
	m_paths += path + "/;";
}

void SkoolSyntaxChecker::run()
{
	QMutexLocker locker( &m_mutex );
	delete m_reader;
	m_reader = new SkoolReader();
	m_reader->setClassPath( m_paths.toStdString() );
	m_reader->readString( m_text );
}

const SkoolReader * SkoolSyntaxChecker::reader() {
	wait();
	return m_reader;
}
