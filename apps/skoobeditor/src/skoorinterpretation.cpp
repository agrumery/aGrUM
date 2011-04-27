#include "skoorinterpretation.h"

#include "qsciscintillaextended.h"
#include "prmtreemodel.h"
#include <agrum/prm/skoor/SkoorInterpreter.h>

#include <QDebug>

using namespace std;
using namespace gum::prm::skoor;

struct SkoorInterpretation::PrivateData {
	QMutex *		                      mutex;
	gum::prm::skoor::SkoorInterpreter *   interpreter;
	QString                               command;
	vector< pair<string,QueryResult> >    result;
};

////////////////////////////////////////////////////////////////////////////////

//! Constructor. Create skoor interpreter.
SkoorInterpretation::SkoorInterpretation( const QsciScintillaExtended * sci, QObject * parent ) :
		AbstractParser(sci, parent), d(new PrivateData)
{
	d->interpreter = new SkoorInterpreter();
	d->mutex = new QMutex();
}

//! Destructor. Delete skoor interpreter.
SkoorInterpretation::~SkoorInterpretation()
{
	// Wait the run methods ends.
	wait();
	delete d->mutex;
	delete d->interpreter;
}

//! Return last command.
QString SkoorInterpretation::command() const
{
	QMutexLocker locker(d->mutex);
	return d->command;
}


//! Return result of execution.
vector< pair<string,QueryResult> > SkoorInterpretation::results() const
{
	QMutexLocker locker(d->mutex);
	return d->result;
}

//! \reimp
void SkoorInterpretation::parse( Priority priority )
{
	QMutexLocker locker(d->mutex);
	d->command.clear();
	locker.unlock();

	AbstractParser::parse(priority);
}

//! Parse a single command
void SkoorInterpretation::parseCommand( const QString & command, Priority priority )
{
	QMutexLocker locker(d->mutex);
	d->command = command;
	locker.unlock();

	AbstractParser::parse(priority);
}


//! \reimp
void SkoorInterpretation::run()
{
	QString f = filename();
	QString b = buffer();

	d->interpreter->setSyntaxMode(isSyntaxMode());
	d->interpreter->clearPaths();
	foreach ( QString s, classPaths() )
		d->interpreter->addPath( s.toStdString() );
	d->interpreter->setSyntaxMode(isSyntaxMode());
	//d->interpreter->setVerboseMode(true);

	QMutexLocker locker(d->mutex);
	QString command = d->command;
	locker.unlock();

	if ( ! command.isEmpty() ) {
		if ( ! d->interpreter || ! d->interpreter->prm() ) { // On a pas encore parsé le fichier
			if ( ! f.isEmpty() ) {
				d->interpreter->interpretFile( f.toStdString() );
			} else {
				d->interpreter->interpretLine( b.toStdString() );
			}
		}
		d->interpreter->interpretLine( command.toStdString() );
	} else if ( ! f.isEmpty() )
		d->interpreter->interpretFile( f.toStdString() );
	else
		d->interpreter->interpretLine( b.toStdString() );

	setErrors( d->interpreter->getErrorsContainer() );
	if ( d->interpreter->prm() == 0 )
		return;
	locker.relock();
	d->result = d->interpreter->results();
	QSharedPointer<PRMTreeModel> ptr( new PRMTreeModel(d->interpreter->prm()) );
	setPRM( ptr );
}
