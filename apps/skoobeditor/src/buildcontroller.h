#ifndef BUILDCONTROLLER_H
#define BUILDCONTROLLER_H

#include <QObject>
#include <QModelIndex>
#include <QSharedPointer>

#include "qsciscintillaextended.h"
#include "prmtreemodel.h"

class MainWindow;
namespace gum {
	class ErrorsContainer;
}

class BuildController : public QObject
{
	Q_OBJECT
public:
	explicit BuildController(MainWindow * mw, QObject *parent = 0);
	~BuildController();

	bool isAutoSyntaxCheck() const;

	QSharedPointer<PRMTreeModel> currentDocumentModel();
	const QSharedPointer<PRMTreeModel> currentDocumentModel() const;

public slots:
	void setAutoSyntaxCheck( bool isAuto );

	void checkSyntax( QsciScintillaExtended * sci = 0 );
	void checkSyntax( const QString & filename );
	void execute( QsciScintillaExtended * sci = 0 );
	void execute( const QString & filename );

	void hideBuildMessage();

signals:
	void currentDocumentModelChanged();

protected slots:
	void startParsing(bool isAuto = true, bool isExecution = false);
	void onParsingFinished();
	void onExecutionFinished();

	void onCommandValided();
	void onCommandParsed();

	void onMsgDoubleClick(QModelIndex index);
	void onDocumentClosed( const QString & filename );

	void executeClass( QsciScintillaExtended * sci );
	void executeSystem( QsciScintillaExtended * sci );

private slots:
	void triggerInit();

private:
	void createNewParser();

	MainWindow * mw;

	struct PrivateData;
	PrivateData * d;
};

#endif // BUILDCONTROLLER_H
