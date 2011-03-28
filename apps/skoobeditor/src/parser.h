#ifndef PARSER_H
#define PARSER_H

#include <QThread>
#include <QMutex>

#include "qsciscintillaextended.h"

class Parser : private QThread
{
	Q_OBJECT
public:
	explicit Parser( const QsciScintillaExtended * sci, QObject *parent = 0);

	void setPaths( const QList<QString> & paths );
	void addPath( const QString & path );
	void addPaths( const QList<QString> & paths );

	bool isSyntaxMode() const;
	const QsciScintillaExtended * document() const;

	//const gum::ErrorsContainer & errors

signals:
	void parsingFinished();
	void parsingStarted();

public slots:
	void parse();
	void setSyntaxMode( bool active );

protected:
	QMutex * mutex() const;

private:
	struct PrivateData;
	PrivateData * d;

};

#endif // PARSER_H
