#include "filecontroller.h"

#include "uis/mainwindow.h"
#include "ui_mainwindow.h"
#include "editcontroller.h"
#include "searchcontroller.h"
#include "viewcontroller.h"
#include "projectcontroller.h"
#include "buildcontroller.h"
#include "qsciscintillaextended.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QSettings>
#include <QList>
#include <QAction>
#include <QSignalMapper>
#include <QDate>
#include <QDebug>

//

struct FileController::PrivateData {
  int numberMaxOfRecentsFiles;
  QMenu* recentsFiles;
  QList<QString> recentsFilesList;
  QHash<QString, QsciScintillaExtended*> openFiles;
  QList<QsciScintillaExtended*> openDocuments;
  QSignalMapper* recentsFilesMapper;
  QString lastDir;
};


/// Constructor
FileController::FileController ( MainWindow* mw, QObject* parent ) :
  QObject ( parent ),
  mw ( mw ),
  d ( new PrivateData ) {
  d->recentsFiles = new QMenu ( mw );
  d->recentsFilesMapper = new QSignalMapper ( this );
  connect ( d->recentsFilesMapper, SIGNAL ( mapped ( QString ) ), this, SLOT ( openFile ( QString ) ) );

  // Construct "Recent files" menu
  mw->ui->actionRecentFiles->setMenu ( d->recentsFiles );

  d->lastDir = QDir::homePath();

  //
  connect ( mw->ui->actionNewSkool, SIGNAL ( triggered() ), this, SLOT ( newSkoolFile() ) );
  connect ( mw->ui->actionNewSkoor, SIGNAL ( triggered() ), this, SLOT ( newSkoorFile() ) );
  connect ( mw->ui->actionOpenFile, SIGNAL ( triggered() ), this, SLOT ( openFile() ) );
  connect ( mw->ui->actionSaveFile, SIGNAL ( triggered() ), this, SLOT ( saveFile() ) );
  connect ( mw->ui->actionSaveAsFile, SIGNAL ( triggered() ), this, SLOT ( saveAsFile() ) );
  connect ( mw->ui->actionSaveAllFiles, SIGNAL ( triggered() ), this, SLOT ( saveAllFiles() ) );
  connect ( mw->ui->actionCloseFile, SIGNAL ( triggered() ), this, SLOT ( closeFile() ) );
  connect ( mw->ui->actionCloseAllFiles, SIGNAL ( triggered() ), this, SLOT ( closeAllFiles() ) );
  connect ( mw->ui->actionUpdateMetadata, SIGNAL ( triggered() ), this, SLOT ( updateMetadata() ) );
  connect ( mw->ui->tabWidget, SIGNAL ( currentChanged ( int ) ), this, SLOT ( onCurrentDocumentChanged ( int ) ) );
  connect ( mw->ui->tabWidget, SIGNAL ( tabCloseRequested ( int ) ), this, SLOT ( closeFile ( int ) ) );

  QTimer::singleShot ( 100, this, SLOT ( triggerInit() ) );
}


/// Destructor
FileController::~FileController() {
  delete d;
}

void FileController::triggerInit() {
  connect ( mw->ui->actionNewFileProject, SIGNAL ( triggered() ), mw->pc, SLOT ( newProject() ) );

  QSettings settings;
  settings.beginGroup ( "file" );

  //
  d->numberMaxOfRecentsFiles = settings.value ( "numberMaxOfRecentsFiles", 5 ).toInt();

  // Read the last closed files in settings
  int size = settings.beginReadArray ( "recentsFiles" );

  for ( int i = 0; i < size ; i++ ) {
    settings.setArrayIndex ( i );
    addToRecentFiles ( settings.value ( "file" ).toString() );
  }

  settings.endArray();


  // Reopen files don't close last time.
  size = settings.beginReadArray ( "openTabs" );

  for ( int i = 0 ; i < size ; i++ ) {
    settings.setArrayIndex ( i );
    openFile ( settings.value ( "file" ).toString() );
  }

  settings.endArray();
}


