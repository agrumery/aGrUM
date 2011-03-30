#ifndef SKOOLINTERPRETATION_H
#define SKOOLINTERPRETATION_H

#include "abstractparser.h"

namespace gum {
	namespace prm {
		namespace skool {
			class SkoolReader;
		}
	}
}

/**
 * This class read a skool file to check its syntax.
 * It send a finished() signal when done.
 * When done, you can retrieve the SkoolReader.
 */
class SkoolInterpretation : public AbstractParser
{
	Q_OBJECT

	gum::prm::skool::SkoolReader * m_reader;
	mutable bool prmChanged;

public:
	SkoolInterpretation( const QsciScintillaExtended * sci, QObject * parent );
	~SkoolInterpretation();

	//! \reimp.
	virtual QSharedPointer<PRMTreeModel> prm();

protected:
	void run();
};

#endif // SKOOLINTERPRETATION_H
