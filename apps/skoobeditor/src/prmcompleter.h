#ifndef PRMCOMPLETER_H
#define PRMCOMPLETER_H

#include <QCompleter>

#include <QStringList>

/**
  The PRMCompleter class provides completions based on current PRM model.
  */

class PRMCompleter : public QCompleter {
    Q_OBJECT
  public:
    explicit PRMCompleter( QObject * parent = 0 );

    virtual QString pathFromIndex( const QModelIndex &index ) const;
    virtual QStringList splitPath( const QString &path ) const;

  signals:

  public slots:

};

#endif // PRMCOMPLETER_H