/// Return all open documents.
QList<QsciScintillaExtended*> FileController::openDocuments() const {
  return d->openDocuments;
}


/// Return the current document, the open tab, or 0 if any.
QsciScintillaExtended* FileController::currentDocument() const {
  return qobject_cast<QsciScintillaExtended*> ( mw->ui->tabWidget->currentWidget() );
}


/// Return true if there is at less one open document, false otherwise.
bool FileController::hasCurrentDocument() const {
  return mw->ui->tabWidget->currentIndex() > -1;
}


/// Return true if \a file is open, false otherwise.
bool FileController::isOpenFile ( const QString& file ) const {
  // Handle absolute and relative file path.
  QFileInfo info ( file );

  if ( ! info.isFile() )
    return false;

  return openFiles().contains ( info.absoluteFilePath() );
}


/// Return the document corresponding to \a file, or 0 if this file is not open.
/// \a file must be an absoltue path.
QsciScintillaExtended* FileController::fileToDocument ( const QString& file ) const {
  return openFiles().value ( file, 0 );
}


/// The max number of entry in the "recent files" list.
int FileController::numberMaxOfRecentsFiles() const {
  return d->numberMaxOfRecentsFiles;
}


/// The max number of entry in the "recent files" list.
void FileController::setNumberMaxOfRecentsFiles ( int nb ) {
  for ( int i = d->numberMaxOfRecentsFiles ; i > nb && i > 0 ; i-- )
    removeOfRecentFiles ( d->recentsFilesList.last() );

  if ( nb >= 0 )
    d->numberMaxOfRecentsFiles = nb;
  else
    d->numberMaxOfRecentsFiles = 0;
}


/**
  Return a hashmap of open files,
  with the filename as key,
  and a pointer on the QsciScintillaExtended widget as value.
  */
const QHash<QString, QsciScintillaExtended*>& FileController::openFiles() const {
  return d->openFiles;
}


/**
  Add a new tab and a new empty file.
  */
void FileController::newSkoolFile() {
  // We add a tab in the tabbar,
  newDocument ( tr ( "Nouveau Skool" ), QsciScintillaExtended::Skool );

  // and give it the focus
  mw->ui->tabWidget->setCurrentIndex ( mw->ui->tabWidget->count() - 1 );
  mw->ui->tabWidget->currentWidget()->setFocus();
}


/**
  Add a new tab and a new empty file.
  */
void FileController::newSkoorFile() {
  // We add a tab in the tabbar,
  newDocument ( tr ( "Nouveau Skoor" ), QsciScintillaExtended::Skoor );

  // and give it the focus
  mw->ui->tabWidget->setCurrentIndex ( mw->ui->tabWidget->count() - 1 );
  mw->ui->tabWidget->currentWidget()->setFocus();
}

/**
  Open the file specified in argument,
  or open a file dialog.
  In the file dialog, several files may be open.
  Return false if the file can not be open, true otherwise.
  */
