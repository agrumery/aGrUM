#include "project.h"

#include <QFileSystemModel>
#include <QFile>
#include <QDir>
#include <QList>
#include <QMessageBox>
#include <QDebug>


//
struct Project::PrivateData {
	QDir dir;
	QList<QString> paths;

	QModelIndex rootIndex;

	void createProjectTree();
	void readSkoopFile();
	void writeSkoopFile();
};

Project::Project(const QString & projDir, QObject * parent) : QFileSystemModel(parent)
{
	d = new PrivateData;
	d->dir = QDir(projDir);
	d->paths << projDir;

	// We create project files
	if ( ! d->dir.exists(d->dir.dirName()+".skoop") ) {
		d->createProjectTree();
	} else
		d->readSkoopFile();

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

void Project::addPaths( const QList<QString> & paths )
{
	d->paths << paths;
}

void Project::removePath( const QString & path )
{
	d->paths.removeOne( path );
}

void Project::clearPaths()
{
	d->paths.clear();
}

QList<QString> Project::paths() const
{
	return d->paths;
}


void Project::PrivateData::createProjectTree()
{
	writeSkoopFile();
	dir.mkdir(tr("classes"));
	dir.mkdir(tr("requests"));
	dir.mkdir(tr("systems"));
}

void Project::close()
{
	d->writeSkoopFile();
}


void Project::PrivateData::readSkoopFile()
{
	QString filename = dir.absolutePath() + QDir::separator() + dir.dirName()+".skoop";
	QFile file(filename);
	if ( ! file.open( QFile::ReadOnly ) ) {
		QMessageBox::warning(0, tr("Erreur non fatal"), tr("Échec lors de l'ouverture du fichier projet %1").arg(filename));
		return;
	}
	QDataStream in(&file);
	QMultiMap<QString, QString> map;
	in >> map;
	paths = map.values("paths");
}

void Project::PrivateData::writeSkoopFile()
{
	QString filename = dir.dirName()+".skoop";
	QFile file(filename);
	if ( ! file.open( QFile::WriteOnly ) ) {
		QMessageBox::warning(0, tr("Erreur non fatal"), tr("Échec lors de l'ouverture du fichier projet %1").arg(filename));
		return;
	}
	QDataStream out(&file);
	QMultiMap<QString, QString> map;
	foreach( QString s, paths )
		map.insertMulti("paths", s);
	out << map;
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
