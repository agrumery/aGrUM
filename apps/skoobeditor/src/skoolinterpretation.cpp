#include "skoolinterpretation.h"

#include "qsciscintillaextended.h"
#include "prmtreemodel.h"
#include <agrum/prm/skool/SkoolReader.h>

#include <QDebug>

using namespace gum::prm::skool;


//! Constructor.
SkoolInterpretation::SkoolInterpretation( const QsciScintillaExtended * sci, QObject * parent ) :
		AbstractParser(sci, parent), m_reader(0), prmChanged(false)
{
}

//! Destructor.
SkoolInterpretation::~SkoolInterpretation()
{
	// Wait the run methods ends.
	wait();
	if (m_reader)
		delete m_reader->prm();
	delete m_reader;
}

//! \reimp
void SkoolInterpretation::run()
{
	QString f = filename();
	string b = buffer().toStdString();

	if (m_reader)
		delete m_reader->prm();
	delete m_reader;

	m_reader = new SkoolReader();
	m_reader->setClassPath( classPaths().join(";").toStdString() );
	m_reader->showElegantErrorsAndWarnings();

	if ( f.isEmpty() ) {
		m_reader->readString( b );
	} else
		m_reader->readFile( f.toStdString() );

	setErrors( m_reader->getErrorsContainer() );
	QSharedPointer<PRMTreeModel> ptr( new PRMTreeModel(m_reader->prm()) );
	setPRM( ptr );
}

