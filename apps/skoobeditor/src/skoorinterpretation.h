#ifndef SKOORINTERPRETATION_H
#define SKOORINTERPRETATION_H

#include "abstractparser.h"

#include <vector>

namespace gum {
	namespace prm {
		namespace skoor {
			class SkoorInterpreter;
			typedef std::vector< std::pair<std::string,float> > QueryResult;
		}
	}
}

/**
 * This class read a skoor and interprete it.
 * If \a syntaxMode is true, any calcul will be done.
 * It send a finished() signal when done.
 * When done, you can retrieve the SkoorInterpreter.
 */
class SkoorInterpretation : public AbstractParser
{
	Q_OBJECT

	gum::prm::skoor::SkoorInterpreter * m_interpreter;
	mutable bool prmChanged;

public:
	SkoorInterpretation( const QsciScintillaExtended * sci, QObject * parent );
	~SkoorInterpretation();

	//! \reimp.
	virtual QSharedPointer<PRMTreeModel> prm();

	//! Return result of execution.
	std::vector< std::pair<std::string,gum::prm::skoor::QueryResult> > results() const;

protected:
	void run();
};


#endif // SKOORINTERPRETATION_H
