#include "skoorinterpretation.h"

#include <QDebug>

SkoorInterpretation::SkoorInterpretation( QObject * parent, bool syntaxMode ) :
				QThread(parent), m_syntaxMode(syntaxMode)
{
	m_interpreter = new gum::prm::skoor::SkoorInterpreter();
}

SkoorInterpretation::SkoorInterpretation( const QString & filename, QObject * parent, bool syntaxMode ) :
		QThread(parent), m_filename(filename), m_syntaxMode(syntaxMode)
{
	m_interpreter = new gum::prm::skoor::SkoorInterpreter(/*filename.toStdString()*/);
}

SkoorInterpretation::~SkoorInterpretation()
{
	// Wait the run methods ends.
	wait();
	delete m_interpreter;
}

QString SkoorInterpretation::filename() const
{
	return m_filename;
}

void SkoorInterpretation::setDocument( const QString & text )
{
	QMutexLocker locker(&m_mutex);
	m_text = text.toStdString();
}

void SkoorInterpretation::setPath( const QList<QString> & paths )
{
	QMutexLocker locker(&m_mutex);
	m_paths = paths;
}

void SkoorInterpretation::addPath( const QString & path )
{
	QMutexLocker locker(&m_mutex);
	m_paths << path;
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
