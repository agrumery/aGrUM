#ifndef SKOORINTERPRETATION2_H
#define SKOORINTERPRETATION2_H

#include "skoorinterpretation.h"

class SkoorInterpretation2 : public SkoorInterpretation
{
	Q_OBJECT


	QString m_command;

public:
	SkoorInterpretation2( const QsciScintillaExtended * sci, QObject * parent );

	//! Return last command.
	QString command() const;

public slots:
	//! \reimp
	virtual void parse( Priority priority = InheritPriority );
	//! Parse a single command
	virtual void parseCommand( const QString & command, Priority priority = InheritPriority );

protected:
	virtual void run();
};

#endif // SKOORINTERPRETATION2_H
