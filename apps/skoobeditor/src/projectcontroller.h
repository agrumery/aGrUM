#ifndef PROJECTCONTROLLER_H
#define PROJECTCONTROLLER_H

#include <QObject>
#include <QModelIndex>

#include "project.h"

class MainWindow;


class ProjectController : public QObject
{
	Q_OBJECT
public:
	explicit ProjectController(MainWindow * mw, QObject *parent = 0);
	~ProjectController();

	Project * currentProject() const;
	bool isOpenProject() const;

public slots:
	void newProject();
	void openProject(QString projectpath = QString());
	void closeProject();

	void createNewClassFile();
	void createNewRequestFile();
	void createNewSystemFile();

protected:
	void addToRecentsProjects( const QString & projectPath );
	void removeOfRecentsProjects( const QString & projectPath );
	void saveProjectsState();

protected slots:
	bool on_projectExplorator_clicked( QModelIndex index );
	bool on_projectExplorator_doubleClicked( QModelIndex index );
	void onFileRenamed( const QString & path, const QString & oldName, const QString & newName );
	void onFileMoved( const QString & oldFilePath, const QString & newPath );
	void onCustomContextMenuRequested( const QPoint & pos );
	void onItemRenameFinished();

	void triggerInit();

private:
	MainWindow * mw;
	Project * currentProj;

	struct PrivateData;
	PrivateData * d;
};

#endif // PROJECTCONTROLLER_H