bool FileController::openFile ( const QString& filename ) {
  QStringList filenameList;
  bool success = true;

  // If the filename is passed in argument,
  // we open this file
  // else we open a dialog

  if ( ! filename.isEmpty() )
    filenameList.append ( filename );
  else
    filenameList = QFileDialog::getOpenFileNames ( mw, tr ( "Ouvrir le(s) fichier(s)" ), d->lastDir, tr ( "Skoob Files(*.skool *.skoor)" ) );

  if ( filenameList.isEmpty() )
    return false;

  // For each filename
  foreach ( QString s, filenameList ) {

    QFileInfo fileInfo ( s );

    if ( ! fileInfo.isFile() )
      return false;

    s = fileInfo.absoluteFilePath();

    // If the file is already open, switch to it.
    if ( openFiles().contains ( s ) ) {
      mw->ui->tabWidget->setCurrentWidget ( openFiles().value ( s ) );
      continue;
    }

    // Try to open the .bak file if there was a crash last time.
    if ( QFile::exists ( s + ".bak" ) ) {
      int choice = QMessageBox::question ( mw, QString ( "Récupération du fichier" ),
                                           tr ( "Il semble que le programme ne se soit pas terminé correctement et qu'une version plus récente du fichier '%1' existe.\nVoulez-vous essayer de la récupérer ?" ).arg ( QFileInfo ( s ).fileName() ),
                                           QMessageBox::Yes, QMessageBox::No );

      if ( choice == QMessageBox::Yes ) {
        bool result = openFile ( s + ".bak" );

        if ( result ) {
          currentDocument()->setFilename ( s );
          return result;
        }
      }
    }

    // We open the file
    QFile file ( s );

    if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) ) {
      QMessageBox::warning ( mw, QString ( "Erreur" ), tr ( "Impossible d'ouvrir %1 en lecture." ).arg ( s ) );
      success = false;

    } else  {
      // We charge the file,
      QString text = QString::fromUtf8 ( file.readAll() );
      file.close();

      QsciScintillaExtended* sci = newDocument ( fileInfo.fileName() );
      sci->setFilename ( s );
      d->openFiles.insert ( s, sci );
      sci->setText ( text );
      sci->setModified ( false );

      // If current tab is empty, open in it

      if ( hasCurrentDocument() && currentDocument()->text().isEmpty() ) {
        int index = mw->ui->tabWidget->currentIndex();
        mw->ui->tabWidget->removeTab ( index );
        mw->ui->tabWidget->insertTab ( index, sci, fileInfo.fileName() );
      }

      // and give it the focus
      mw->ui->tabWidget->setCurrentWidget ( sci );

      sci->setFocus();

      removeOfRecentFiles ( s );

      emit fileOpened ( s, sci );
    }

    // Update last dir
    d->lastDir = fileInfo.absolutePath();

    //
    if ( s.endsWith ( ".bak" ) )
      currentDocument()->setFilename ( s.left ( s.length() - 4 ) );

  }

  saveFilesState();

  return success;
}


/**
  Save the file passed in argument, or the current tab.
  If the file has never been save, go to save as.
  */
bool FileController::saveFile ( QsciScintillaExtended* sci, bool force ) {
  // If argument is 0, get the current editor
  if ( sci == 0 )
    sci = currentDocument();

  // If there is any editor open
  if ( sci == 0 )
    return false;
  else if ( ! force && ( sci->text().isEmpty() || ! sci->isModified() ) )
    return true;

  QString filename = sci->filename();

  // If file never has been save, go the save as.
  if ( filename.isEmpty() ) {
    return saveAsFile ( sci );
  } else {
    // Update metadatas ?
    QSettings settings;
    settings.beginGroup ( "preferences/metadata" );

    if ( settings.value ( "updateDateChecked", false ).toBool() )
      updateMetadata ( sci );

    // Open the file in write mode
    QFile file ( filename );

    if ( !file.open ( QIODevice::WriteOnly | QIODevice::Text ) ) {
      QMessageBox::warning ( mw, tr ( "Erreur" ), tr ( "Impossible d'ouvrir %1 en écriture." ).arg ( filename ) );
      return false;
    }

    // write inside
    file.write ( sci->text().toUtf8() );

    file.close();

    sci->setModified ( false );

    // Update last dir
    d->lastDir = QFileInfo ( filename ).dir().path();
  }

  emit fileSaved ( filename, sci );

  saveFilesState();

  return true;
}


/**
  Ask a filename for the file, and save it.
  */
