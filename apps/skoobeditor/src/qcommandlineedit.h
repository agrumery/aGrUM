#ifndef QCOMMANDLINEEDIT_H
#define QCOMMANDLINEEDIT_H

#include <QLineEdit>

class QCommandLineEdit : public QLineEdit {
    Q_OBJECT;

  public:
    QCommandLineEdit( QWidget * parent = 0 );

    //! \reimp
    QCompleter * completer() const;
    //! \reimp
    void setCompleter( QCompleter * c );

  public slots:
    /// Autocomplete from QCompleter and its model.
    void autoComplete();

  protected:
    virtual void keyPressEvent( QKeyEvent * event );

  protected slots:
    void insertCompletion( const QString& completion );
    void onReturnPressed();

  private:

    struct PrivateData;
    PrivateData * d;
};

#endif // QCOMMANDLINEEDIT_H
