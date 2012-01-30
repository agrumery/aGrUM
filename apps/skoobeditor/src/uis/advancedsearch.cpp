#include "advancedsearch.h"
#include "ui_advancedsearch.h"

AdvancedSearch::AdvancedSearch( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::AdvancedSearch ) {
  ui->setupUi( this );

  m_isSearch = false;
  m_isReplaceAll = false;
}

AdvancedSearch::~AdvancedSearch() {
  delete ui;
}


bool AdvancedSearch::isSearch() const { return m_isSearch; }

bool AdvancedSearch::isReplaceAll() const { return m_isReplaceAll; }

bool AdvancedSearch::isDocumentSearch() const { return ui->documentRadio->isChecked(); }

bool AdvancedSearch::isProjectSearch() const { return ui->projectRadio->isChecked(); }

bool AdvancedSearch::isOpenFilesSearch() const { return ui->openFilesRadio->isChecked(); }

bool AdvancedSearch::isCaseSensitive() const { return ui->caseBox->isChecked(); }

bool AdvancedSearch::isRegex() const { return ui->regexBox->isChecked(); }

bool AdvancedSearch::isWholeWord() const { return ui->wordBox->isChecked(); }

AdvancedSearch::Filter AdvancedSearch::filter() const  { return ( Filter ) ui->filterCombo->currentIndex(); }

QString AdvancedSearch::searchText() const { return ui->searchEdit->text(); }

QString AdvancedSearch::replaceText() const { return ui->replaceEdit->text(); }

void AdvancedSearch::setDocumentSearch() { ui->documentRadio->click(); }

void AdvancedSearch::setProjectSearch() { ui->projectRadio->click(); }

void AdvancedSearch::setOpenFilesSearch() { ui->openFilesRadio->click(); }

void AdvancedSearch::setCaseSensitive( bool checked ) { ui->caseBox->setChecked( checked ); }

void AdvancedSearch::setRegex( bool checked ) { ui->regexBox->setChecked( checked ); }

void AdvancedSearch::setWholeWord( bool checked ) { ui->wordBox->setChecked( checked ); }

void AdvancedSearch::setFilter( Filter filter ) { ui->filterCombo->setCurrentIndex(( int ) filter ); }

void AdvancedSearch::setSearchText( const QString & text ) { ui->searchEdit->setText( text ); }

void AdvancedSearch::setReplaceText( const QString & text ) { ui->replaceEdit->setText( text ); }

void AdvancedSearch::setProjectSearchEnabled( bool enabled ) {
  ui->projectRadio->setEnabled( enabled );

  if ( !enabled )
    ui->documentRadio->setChecked( true );
}

void AdvancedSearch::on_cancelButton_clicked() {
  m_isSearch = false;
  m_isReplaceAll = false;
  reject();
}

void AdvancedSearch::on_searchButton_clicked() {
  m_isSearch = true;
  m_isReplaceAll = false;
  accept();
}

void AdvancedSearch::on_replaceAllButton_clicked() {
  m_isSearch = false;
  m_isReplaceAll = true;
  accept();
}

void AdvancedSearch::on_documentRadio_toggled( bool checked ) {
  ui->filterCombo->setEnabled( !checked );
}

void AdvancedSearch::closeEvent( QCloseEvent * ) {
  m_isSearch = false;
  m_isReplaceAll = false;
}
