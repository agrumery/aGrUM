#include "skoolinterpretation.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>

using namespace gum::prm::skool;


SkoolInterpretation::SkoolInterpretation( const QsciScintillaExtended * sci, QObject * parent ) :
		QThread(parent), m_sci(sci), m_title(sci->title()), m_isDocModified(m_sci->isModified()), m_reader(0)
{
	//m_interpreter = new gum::prm::skoor::SkoorInterpreter();
	if ( m_sci != 0 ) {
		m_filename = sci->filename();
		// If file is not saved in a file
		if ( m_filename.isEmpty() )
			// Parse code
			m_text = sci->text().toStdString();

		// If file is saved in a file and has been modified
		else if ( m_isDocModified ) {
			// Save modifications in a temporary file
			m_filename += ".bak";
			QFile file( m_filename );
			if ( file.open(QFile::WriteOnly) ) {
				file.write( sci->text().toUtf8() );
				file.close();
			// If modifications can't be saved, parse code
			} else {
				m_text = sci->text().toStdString();
				m_filename.clear();
				addPath( QFileInfo(sci->filename()).path() );
			}
		}
	}
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

const QString & SkoolInterpretation::documentTitle() const
{
	return m_title;
}

void SkoolInterpretation::setPaths( const QList<QString> & paths )
{
	QMutexLocker locker(&m_mutex);
	m_paths.clear();
	locker.unlock();
	addPaths(paths);
}

void SkoolInterpretation::addPath( const QString & path )
{
	QMutexLocker locker(&m_mutex);
	if ( path.endsWith(QChar('/')) )
		m_paths += path + ";";
	else
		m_paths += path + "/;";
}

void SkoolInterpretation::addPaths( const QList<QString> & paths )
{
	foreach ( QString path, paths )
		addPath(path);
}

void SkoolInterpretation::run()
{
	QMutexLocker locker( &m_mutex );

	if (m_reader)
		delete m_reader->prm();
	delete m_reader;

	m_reader = new SkoolReader();
	m_reader->setClassPath( m_paths.toStdString() );

	if ( m_filename.isEmpty() ) {
		m_reader->readString( m_text );
	} else
		m_reader->readFile( m_filename.toStdString() );
}

/**
  When you take the reader, you must delete it yourself.
  */
const SkoolReader * SkoolInterpretation::reader() {
	this->wait();
	return m_reader;
}
