#include "qcommandlineedit.h"

#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QDebug>

struct QCommandLineEdit::PrivateData {
	QStringList history;
	int idx;
};

QCommandLineEdit::QCommandLineEdit(QWidget * parent) : QLineEdit(parent), d(new PrivateData)
{
	d->idx = 0;
	d->history << "";
	connect( this, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()) );
}

//! \reimp
//! To
void QCommandLineEdit::keyPressEvent( QKeyEvent * event )
{
	qDebug() << event->text();
	if ( completer() == 0 || completer()->popup() == 0 || completer()->popup()->isHidden() ) {
		switch( event->key() ) {
		case Qt::Key_Up:
			if ( d->idx == d->history.size()-1 )
				d->history[d->idx] = text();
			if ( d->idx > 0 ) {
				d->idx--;
				setText( d->history[d->idx]);
			}
			return;
		case Qt::Key_Down:
			if ( d->idx == d->history.size()-1 )
				d->history[d->idx] = text();
			if ( d->idx < d->history.size()-1 ) {
				d->idx++;
				setText( d->history[d->idx]);
			}
			return;
		}
	}
	QLineEdit::keyPressEvent(event);
}

//! We save the command in history.
void QCommandLineEdit::onReturnPressed()
{
	qDebug() << "in QCommandLineEdit::onReturnPressed()" << text() << d->idx << d->history.size();
	if ( text().isEmpty() )
		return;
	d->history.last() = text();
	d->history << "";
	d->idx = d->history.size()-1;
}
