#include "projectproperties.h"
#include "ui_projectproperties.h"

#include <QFileDialog>
#include <QDebug>

ProjectProperties::ProjectProperties( Project * p, QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::ProjectProperties ),
    p( p ),
    parent( parent ) {
  ui->setupUi( this );

  if ( p ) {
    ui->listWidget->addItems( p->paths() );
    ui->listWidget->item( 0 )->setFlags( Qt::NoItemFlags );
  }

  connect( ui->addButton, SIGNAL( clicked() ), this, SLOT( onAddButtonClicked() ) );

  connect( ui->delButton, SIGNAL( clicked() ), this, SLOT( onDelButtonClicked() ) );
}

ProjectProperties::~ProjectProperties() {
  delete ui;
}

void ProjectProperties::onAddButtonClicked() {
  QString dirPath = QFileDialog::getExistingDirectory( parent, tr( "Ajouter un répertoire de classes" ), QDir::homePath() );
  ui->listWidget->addItem( dirPath );
}

void ProjectProperties::onDelButtonClicked() {
  delete ui->listWidget->takeItem( ui->listWidget->currentRow() );
}

void ProjectProperties::accept() {
  if ( p ) {
    p->clearPaths();
    foreach( QListWidgetItem * item, ui->listWidget->findItems( "", Qt::MatchContains ) )
    p->addPath( item->text() );
  }

  QDialog::accept();
}


void ProjectProperties::reject() {
  ui->listWidget->clear();

  if ( p ) {
    ui->listWidget->addItems( p->paths() );
    ui->listWidget->item( 0 )->setFlags( Qt::NoItemFlags );
  }

  QDialog::reject();
}
