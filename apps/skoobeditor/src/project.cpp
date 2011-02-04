#include "project.h"

#include <QFileSystemModel>
#include <QFile>
#include <QDir>
#include <QList>
#include <QDebug>


//
struct Project::PrivateData {
	QDir dir;
	QList<QString> paths;

	QModelIndex rootIndex;

	void createProjectTree();
};

Project::Project(const QString & projDir, QObject * parent) : QFileSystemModel(parent)
{
	d = new PrivateData;
	d->dir = QDir(projDir);
	d->paths << projDir;

	// We create project files
	if ( ! d->dir.exists(d->dir.dirName()+".skoop") )
		d->createProjectTree();
	else
		;// TODO : process .skoop file

	setRootPath(d->dir.absolutePath());
	d->rootIndex = index(d->dir.absolutePath());
}

Project::~Project()
{
	delete d;
}


QString Project::name()
{
	return d->dir.dirName();
}


QString Project::dir()
{
	return d->dir.absolutePath();
}


///
bool Project::isInside( const QString & filePath ) const
{
	 QFileInfo info(filePath);
	if ( info.isFile() )
		return QDir::match( d->dir.absolutePath() + "*", info.absolutePath() );
	else
		return false;
}

void Project::addPath( const QString & path )
{
	d->paths << path;
}


QList<QString> Project::paths() const
{
	return d->paths;
}


void Project::PrivateData::createProjectTree()
{
	QString filename = dir.absolutePath() + QDir::separator() + dir.dirName() + ".skoop";

	QFile projectFile(filename);

	projectFile.open(QIODevice::WriteOnly);

	projectFile.write(tr("Hello !\n").toUtf8());

	projectFile.close();

	dir.mkdir(tr("classes"));
	dir.mkdir(tr("requests"));
	dir.mkdir(tr("systems"));
}

void Project::close()
{

}

QModelIndex Project::root() const
{
	return d->rootIndex;
}


int Project::columnCount(const QModelIndex &) const
{
	return 1;
}

QVariant Project::headerData ( int section, Qt::Orientation orientation, int role ) const
{
	if ( section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole )
		return d->dir.dirName();
	else
		return QFileSystemModel::headerData(section, orientation, role);
}
