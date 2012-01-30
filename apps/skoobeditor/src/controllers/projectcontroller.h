#ifndef PROJECTCONTROLLER_H
#define PROJECTCONTROLLER_H

#include <QObject>
#include <QModelIndex>

#include "project.h"

class MainWindow;


class ProjectController : public QObject {
    Q_OBJECT
  public:
    explicit ProjectController( MainWindow * mw, QObject *parent = 0 );
    ~ProjectController();

    Project * currentProject() const;
    bool isOpenProject() const;

  public slots:
    void newProject();
    void openProject( QString projectpath = QString() );
    void closeProject();

    void createNewClassFile();
    void createNewRequestFile();
    void createNewSystemFile();

    /*!
     This method change a filename, a package in all file in the project.
     It change package in the file when it is moved,
     class or system name when it is renamed,
     and imports in others files in both cases.

     If \a fromFilePath is a file, it can be :
     - renamed, if only filename change in \a toFilePath;
     - moved, if only path change in \a toFilePath;
     - moved and renamed, if both path and filename change;
     - removed, if \a toFilePath is an empty string;

     If \a fromFilePath is a directory, it can be :
     - renamed, if only dirname change in \a toFilePath;
     - moved, if only path change in \a toFilePath;
     - moved and renamed, if both path and dirname change;
     - removed, if \a toFilePath is an empty string;

     This method do nothing if fromFilePath is not a valid file in the project.
    */
    void refactor( const QString & fromFilePath, const QString & toFilePath );

  protected:
    void addToRecentsProjects( const QString & projectPath );
    void removeOfRecentsProjects( const QString & projectPath );
    void saveProjectsState();

  protected slots:
    bool on_projectExplorator_clicked( QModelIndex index );
    bool on_projectExplorator_doubleClicked( QModelIndex index );
    void onProjectFileRenamed( const QString & path, const QString & oldName, const QString & newName );
    void onProjectFileMoved( const QString & oldFilePath, const QString & newPath );
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
