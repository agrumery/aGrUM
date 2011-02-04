#ifndef BUILDCONTROLLER_H
#define BUILDCONTROLLER_H

#include <QObject>
#include <QModelIndex>

#include "qsciscintillaextended.h"

class MainWindow;

class BuildController : public QObject
{
    Q_OBJECT
public:
	explicit BuildController(MainWindow * mw, QObject *parent = 0);
	~BuildController();

	bool isAutoSyntaxCheck() const;

public slots:
	void setAutoSyntaxCheck( bool isAuto );

	void checkSyntax( QsciScintillaExtended * sci = 0 );
	void checkSyntax( const QString & filename );
	void execute( QsciScintillaExtended * sci = 0, bool checkSyntaxOnly = false );
	void execute( const QString & filename, bool checkSyntaxOnly = false );

	void hideBuildMessage();

protected slots:
	void onInterpretationFinished();
	void onSyntaxCheckFinished();
	void onMsgDoubleClick(QModelIndex index);

	void startAutoSyntaxCheckThread(int i = -2);
	void onSkoorSyntaxThreadFinished();
	void onSkoolSyntaxThreadFinished();

	void triggerInit();

private:
	MainWindow * mw;

	struct PrivateData;
	PrivateData * d;
};

#endif // BUILDCONTROLLER_H