bool FileController::saveAsFile ( QsciScintillaExtended* sci, QString dir ) {
  // If argument is 0, get the current editor
  if ( sci == 0 )
    sci = currentDocument();

  if ( sci == 0 )
    return false;

  if ( dir.isEmpty() && ! sci->filename().isEmpty() )
    dir = sci->filename();
  else if ( dir.isEmpty() && mw->pc->currentProject() != 0 )
    dir = mw->pc->currentProject()->dir();
  else if ( dir.isEmpty() )
    dir = d->lastDir;

  const QString skoolFilter = tr ( "Fichier de classe ou système (*.skool)" );

  const QString skoorFilter = tr ( "Fichier de requête (*.skoor)" );

  QString filters, selectedFilter;

  // Set pre-selectionned filter
  if ( sci->lexerEnum() == QsciScintillaExtended::Skoor )
    filters = skoorFilter + ";;" + skoolFilter;
  else
    filters = skoolFilter + ";;" + skoorFilter;

  QString filename = QFileDialog::getSaveFileName ( mw, tr ( "Save file as..." ),
                     dir, filters,
                     &selectedFilter );

  if ( filename.isEmpty() )
    return false;

  if ( selectedFilter == skoolFilter && ! filename.endsWith ( ".skool" ) )
    filename += ".skool";
  else if ( selectedFilter == skoorFilter && ! filename.endsWith ( ".skoor" ) )
    filename += ".skoor";

  d->openFiles.remove ( sci->filename() ); // Do nothing if sci was not already saved.

  sci->setFilename ( filename );

  d->openFiles.insert ( filename, sci );

  return saveFile ( sci, true );
}


/**
  Process saveFile on all open editor.
  Return true if all files have been saved, false otherwise.
  */
bool FileController::saveAllFiles() {
  bool success = true;

  // Save the curretn file
  QsciScintillaExtended* temp = currentDocument();

  // Change current index allow to view
  // the file if we must saveAs it.

  for ( int i = 0, size = mw->ui->tabWidget->count() ; i < size ; i++ ) {
    mw->ui->tabWidget->setCurrentIndex ( i ); // Set the document i to the current file

    if ( ! saveFile() ) // save the current file
      success = false;
  }

  // Reshow the previous current file.
  mw->ui->tabWidget->setCurrentWidget ( temp );

  return success;
}


/**
  Close the current editor.
  If the file has been modified, ask for save it.
  Add the closed file to the "recentFiles" list.
  */
bool FileController::closeFile ( int index ) {
  QsciScintillaExtended* sci;

  if ( index == -1 ) {
    sci = currentDocument();
    index = mw->ui->tabWidget->currentIndex();
  } else
    sci = qobject_cast<QsciScintillaExtended*> ( mw->ui->tabWidget->widget ( index ) );

  // if there is any open editor
  if ( sci == 0 )
    return false;

  // If current tab is empty, just close it
  if ( sci->text().isEmpty() )
    mw->ui->tabWidget->removeTab ( index );

  // If file has changed, ask for save
  if ( sci->isModified() ) {
    int answer = QMessageBox::question ( mw, tr ( "Sauvegarder le fichier ? " ),
                                         tr ( "%1 n'a pas été enregistré : voulez-vous le sauvegarder ?" ).
                                         arg ( mw->ui->tabWidget->tabText ( mw->ui->tabWidget->currentIndex() ) ),
                                         QMessageBox::Ok | QMessageBox::Discard | QMessageBox::Cancel,
                                         QMessageBox::Ok );

    switch ( answer ) {
      case QMessageBox::Ok :

        // If we can't save the file, we don't close it.

        if ( saveFile ( sci ) )
          break;

      case QMessageBox::Escape :
      case QMessageBox::Cancel :
        return false;

        break;
    }
  }

  // Add the file to the "recentFiles" list.
  if ( ! sci->filename().isEmpty() ) {
    addToRecentFiles ( sci->filename() );
    d->openFiles.remove ( sci->filename() );
  }

  d->openDocuments.removeOne ( sci );

  mw->ui->tabWidget->removeTab ( index );


  if ( mw->ui->tabWidget->count() == 0 ) {
    mw->ui->menuEdit->setEnabled ( false );
    mw->ui->menuSearch->setEnabled ( false );
  } else if ( index == 0 )
    mw->ui->tabWidget->widget ( index )->setFocus();
  else
    mw->ui->tabWidget->widget ( index - 1 )->setFocus();

  emit fileClosed ( sci->filename() );

  sci->deleteLater();

  saveFilesState();

  return true;
}


