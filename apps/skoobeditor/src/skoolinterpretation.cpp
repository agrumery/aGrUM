#include "skoolinterpretation.h"

#include <QDebug>

using namespace gum::prm::skool;

SkoolInterpretation::SkoolInterpretation( QObject * parent ) :
		QThread(parent), m_sci(0), m_reader(0)
{
}

SkoolInterpretation::SkoolInterpretation( const QString & skoolFilename, QObject * parent) :
		QThread(parent), m_sci(0), m_title(skoolFilename), m_reader(0)
{
}

SkoolInterpretation::SkoolInterpretation( const QsciScintillaExtended * sci, QObject * parent ) :
		QThread(parent), m_sci(sci), m_title(sci->title()), m_reader(0)
{
}

SkoolInterpretation::~SkoolInterpretation()
{
	// Wait the run methods ends.
	wait();
	if (m_reader)
		delete m_reader->prm();
	delete m_reader;
}


/// Warning : this document may be 0 and it may no longer exists (-> segfault).
const QsciScintillaExtended * SkoolInterpretation::document() const
{
	QMutexLocker locker(&m_mutex);
	return m_sci;
}

QString SkoolInterpretation::documentTitle() const
{
	QMutexLocker locker(&m_mutex);
	return m_title;
}

void SkoolInterpretation::setDocument( const QString & text )
{
	QMutexLocker locker(&m_mutex);
	m_text = text.toStdString();
}

void SkoolInterpretation::setPath( const QList<QString> & paths )
{
	QMutexLocker locker(&m_mutex);
	foreach ( QString path, paths )
		m_paths += path + "/;";
}

void SkoolInterpretation::addPath( const QString & path )
{
	QMutexLocker locker(&m_mutex);
	m_paths += path + "/;";
}

void SkoolInterpretation::run()
{
	QMutexLocker locker( &m_mutex );
	if (m_reader)
		delete m_reader->prm();
	delete m_reader;
	m_reader = new SkoolReader();
	m_reader->setClassPath( m_paths.toStdString() );
	m_reader->readString( m_text );
}

/**
  When you take the reader, you must delete it yourself.
  */
const SkoolReader * SkoolInterpretation::reader() {
	this->wait();
	return m_reader;
}
