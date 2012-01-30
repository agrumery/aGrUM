#include "abstractparser.h"

#include "qsciscintillaextended.h"
#include "prmtreemodel.h"

#include <agrum/core/errorsContainer.h>
#include <agrum/prm/PRM.h>

#include <QFile>
#include <QFileInfo>
#include <QApplication>
#include <QDebug>

struct AbstractParser::PrivateData {
  const QsciScintillaExtended * sci;
  QStringList paths;
  QMutex *  mutex;
  bool   syntaxMode;
  QString filename;
  QString buffer;
  bool isDocModified;
  gum::ErrorsContainer errors;
  QSharedPointer<PRMTreeModel> prm;
};

////////////////////////////////////////////////////////////////////////////////

//! Constructor
AbstractParser::AbstractParser( const QsciScintillaExtended * sci, QObject *parent ) :
    QThread( parent ), d( new PrivateData ) {
  d->sci = sci;
  d->syntaxMode = true;
  d->mutex = new QMutex();
}

//! Destructor
AbstractParser::~AbstractParser() {
  wait();
  delete d->mutex;
  delete d;
}

////////////////////////////////////////////////////////////////////////////////

//! Update filename and code source to parse.
void AbstractParser::parse( Priority priority ) {
  if ( d->sci == 0 || isRunning() )
    return;

  setFilename( d->sci->filename() );

  d->isDocModified = d->sci->isModified();

  // If file is not saved in a file
  if ( filename().isEmpty() ) {
    // Parse code
    setBuffer( d->sci->text() );

    // If file is saved in a file and has been modified
  } else if ( d->isDocModified ) {
    // Save modifications in a temporary file
    setFilename( filename() + ".bak" );
    QFile file( filename() );

    if ( file.open( QFile::WriteOnly ) ) {
      file.write( d->sci->text().toUtf8() );
      file.close();
      // If modifications can't be saved, parse code
    } else {
      setBuffer( d->sci->text() );
      setFilename( QString() );
      addClassPath( QFileInfo( d->sci->filename() ).path() );
    }
  }

  start( priority );
}

//!
QStringList AbstractParser::classPaths() const {
  QMutexLocker locker( d->mutex );
  return d->paths;
}

//! Set the list of class path to \a paths. Clear the previous path if any.
void AbstractParser::setClassPaths( const QStringList & paths ) {
  QMutexLocker locker( d->mutex );
  d->paths.clear();
  locker.unlock();
  addClassPaths( paths );
}

//! Add a class path to the existant list.
void AbstractParser::addClassPath( const QString & path ) {
  QMutexLocker locker( d->mutex );

  if ( path.endsWith( "/" ) )
    d->paths += path;
  else
    d->paths += path + "/";
}

//! Add a list of class path to the existant.
void AbstractParser::addClassPaths( const QStringList & paths ) {
  foreach( QString path, paths )
  addClassPath( path );
}

//! Return true if syntax mode is activated.
bool AbstractParser::isSyntaxMode() const {
  QMutexLocker locker( d->mutex );
  return d->syntaxMode;
}

//! Set syntax mode to true if \a active is true. Default is true.
void AbstractParser::setSyntaxMode( bool active ) {
  QMutexLocker locker( d->mutex );
  d->syntaxMode = active;
}

//! Return document this parser parse.
const QsciScintillaExtended * AbstractParser::document() const {
  QMutexLocker locker( d->mutex );
  return d->sci;
}

//! Retrieve parsing errors and warnings.
gum::ErrorsContainer AbstractParser::errors() const {
  QMutexLocker locker( d->mutex );
  return d->errors;
}

//! Retrieve prm model.
QSharedPointer<PRMTreeModel> AbstractParser::prm() const {
  QMutexLocker locker( d->mutex );
  return d->prm;
}

////////////////////////////////////////////////////////////////////////////////

//! Return the filename to parse. If empty, look buffer().
QString AbstractParser::filename() const {
  QMutexLocker( d->mutex );
  return d->filename;
}

//! Set filename thread safe
void AbstractParser::setFilename( const QString & filename ) {
  QMutexLocker( d->mutex );
  d->filename = filename;
}

//! Return the buffer to parse. If empty, look filename().
QString AbstractParser::buffer() const {
  QMutexLocker( d->mutex );
  return d->buffer;

}


//! Set buffer thread safe
void AbstractParser::setBuffer( const QString & buffer ) {
  QMutexLocker( d->mutex );
  d->buffer = buffer;
}

void AbstractParser::setPRM( const QSharedPointer<PRMTreeModel> & prm ) {
  QMutexLocker( d->mutex );
  d->prm.clear();
  d->prm = prm;
  d->prm->moveToThread( QApplication::instance()->thread() );
}

//! Set the ErrorsContainer
void AbstractParser::setErrors( const gum::ErrorsContainer & errors ) {
  QMutexLocker( d->mutex );
  d->errors = errors;
}
