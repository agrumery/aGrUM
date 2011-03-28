#include "skoorinterpretation.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>


SkoorInterpretation::SkoorInterpretation( const QsciScintillaExtended * sci, QObject * parent, bool syntaxMode ) :
		QThread(parent), m_sci(sci), m_title(m_sci->title()), m_syntaxMode(syntaxMode), m_isDocModified(m_sci->isModified()), m_interpreter(0)
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

SkoorInterpretation::~SkoorInterpretation()
{
	// Wait the run methods ends.
	wait();
	delete m_interpreter;
}

/// Warning : this document may be 0 and it may no longer exists (-> segfault).
const QsciScintillaExtended * SkoorInterpretation::document() const
{
	return m_sci;
}

const QString & SkoorInterpretation::documentTitle() const
{
	return m_title;
}

void SkoorInterpretation::setPaths( const QList<QString> & paths )
{
	QMutexLocker locker(&m_mutex);
	m_paths.clear();
	locker.unlock();
	addPaths(paths);
}

void SkoorInterpretation::addPath( const QString & path )
{
	QMutexLocker locker(&m_mutex);
	if ( path.endsWith("/") )
		m_paths += path;
	else
		m_paths += path + "/";
}

void SkoorInterpretation::addPaths( const QList<QString> & paths )
{
	foreach ( QString path, paths )
		addPath(path);
}

void SkoorInterpretation::run()
{
	QMutexLocker locker(&m_mutex);

	delete m_interpreter;

	if ( m_filename.isEmpty() ) {
		m_interpreter = new gum::prm::skoor::SkoorInterpreter();
		m_interpreter->processCommandLine( m_text );
	} else
		m_interpreter = new gum::prm::skoor::SkoorInterpreter(m_filename.toStdString());

	m_interpreter->setSyntaxMode(m_syntaxMode);
	foreach ( QString s, m_paths )
		m_interpreter->addPath( s.toStdString() );

	locker.unlock();
	m_interpreter->interpret();
}

const gum::prm::skoor::SkoorInterpreter * SkoorInterpretation::interpreter() {
	this->wait(); // Si le processus n'est pas fini, on attend
	return m_interpreter;
}


bool SkoorInterpretation::isSyntaxMode() const
{
	QMutexLocker locker(&m_mutex);
	return m_syntaxMode;
}
