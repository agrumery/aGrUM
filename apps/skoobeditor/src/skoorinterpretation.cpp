#include "skoorinterpretation.h"

#include "qsciscintillaextended.h"
#include "prmtreemodel.h"
#include <agrum/prm/skoor/SkoorInterpreter.h>

#include <QDebug>

using namespace std;
using namespace gum::prm::skoor;

SkoorInterpretation::SkoorInterpretation( const QsciScintillaExtended * sci, QObject * parent ) :
		AbstractParser(sci, parent), m_interpreter(0), prmChanged(false)
{
}

SkoorInterpretation::~SkoorInterpretation()
{
	// Wait the run methods ends.
	wait();
	delete m_interpreter;
}

void SkoorInterpretation::run()
{
	QString f = filename();
	string b = buffer().toStdString();

	delete m_interpreter;

	if ( f.isEmpty() ) {
		m_interpreter = new SkoorInterpreter();
		m_interpreter->processCommandLine( b );
	} else
		m_interpreter = new SkoorInterpreter(f.toStdString());

	m_interpreter->setSyntaxMode(isSyntaxMode());
	foreach ( QString s, classPaths() )
		m_interpreter->addPath( s.toStdString() );

	m_interpreter->interpret();

	setErrors( m_interpreter->getErrorsContainer() );
	prmChanged = true;
}

//! \reimp.
QSharedPointer<PRMTreeModel> SkoorInterpretation::prm()
{
	if ( isFinished() && prmChanged ) {
		prmChanged = false;
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

