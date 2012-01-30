#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QCloseEvent>

namespace Ui {

  class MainWindow;
}

class FileController;

class EditController;

class SearchController;

class ViewController;

class ProjectController;

class BuildController;

class Properties;

/**
  The program main window.
  This class include and load the ui file, which describes all widgets and actions.
  Actions are handle in controllers, that are separated in different files.

  This class only handles the quit action.
  */

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow( QWidget * parent = 0 );
    ~MainWindow();

  protected:
    void closeEvent( QCloseEvent * event );

  protected slots:
    void showHelp();
    void showAboutDialog();

  private:
    Ui::MainWindow * ui;

    FileController * fc;
    EditController * ec;
    SearchController * sc;
    ViewController * vc;
    ProjectController * pc;
    BuildController * bc;

    struct PrivateData;
    PrivateData * d;

    friend class FileController;

    friend class EditController;

    friend class SearchController;

    friend class ViewController;

    friend class ProjectController;

    friend class BuildController;

    friend class Properties;
};

#endif // MAINWINDOW_H
