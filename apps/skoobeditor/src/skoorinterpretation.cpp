#include "skoorinterpretation.h"

#include <QDebug>

SkoorInterpretation::SkoorInterpretation( QObject * parent, bool syntaxMode ) :
				QThread(parent), m_sci(0), m_syntaxMode(syntaxMode)
{
	m_interpreter = new gum::prm::skoor::SkoorInterpreter();
}

SkoorInterpretation::SkoorInterpretation( const QsciScintillaExtended * sci, QObject * parent, bool syntaxMode ) :
		QThread(parent), m_sci(sci), m_syntaxMode(syntaxMode)
{
	m_interpreter = new gum::prm::skoor::SkoorInterpreter();
	if ( m_sci != 0 )
		m_title = m_sci->title();
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

QString SkoorInterpretation::documentTitle() const
{
	return m_title;
}

void SkoorInterpretation::setDocument( const QString & text )
{
	QMutexLocker locker(&m_mutex);
	m_text = text.toStdString();
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
	if ( path.endsWith(QChar('/')) )
		m_paths += path + ";";
	else
		m_paths += path + "/;";
}

void SkoorInterpretation::addPaths( const QList<QString> & paths )
{
	foreach ( QString path, paths )
		addPath(path);
}

void SkoorInterpretation::run()
{
	m_interpreter->clearContext();
	m_interpreter->clearPaths();

	QMutexLocker locker(&m_mutex);
	m_interpreter->setSyntaxMode(m_syntaxMode);
	foreach ( QString s, m_paths )
		m_interpreter->addPath( s.toStdString() + "/" );

	m_interpreter->processCommandLine( m_text );
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
