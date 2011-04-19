#include "skoorinterpretation.h"

#include "qsciscintillaextended.h"
#include "prmtreemodel.h"
#include <agrum/prm/skoor/SkoorInterpreter.h>

#include <QDebug>

using namespace std;
using namespace gum::prm::skoor;

SkoorInterpretation::SkoorInterpretation( const QsciScintillaExtended * sci, QObject * parent ) :
		AbstractParser(sci, parent), m_interpreter(0), m_prmChanged(false)
{
	m_interpreter = new SkoorInterpreter();
}

SkoorInterpretation::~SkoorInterpretation()
{
	// Wait the run methods ends.
	wait();
	delete m_interpreter;
}

//! Return last command.
QString SkoorInterpretation::command() const
{
	if ( isFinished() )
		return m_command;
	else
		return QString();
}

//! \reimp
void SkoorInterpretation::parse( Priority priority )
{
	m_command.clear();
	AbstractParser::parse(priority);
}

//! Parse a single command
void SkoorInterpretation::parseCommand( const QString & command, Priority priority )
{
	if ( isRunning() )
		wait();
	m_command = command;
	AbstractParser::parse(priority);
}


void SkoorInterpretation::run()
{
	QString f = filename();
	string b = buffer().toStdString();

	m_interpreter->setSyntaxMode(isSyntaxMode());
	m_interpreter->clearPaths();
	foreach ( QString s, classPaths() )
		m_interpreter->addPath( s.toStdString() );
	m_interpreter->setSyntaxMode(isSyntaxMode());

	if ( ! m_command.isEmpty() ) {
		if ( prm().isNull() ) { // On a pas encore parsé le fichier
			if ( ! f.isEmpty() )
				m_interpreter->interpretFile(f.toStdString());
			else
				m_interpreter->interpretLine( b );
		}
		m_interpreter->interpretLine( m_command.toStdString() );
	} else if ( ! f.isEmpty() )
		m_interpreter->interpretFile(f.toStdString());
	else
		m_interpreter->interpretLine( b );

	m_prmChanged = true;
	setErrors( m_interpreter->getErrorsContainer() );
}

//! \reimp.
QSharedPointer<PRMTreeModel> SkoorInterpretation::prm()
{
	if ( isFinished() && m_prmChanged ) {
		m_prmChanged = false;
		setPRM( m_interpreter->prm() );
	}
	return AbstractParser::prm();
}

vector< pair<string,QueryResult> > SkoorInterpretation::results() const
{
	if ( isFinished() && ! isSyntaxMode() && m_interpreter )
		return m_interpreter->results();
	else
		return vector< pair<string,QueryResult> >();
}

