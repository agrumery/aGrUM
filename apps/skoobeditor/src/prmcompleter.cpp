#include "prmcompleter.h"

#include "prmtreemodel.h"

#include <QEvent>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QDebug>

PRMCompleter::PRMCompleter( QObject *parent ) :
    QCompleter( parent ) {
  setCompletionRole( PRMTreeModel::LocalDataRole );
}


QString PRMCompleter::pathFromIndex( const QModelIndex &index ) const {
  if ( ! model() )
    return 0;

  return model()->data( index,Qt::DisplayRole ).toString();
}

QStringList PRMCompleter::splitPath( const QString &path ) const {
  return path.split( '.' );
}
