#include "skoorinterpretation2.h"

#include "qsciscintillaextended.h"
#include "prmtreemodel.h"
#include <agrum/prm/skoor/SkoorInterpreter.h>

#include <QDebug>

using namespace std;
using namespace gum::prm::skoor;

SkoorInterpretation2::SkoorInterpretation2( const QsciScintillaExtended * sci, QObject *parent) :
	SkoorInterpretation(sci,parent)
{
}


//! \reimp
void SkoorInterpretation2::parse( Priority priority )
{
	m_command.clear();
	SkoorInterpretation::parse(priority);
}

//! Parse a single command
void SkoorInterpretation2::parseCommand( const QString & command, Priority priority )
{
	if ( isRunning() )
		wait();
	m_command = command;
	SkoorInterpretation::parse(priority);
}

//! Return last command.
QString SkoorInterpretation2::command() const
{
	if ( isFinished() )
		return m_command;
	else
		return QString();
}

void SkoorInterpretation2::run()
{
	if( m_command.isEmpty() )
		return SkoorInterpretation::run();

	if ( ! m_interpreter )
		SkoorInterpretation::run();

	//m_interpreter->setSyntaxMode(true);
	//m_interpreter->setVerboseMode(true);
	m_interpreter->interpretLine( m_command.toStdString() );
	//m_interpreter->processCommandLine( m_command.toStdString() );
	//m_interpreter->interpret();
	qDebug() << "m_interpreter->getErrorsContainer().count() = " << m_interpreter->getErrorsContainer().count();

	setErrors( m_interpreter->getErrorsContainer() );
}