/**
  Process closeFile successively for each tab.
  Return true if all files have been closed, false otherwise.
  */
bool FileController::closeAllFiles() {
  bool success = true;

  for ( int i = 0, size = mw->ui->tabWidget->count() ; i < size ; i++ ) {
    mw->ui->tabWidget->setCurrentIndex ( i ); // Set the document i to the current file

    if ( ! closeFile() ) // Close the current file
      success = false;
  }

  return success;
}


/**
  Include comments at the beginning of the file,
  with licence info, autor info, and project info.
  */
void FileController::updateMetadata ( QsciScintillaExtended* sci ) {
  // If argument is 0, get the current editor
  if ( sci == 0 )
    sci = currentDocument();

  // If there is any editor open
  if ( sci == 0 || sci->text().isEmpty() )
    return;

  // If there is already metadatas
  // erase them.
  if ( sci->text ( 0 ).startsWith ( "/// *****" ) ) {
    sci->beginUndoAction();

    while ( sci->text ( 0 ).startsWith ( "/// " ) ) {
      sci->setSelection ( 0, 0, 1, 0 );
      sci->removeSelectedText(); // Remove the line
    }

    sci->setSelection ( 0, 0, 1, 0 );

    sci->removeSelectedText(); // Remove the blanc line
    sci->endUndoAction();
  }

  QSettings settings;

  settings.beginGroup ( "preferences/metadata" );

  QString metadata ( "\n" );

  if ( mw->pc->isOpenProject() ) {
    metadata += "THIS FILE IS PART OF '" + mw->pc->currentProject()->name() + "' PROJECT.\n";
    metadata += "\n";
  }

  if ( ! settings.value ( "autor", "" ).toString().isEmpty() ) {
    metadata += "      AUTOR(S) :\n";
    metadata += settings.value ( "autor" ).toString() + "\n";
    metadata += "\n";
  }

  if ( ! settings.value ( "licence", "" ).toString().isEmpty() ) {
    metadata += "      LICENCE :\n";
    metadata += settings.value ( "licence" ).toString();
    metadata += "\n\n";
  }

  metadata += "      LAST MODIFICATION : ";

  metadata += QDate::currentDate().toString ( Qt::DefaultLocaleLongDate );
  metadata += "\n";

  // Add commentline.
  QString comment;
  comment += "/// ****************************************************************************\n";

  foreach ( QString s, metadata.split ( "\n" ) )
    comment += "///   " + s + "\n";

  comment += "/// ****************************************************************************\n\n";

  // Add metadatas to document.
  sci->insertAt ( comment, 0, 0 );
}


/**
  Process saveAllFiles and, if success,
  close the application.
  Remove also all temp file from building.
  */
bool FileController::quit() {
  // Ask to save each files modified
  int tabCount = mw->ui->tabWidget->count();
  QList<QsciScintillaExtended*> toSave;  // Document to save

  for ( int i = 0 ; i < tabCount ; i++ ) {
    mw->ui->tabWidget->setCurrentIndex ( i );
    QsciScintillaExtended* sci = currentDocument();

    if ( sci->isModified() ) {
      int answer = QMessageBox::question ( mw, tr ( "Sauvegarder le fichier ? " ),
                                           tr ( "%1 n'a pas été enregistré : voulez-vous le sauvegarder ?" ).
                                           arg ( mw->ui->tabWidget->tabText ( mw->ui->tabWidget->currentIndex() ) ),
                                           QMessageBox::Ok | QMessageBox::Discard | QMessageBox::Cancel,
                                           QMessageBox::Ok );

      if ( answer == QMessageBox::Cancel ) {
        return false;
      } else if ( answer == QMessageBox::Ok ) {
        toSave.append ( sci );

        // If the file has not filename, ask for it

        if ( sci->filename().isEmpty() ) {
          QString filename = QFileDialog::getSaveFileName ( mw, tr ( "Save file as..." ), QDir::homePath() );

          if ( filename.isEmpty() ) {
            return false;
          } else {
            sci->setFilename ( filename );
            d->openFiles.insert ( filename, sci );
            // We update the tab title.
            mw->ui->tabWidget->setTabText ( mw->ui->tabWidget->indexOf ( sci ), QFileInfo ( filename ).fileName() );
          }
        }
      }
    }
  }

  // Save all files that must be saved
  // Return on first failure
  foreach ( QsciScintillaExtended * sci, toSave )
    if ( ! saveFile ( sci ) )
      return false;

  // Emit signal fileClosed to others controllers proceed to cleaning
  // if necessary (build controller clean temp file for exemple).
  foreach ( const QString & s, d->openFiles.keys() )
    emit fileClosed ( s );

  return true;
}


