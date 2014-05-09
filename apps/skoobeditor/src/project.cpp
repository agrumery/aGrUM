#include "project.h"

#include <QFileSystemModel>
#include <QFile>
#include <QDir>
#include <QList>
#include <QMessageBox>
#include <QMimeData>
#include <QUrl>
#include <QThread>
#include <QDebug>


//

struct Project::PrivateData {
  QDir dir;
  QList<QString> paths;
  bool editable;

  QModelIndex rootIndex;

  void createProjectTree();
  void readSkoopFile();
  void writeSkoopFile();
};


/// Constructor
Project::Project ( const QString& projDir, QObject* parent ) : QFileSystemModel ( parent ) {
  d = new PrivateData;
  d->dir = QDir ( projDir );
  d->paths << projDir;
  d->editable = false;

  // We create project files

  if ( ! d->dir.exists ( d->dir.dirName() + ".skoop" ) ) {
    d->createProjectTree();
  } else
    d->readSkoopFile();

  setNameFilters ( QStringList() << "*.skool" << "*.skoor" );

  setNameFilterDisables ( false );

  setReadOnly ( false );

  setRootPath ( d->dir.absolutePath() );

  d->rootIndex = index ( d->dir.absolutePath() );
}


/// Destructor
Project::~Project() {
  close();
  delete d;
}


/**
  Return the project name.
  */
QString Project::name() const {
  return d->dir.dirName();
}


/**
  Return the project root directory.
  */
QString Project::dir() const {
  return d->dir.absolutePath();
}


/**
  */
bool Project::isEditable() const {
  return d->editable;
}

/**
 */
void Project::setEditable ( bool editable ) {
  d->editable = editable;
}

/**
  Return true if \a filepath is in the project, false otherwise.
  */
bool Project::isInside ( const QString& filePath ) const {
  QFileInfo info ( filePath );

  if ( info.isFile() )
    return QDir::match ( d->dir.absolutePath() + "*", info.absolutePath() );
  else
    return false;
}


/**
  Return all skool and skoor files in the project.
  \note Compute list each time.
  */
QList<QString> Project::files() const {
// QList<QString> closeList;
// QList<QModelIndex> openList;
// openList << root();
// while ( ! openList.isEmpty() ) {
//  QModelIndex current = openList.takeFirst();
//  if ( hasChildren(current) ) {
//   for ( int i = 0 ; i < rowCount(current) ; i++ )
//    openList << current.child(i,0);
//  } else
//   closeList << filePath(current);
// }
// return closeList;

  QList<QString> closeList;
  QList<QDir> openList;
  openList << d->dir;

  while ( ! openList.isEmpty() ) {
    QDir current = openList.takeFirst();

    foreach ( QFileInfo info, current.entryInfoList ( QDir::AllEntries | QDir::NoDotAndDotDot ) ) {
      if ( info.isDir() )
        openList << info.absoluteFilePath();
      else
        closeList << info.absoluteFilePath();
    }
  }

  return closeList;
}


/**
  */
bool Project::rmdirRec ( const QModelIndex& parent ) {
  if ( ! isDir ( parent ) )
    return false;

  QDir dir ( filePath ( parent ) );

  bool result = true;

  foreach ( QFileInfo info, dir.entryInfoList ( QDir::AllEntries | QDir::NoDotAndDotDot ) ) {
    if ( info.isDir() )
      result &= rmdirRec ( index ( info.absoluteFilePath() ) );
    else
      result &= dir.remove ( info.absoluteFilePath() );
  }

  // If result is false, don't try to rmdir.
  return result && rmdir ( parent );
}


/**
  */
void Project::addPath ( const QString& path ) {
  d->paths << path;
}


/**
  */
void Project::addPaths ( const QList<QString>& paths ) {
  d->paths << paths;
}


/**
  */
void Project::removePath ( const QString& path ) {
  d->paths.removeOne ( path );
}


