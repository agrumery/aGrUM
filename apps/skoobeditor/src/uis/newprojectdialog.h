#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>

namespace Ui {

  class NewProjectDialog;
}

class NewProjectDialog : public QDialog {
    Q_OBJECT

  public:
    explicit NewProjectDialog( QWidget *parent = 0 );
    ~NewProjectDialog();

    QString projectName();
    QString projectDir();

  protected slots:
    void on_selectDirectoryButton_clicked();
    void on_buttonBox_accepted();

  private:
    Ui::NewProjectDialog *ui;
};

#endif // NEWPROJECTDIALOG_H
