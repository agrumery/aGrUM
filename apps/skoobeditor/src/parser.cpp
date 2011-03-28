#include "parser.h"


struct Parser::PrivateData {
	const QsciScintillaExtended * sci;
	QString			title;
	QList<QString>	paths;
	QMutex *		mutex;
	bool			syntaxMode;
};

Parser::Parser(const QsciScintillaExtended * sci, QObject *parent) :
	QThread(parent), d(new PrivateData)
{
	d->sci = sci;
	if ( sci )
		d->title = sci->title();
	d->mutex = new QMutex();
}

void Parser::parse()
{

}

void Parser::setPaths( const QList<QString> & paths )
{
	QMutexLocker locker(mutex());
	d->paths.clear();
	locker.unlock();
	addPaths(paths);
}

void Parser::addPath( const QString & path )
{
	QMutexLocker locker(mutex());
	if ( path.endsWith("/") )
		d->paths += path;
	else
		d->paths += path + "/";
}

void Parser::addPaths( const QList<QString> & paths )
{
	foreach ( QString path, paths )
		addPath(path);
}

QMutex * Parser::mutex() const
{
	return d->mutex;
}

bool Parser::isSyntaxMode() const
{
	QMutexLocker locker(mutex());
	return d->syntaxMode;
}

void Parser::setSyntaxMode( bool active )
{
	QMutexLocker locker(mutex());
	d->syntaxMode = active;
}

const QsciScintillaExtended * Parser::document() const
{
	QMutexLocker locker(mutex());
	return d->sci;
}
