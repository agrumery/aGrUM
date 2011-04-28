#ifndef QCOMMANDLINEEDIT_H
#define QCOMMANDLINEEDIT_H

#include <QLineEdit>

class QCommandLineEdit : public QLineEdit
{
	Q_OBJECT;

public:
	QCommandLineEdit(QWidget * parent = 0);

protected:
	virtual void keyPressEvent( QKeyEvent * event );

protected slots:
	void onReturnPressed();

private:
	struct PrivateData;
	PrivateData * d;
};

#endif // QCOMMANDLINEEDIT_H
