#ifndef QKEYSEQUENCEWIDGET_H
#define QKEYSEQUENCEWIDGET_H

#include <QWidget>
#include <QKeyEvent>

class QKeySequenceWidget : public QWidget {
    Q_OBJECT

  public:
    QKeySequenceWidget( QWidget * parent = 0 );
    QKeySequenceWidget( const QKeySequence & sequence, QWidget * parent = 0 );

    virtual QSize sizeHint() const;

    virtual QKeySequence keySequence() const;
    virtual void setKeySequence( const QKeySequence & keySequence );
    virtual void setKeySequence( const QString & keySequence );

  public slots:
    void clear();
    void setFocus();

  protected slots:
    void onLineEditionFinished();

  signals:
    void shortcutChanged( QKeySequence );

  private:
    Q_DISABLE_COPY( QKeySequenceWidget )

    struct PrivateData;
    PrivateData * d;

    void initParameters();
};


#include <QLineEdit>

/**
  */

class MyLineEdit : public QLineEdit {
    Q_OBJECT
  public:
    /** */
    MyLineEdit( QWidget * parent = 0 ) : QLineEdit( parent ) {}

    /** */
    ~MyLineEdit() {}

    /** */
    virtual QKeySequence keySequence() const {
      return seq;
    }

    /** */
    virtual void setKeySequence( const QKeySequence & keySequence ) {
      seq = keySequence;
      setText( seq.toString() );
    }

  public slots:
    /** */
    virtual void clear() {
      seq = QKeySequence();
      setText( QString() );
    }

  protected:
    /** */
    virtual void keyPressEvent( QKeyEvent * event ) {
      if ( seq.count() <= 4 &&
           event->key() != Qt::Key_Control &&
           event->key() != Qt::Key_Shift &&
           event->key() != Qt::Key_Alt &&
           event->key() != Qt::Key_Meta ) {

        QKeySequence shortcut( event->modifiers() + event->key() );

        if ( seq.count() == 0 || ! selectedText().isEmpty() )
          seq = shortcut;
        else
          seq = QKeySequence( seq.toString() + "," + shortcut.toString() );

        setText( seq.toString() );
      }

    }

  private:
    Q_DISABLE_COPY( MyLineEdit )

    QKeySequence seq; ///
};

QDebug operator<<( QDebug dbg, const QKeySequenceWidget & object );

#endif // QKEYSEQUENCEWIDGET_H