/**
  clear all paths.
  */
void Project::clearPaths() {
  d->paths.clear();
}


/**
  Return paths to search for classes or systems.
  */
QList<QString> Project::paths() const {
  return d->paths;
}


/**
  */
void Project::PrivateData::createProjectTree() {
  writeSkoopFile();
  dir.mkdir ( tr ( "classes" ) );
  dir.mkdir ( tr ( "requests" ) );
  dir.mkdir ( tr ( "systems" ) );
}


/**
  Close project.
  Save project's properties in project skoop file.
  */
void Project::close() {
  d->writeSkoopFile();
}


/**
  */
void Project::PrivateData::readSkoopFile() {
  QString filename = dir.absolutePath() + QDir::separator() + dir.dirName() + ".skoop";
  QFile file ( filename );

  if ( ! file.open ( QFile::ReadOnly ) ) {
    QMessageBox::warning ( 0, tr ( "Erreur non fatal" ), tr ( "Échec lors de l'ouverture du fichier projet %1" ).arg ( filename ) );
    return;
  }

  QDataStream in ( &file );

  QMultiMap<QString, QString> map;
  in >> map;

  // If file is corrupted

  if ( map.size() == 0 ) {
    QMessageBox::warning ( 0, tr ( "Erreur non fatal" ), tr ( "Échec lors de la récupération des information : %1 est corrompu." ).arg ( filename ) );
  } else
    paths = map.values ( "paths" );
}


/**
  */
void Project::PrivateData::writeSkoopFile() {
  QString filename = dir.absolutePath() + "/" + dir.dirName() + ".skoop";
  QFile file ( filename );

  if ( ! file.open ( QFile::WriteOnly ) ) {
    QMessageBox::warning ( 0, tr ( "Erreur non fatal" ), tr ( "Échec lors de l'ouverture du fichier projet %1" ).arg ( filename ) );
    return;
  }

  QDataStream out ( &file );

  QMultiMap<QString, QString> map;

  foreach ( QString s, paths )
    map.insertMulti ( "paths", s );

  out << map;
}


/// \reimp
QModelIndex Project::root() const {
  return d->rootIndex;
}


/// \reimp
int Project::columnCount ( const QModelIndex& ) const {
  return 1;
}


/// \reimp
QVariant Project::data ( const QModelIndex& index, int role ) const {
  if ( role == Qt::DecorationRole && index.data().toString().endsWith ( ".skool" ) ) {
    return QIcon ( ":/icons/icons/new.png" );
  } else if ( role == Qt::DecorationRole && index.data().toString().endsWith ( ".skoor" ) ) {
    return QIcon ( ":/icons/icons/gear-icon.png" );
  }

  return QFileSystemModel::data ( index, role );
}

/// \reimp
QVariant Project::headerData ( int section, Qt::Orientation orientation, int role ) const {
  if ( section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole )
    return d->dir.dirName();
  else
    return QFileSystemModel::headerData ( section, orientation, role );
}


/// \reimp
bool Project::dropMimeData ( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) {
  bool result = QFileSystemModel::dropMimeData ( data, action, row, column, parent );

  if ( result && action == Qt::MoveAction && data->hasUrls() )
    foreach ( QUrl url, data->urls() ) {
      QFileInfo info ( url.toLocalFile() );
      emit fileMoved ( info.absoluteFilePath(), filePath ( parent ) + "/" + info.fileName() );
    }

  return result;
}


/// \reimp Remove Qt::ItemIsEditable of default flags
Qt::ItemFlags Project::flags ( const QModelIndex& index ) const {
  Qt::ItemFlags flags = QFileSystemModel::flags ( index );
// if ( isDir(index) ) // A dir can't be dragged for moment.
//  flags &= ~ Qt::ItemIsDragEnabled;

  if ( ! d->editable )
    flags &= ~ Qt::ItemIsEditable;

  return flags;
}
