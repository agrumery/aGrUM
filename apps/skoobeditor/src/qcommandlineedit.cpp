#include "qcommandlineedit.h"

#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QDebug>

struct QCommandLineEdit::PrivateData {
  QStringList history;
  int idx;
  QCompleter * completer;
};

QCommandLineEdit::QCommandLineEdit( QWidget * parent ) : QLineEdit( parent ), d( new PrivateData ) {
  d->idx = 0;
  d->history << "";
  d->completer = 0;
  connect( this, SIGNAL( returnPressed() ), this, SLOT( onReturnPressed() ) );
}

//! \reimp
QCompleter * QCommandLineEdit::completer() const {
  return d->completer;
}

//! \reimp
void QCommandLineEdit::setCompleter( QCompleter * c ) {
  if ( d->completer )
    disconnect( d->completer, SIGNAL( activated( QString ) ),
                this, SLOT( insertCompletion( QString ) ) );

  d->completer = c;

  connect( d->completer, SIGNAL( activated( QString ) ),
           this, SLOT( insertCompletion( QString ) ) );
}

//! \reimp
//! Handle Key_Up and Key_Down for historic.
void QCommandLineEdit::keyPressEvent( QKeyEvent * event ) {
  if ( d->completer == 0 || d->completer->popup() == 0 || d->completer->popup()->isHidden() ) {
    switch ( event->key() ) {
      case Qt::Key_Up:

        if ( d->idx == d->history.size()-1 )
          d->history[d->idx] = text();

        if ( d->idx > 0 ) {
          d->idx--;
          setText( d->history[d->idx] );
        }

        return;

      case Qt::Key_Down:

        if ( d->idx == d->history.size()-1 )
          d->history[d->idx] = text();

        if ( d->idx < d->history.size()-1 ) {
          d->idx++;
          setText( d->history[d->idx] );
        }

        return;
    }
  } else if ( d->completer && d->completer->popup() && d->completer->popup()->isVisible() ) {
    // The following keys are forwarded by the completer to the widget
    switch ( event->key() ) {
      case Qt::Key_Enter:
      case Qt::Key_Return:
//    case Qt::Key_Escape:
//    case Qt::Key_Tab:
//    case Qt::Key_Backtab:
        event->ignore();
        return; // let the completer do default behavior
      case Qt::Key_Right:
        QLineEdit::keyPressEvent( event );
        autoComplete();
        return;
      case Qt::Key_Left:
        QLineEdit::keyPressEvent( event );
        autoComplete();
        return;
      default:
        break;
    }

    if ( event->modifiers() == Qt::ControlModifier )
      return;

    QLineEdit::keyPressEvent( event );

    autoComplete();

    return;
  }

  QLineEdit::keyPressEvent( event );
}

/// Autocomplete from QCompleter and its model.
void QCommandLineEdit::autoComplete() {
  if ( ! d->completer )
    return;

  d->completer->setWidget( this );

  QString lineText = text();

  int caret = cursorPosition(), start = caret - 1;

  // Search start of prefix.
  while ( start >= 0 && ( lineText[start].isLetterOrNumber() ||
                          lineText[start] == '_' ||
                          lineText[start] == '.' ||
                          lineText[start] == '[' ||
                          lineText[start] == ']' ) ) start--;

  start++;

  QString prefix = lineText.mid( start, caret - start );

  if ( prefix.size() == 0 && d->completer->popup() ) {
    d->completer->popup()->hide();
    return;
  }

  d->completer->setCompletionPrefix( prefix );

  int count = d->completer->completionCount();

  QRect cr;

  if ( count == 0 )
    return;
  else if ( count == 1 && ! d->completer->popup()->isVisible() ) {
    // Insert directly the result
    d->completer->setCurrentRow( 0 );
    insertCompletion( d->completer->currentCompletion() );
  } else {
    // Compute popup size.
    QPoint topLeft = mapToGlobal( pos() + QPoint( 0,height() ) );
    QSize size( d->completer->popup()->sizeHintForColumn( 0 ) +
                d->completer->popup()->verticalScrollBar()->sizeHint().width(),
                cursorRect().height() );
    cr = QRect( topLeft, size );
    // Select first result
    d->completer->popup()->setCurrentIndex( d->completer->completionModel()->index( 0, 0 ) );
    // Show popup
    d->completer->complete();
  }
}

/**
  */
void QCommandLineEdit::insertCompletion( const QString& completion ) {
  if ( d->completer->widget() != this )
    return;

  // select prefix
  setSelection( cursorPosition() - d->completer->completionPrefix().length(), cursorPosition() );

  // remove and insert
  cut();

  insert( completion );

  // Set index
  setCursorPosition( cursorPosition() + completion.length() );
}

//! We save the command in history.
void QCommandLineEdit::onReturnPressed() {
  if ( text().isEmpty() )
    return;

  d->history.last() = text();

  d->history << "";

  d->idx = d->history.size()-1;
}