/**
  */
void FileController::onCurrentDocumentChanged ( int index ) {
  QsciScintillaExtended* sci = 0;

  // if index == -2, signal was emit by a QsciScintilla

  if ( index == -2 ) {
    sci = qobject_cast<QsciScintillaExtended*> ( sender() );
    index = mw->ui->tabWidget->indexOf ( sci );
    // else, by QTabWidget
  } else
    sci = qobject_cast<QsciScintillaExtended*> ( mw->ui->tabWidget->widget ( index ) );

  if ( sci != 0 ) {
    mw->setWindowFilePath ( sci->title() );
    mw->setWindowModified ( sci->isModified() );
    mw->ui->tabWidget->setTabText ( index, sci->title() + ( sci->isModified() ? "*" : "" ) );
    mw->ui->actionBuild->setEnabled ( true );
    mw->ui->actionExecute->setEnabled ( true );
    mw->vc->setCommandWidgetVisible ( sci->lexerEnum() == QsciScintillaExtended::Skoor );

  } else {
    mw->setWindowFilePath ( QString() );
    mw->setWindowModified ( false );
    mw->ui->actionExecute->setEnabled ( false );
    mw->ui->actionBuild->setEnabled ( false );
    mw->vc->setCommandWidgetVisible ( false );
  }
}


/**
 When document are rename :
 - if filename change, change all old filename reference to the new one (filename often represent class/interface/system name);
 - if path change, change package.
 TODO : change these references in all files.
  */
void FileController::onDocumentRenamed ( const QString& oldFilename, const QString& newFilename ) {
  QsciScintillaExtended* sci = qobject_cast<QsciScintillaExtended*> ( sender() );

  if ( sci == 0 || oldFilename.isEmpty() )
    return;

  onCurrentDocumentChanged ( mw->ui->tabWidget->indexOf ( sci ) );

  // We update openFiles
  d->openFiles.remove ( oldFilename );

  d->openFiles.insert ( newFilename, sci );

  emit fileRenamed ( oldFilename, newFilename, sci );
}


/**
  Add the file the "recentFiles" list.
  */
void FileController::addToRecentFiles ( const QString& filename ) {
  if ( filename.isEmpty() || d->recentsFilesList.contains ( filename ) )
    return;

  d->recentsFilesList.append ( filename );

  QAction* action = new QAction ( QFileInfo ( filename ).fileName(), mw );

  action->setData ( filename );

  connect ( action, SIGNAL ( triggered() ), d->recentsFilesMapper, SLOT ( map() ) );

  d->recentsFilesMapper->setMapping ( action, filename );

  if ( d->recentsFiles->actions().isEmpty() )
    d->recentsFiles->addAction ( action );
  else
    d->recentsFiles->insertAction ( d->recentsFiles->actions().first(), action );

  //
  if ( d->recentsFiles->actions().size() > d->numberMaxOfRecentsFiles )
    d->recentsFiles->removeAction ( d->recentsFiles->actions().last() );
}


