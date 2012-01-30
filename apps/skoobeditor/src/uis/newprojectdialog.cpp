#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

#include <QFileSystemModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

NewProjectDialog::NewProjectDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::NewProjectDialog ) {
  ui->setupUi( this );

  ui->projectDirectory->setText( QDir::homePath() );
}

NewProjectDialog::~NewProjectDialog() {
  delete ui;
}

void NewProjectDialog::on_selectDirectoryButton_clicked() {
  QString dir = QFileDialog::getExistingDirectory( this,
                tr( "Sélectionnez le répertoire du nouveau projet" ),
                QDir::homePath() );

  if ( dir.isEmpty() )
    return;

  ui->projectDirectory->setText( dir );
}

void NewProjectDialog::on_buttonBox_accepted() {
  QString name = QDir::cleanPath( ui->projectDirectory->text() + QDir::separator() + ui->projectName->text() + QDir::separator() );

  if ( name.isEmpty() )
    reject();

  QDir dir( name );

  if ( dir.exists() ) {
    int button = QMessageBox::warning( this,
                                       tr( "Nouveau projet" ),
                                       tr( "Le répertoire '%1' existe déjà.\nSi vous continuez, le dossier existant sera sauvegardé (ajout de .bak) ?" ).arg( name ),
                                       QMessageBox::Ok,
                                       QMessageBox::Cancel );

    if ( button == QMessageBox::Ok ) {
      QDir( QDir::root() ).rename( name,name+".old" );
    } else
      return;
  }

  // If can't create the directory, warn the user.
  if ( ! QDir( QDir::root() ).mkpath( name ) ) {
    QMessageBox::warning( this,tr( "Création du répertoire de projet" ),
                          tr( "Attention : le répertoire ne peut être créé.\nCela peut être dû à l'utilisation de mauvais caractères." ) );
    return;
  }

  accept();
}


QString NewProjectDialog::projectName() {
  if ( result() )
    return ui->projectName->text();
  else
    return QString();

}


QString NewProjectDialog::projectDir() {
  if ( result() )
    return QDir::cleanPath( ui->projectDirectory->text() + QDir::separator() + ui->projectName->text() + QDir::separator() );
  else
    return QString();

}
