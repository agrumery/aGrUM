#ifndef FILECONTROLLER_H
#define FILECONTROLLER_H

#include <QObject>
#include <QHash>

#include "qsciscintillaextended.h"

class MainWindow;

/**
  The FileController class provides methods for files managment.
  All slots have an entry in the File menu.
  Public methods are for other controllers.
  This class controls the last open files too.
  */

class FileController : public QObject {
    Q_OBJECT

    Q_PROPERTY ( int numberMaxOfRecentsFiles READ numberMaxOfRecentsFiles WRITE setNumberMaxOfRecentsFiles );

  public:
    FileController ( MainWindow* mw, QObject* parent = 0 );
    ~FileController();

    /// Return all open documents.
    QList<QsciScintillaExtended*> openDocuments() const;
    /// Return the current document, the open tab, or 0 if any.
    QsciScintillaExtended* currentDocument() const;
    /// Return true if there is at less one open document, false otherwise.
    bool hasCurrentDocument() const;
    /// Return true if \a file is open, false otherwise.
    bool isOpenFile ( const QString& file ) const;
    /// Return the document corresponding to \a file, or 0 if this file is not open.
    QsciScintillaExtended* fileToDocument ( const QString& file ) const;

    /// The max number of entry in the "recent files" list.
    int numberMaxOfRecentsFiles() const;
    void setNumberMaxOfRecentsFiles ( int nb );

    ///
    QsciScintillaExtended* newDocument ( const QString& title, QsciScintillaExtended::Lexer lexer = QsciScintillaExtended::None );

  public slots:
    void newSkoolFile();
    void newSkoorFile();
    bool openFile ( const QString& file = QString() );
    bool saveFile ( QsciScintillaExtended* sci = 0, bool force = false );
    bool saveAsFile ( QsciScintillaExtended* sci = 0, QString dir = "" );
    bool saveAllFiles();
    bool closeFile ( int index = -1 );
    bool closeAllFiles();
    void updateMetadata ( QsciScintillaExtended* sci = 0 );
    bool quit();

  signals:
    void fileOpened ( const QString& filename, QsciScintillaExtended* document );
    void fileSaved ( const QString& filename, QsciScintillaExtended* document );
    void fileRenamed ( const QString& oldFilename, const QString& newFilename, QsciScintillaExtended* document );
    void fileClosed ( const QString& filename );

  protected :
    /**
      Return a hashmap of open files,
      with the filename as key,
      and a pointer on the QsciScintillaExtended widget as value.
      */
    const QHash<QString, QsciScintillaExtended*>& openFiles() const;

  protected slots:
    void addToRecentFiles ( const QString& filename );
    void removeOfRecentFiles ( const QString& filename );
    void saveFilesState();

    void onCurrentDocumentChanged ( int index = -2 );
    void onDocumentRenamed ( const QString& oldFilename, const QString& newFilename );
    void triggerInit();

  private:
    MainWindow* mw;

    struct PrivateData;
    PrivateData* d;
};

#endif // FILECONTROLLER_H