/**
  Remove the file of the "recentFiles" list.
  */
void FileController::removeOfRecentFiles ( const QString& filename ) {
  if ( filename.isEmpty() || ! d->recentsFilesList.contains ( filename ) )
    return;

  d->recentsFilesList.removeOne ( filename );

  QAction* a = qobject_cast<QAction*> ( d->recentsFilesMapper->mapping ( filename ) );

  if ( a != 0 ) {
    d->recentsFilesMapper->removeMappings ( a );
    d->recentsFiles->removeAction ( a );
  }

}


/**
  Save files open and closed in settings.
  */
void FileController::saveFilesState() {
  // Save the open files in settings
  QSettings settings;
  settings.beginGroup ( "file" );

  settings.beginWriteArray ( "openTabs" );
  int tabCount = mw->ui->tabWidget->count();

  for ( int i = 0, j = 0 ; i < tabCount ; i++ ) {
    QsciScintillaExtended* sci = qobject_cast<QsciScintillaExtended*> ( mw->ui->tabWidget->widget ( i ) );

    if ( ! sci->filename().isEmpty() ) {
      settings.setArrayIndex ( j++ );
      settings.setValue ( "file", sci->filename() );
    }
  }

  settings.endArray();

  // Save the last closed files in settings
  int size = d->recentsFiles->actions().size();
  settings.beginWriteArray ( "recentsFiles", size );

  for ( int i = 0 ; i < size ; i++ ) {
    settings.setArrayIndex ( i );
    settings.setValue ( "file", d->recentsFiles->actions().at ( i )->data() );
  }

  settings.endArray();
}


/**
  Create a new empty document, with all options, at end.
  */
QsciScintillaExtended* FileController::newDocument ( const QString& title, QsciScintillaExtended::Lexer lexer ) {
  // We add a tab in the tabbar,
  QsciScintillaExtended* sci =  new QsciScintillaExtended ( lexer, mw->ui->tabWidget );
  sci->setTitle ( title );
  mw->ui->tabWidget->addTab ( sci, title );
  connect ( sci, SIGNAL ( modificationChanged ( bool ) ), this, SLOT ( onCurrentDocumentChanged() ) );
  connect ( sci, SIGNAL ( filenameChanged ( QString, QString ) ), this, SLOT ( onDocumentRenamed ( QString, QString ) ) );

  // Restore editable menu
  mw->ui->menuEdit->setEnabled ( true );
  mw->ui->menuSearch->setEnabled ( true );

  if ( ! mw->vc->isLineNumbersVisible() )
    sci->setMarginWidth ( 0, 0 );

  if ( ! mw->vc->isBookmarksVisible() )
    sci->setMarginWidth ( 1, 0 );

  if ( ! mw->vc->isFoldMargingVisible() )
    sci->setFolding ( QsciScintillaExtended::NoFoldStyle );

  if ( ! mw->vc->isIndentationGuidesVisible() )
    sci->setIndentationGuides ( false );

  if ( mw->vc->isUnprintableCharactersVisible() ) {
    sci->setWhitespaceVisibility ( ( QsciScintillaExtended::WhitespaceVisibility ) true );
    sci->setEolVisibility ( true );
  }

  //
  QSettings settings;

  sci->setTabWidth ( settings.value ( "preferences/editor/tabwidth" ).toInt() );

  sci->setIndentationWidth ( settings.value ( "preferences/editor/indentWidth" ).toInt() );

  sci->setAutoIndent ( settings.value ( "preferences/editor/autoIndent" ).toBool() );

  sci->setTabIndents ( settings.value ( "preferences/editor/tabIndents" ).toBool() );

  sci->setBackspaceUnindents ( settings.value ( "preferences/editor/backspaceUnindents" ).toBool() );

  sci->setIndentationsUseTabs ( settings.value ( "preferences/editor/indentUseTabs" ).toBool() );

  d->openDocuments.append ( sci );

  return sci;
}
