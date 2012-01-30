#include "properties.h"

#include "mainwindow.h"
#include "ui_properties.h"
#include "ui_mainwindow.h"

#include "qsciscintillaextended.h"
#include "controllers/filecontroller.h"

#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>

struct Properties::PrivateData {
  QHash<QTreeWidgetItem*, QAction*> hash;
  QHash<QTreeWidgetItem*, QList<QTreeWidgetItem*> > conflicts;
  QTreeWidgetItem * edited;
};

/* ****************************************************************** */


Properties::Properties( MainWindow * mainw, QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::Properties ),
    mw( mainw ) {
  ui->setupUi( this );
  d = new PrivateData;
  d->edited = 0;

  // ################################################################

  ui->listIcon->addItem( tr( "Éditeur" ) );
  ui->listIcon->addItem( tr( "Raccourcis clavier" ) );
  ui->listIcon->addItem( tr( "Méta-données" ) );

  // ################################################################

  foreach( QAction * a, mw->ui->menuBar->actions() ) {
    ui->shortcutsTree->addTopLevelItem( addShortcuts( a ) );
  }

  ui->shortcutsTree->expandAll();

  // ################################################################

  QSettings settings;
  settings.beginGroup( "preferences" );

  settings.beginGroup( "editor" );
  ui->tabWidthSpinBox->setValue( settings.value( "tabWidth" ).toInt() );
  ui->indentWidthSpinBox->setValue( settings.value( "indentWidth" ).toInt() );
  ui->autoIndentCheckBox->setChecked( settings.value( "autoIndent",true ).toBool() );
  ui->indentUseTabsCheckBox->setChecked( settings.value( "indentUseTabs",true ).toBool() );
  ui->tabIndentsCheckBox->setChecked( settings.value( "tabIndents",true ).toBool() );
  ui->backspaceUnindentsCheckBox->setChecked( settings.value( "backspaceUnindents",true ).toBool() );
  settings.endGroup();

  // ################################################################

  settings.beginGroup( "metadata" );
  ui->auteurPlainText->setPlainText( settings.value( "autor" ).toString() );
  ui->licencePlainText->setPlainText( settings.value( "licence" ).toString() );
  ui->dateCheck->setChecked( settings.value( "updateDateChecked",true ).toBool() );
  settings.endGroup();

  // ################################################################

  settings.beginGroup( "shortcuts" );
  foreach( QTreeWidgetItem * item, d->hash.keys() ) {
    QString shortcut = settings.value( item->data( 3,Qt::DisplayRole ).toString() ).toString();

    if ( ! shortcut.isEmpty() ) {
      item->setData( 1, Qt::DisplayRole, shortcut );
      item->setData( 2, Qt::DisplayRole, shortcut );
      d->hash[item]->setShortcut( shortcut );
    }
  }

  settings.endGroup();
}

Properties::~Properties() {
  delete d;
  delete ui;
}

/**
  */
void Properties::on_shortcutsTree_itemSelectionChanged() {
  if ( ui->shortcutsTree->selectedItems().isEmpty() ) {
    d->edited = 0;
    return;
  }

  QTreeWidgetItem * item = ui->shortcutsTree->selectedItems().first();

  ui->keySequenceWidget->setKeySequence( item->data( 1, Qt::DisplayRole ).toString() );
}

/**
  */
void Properties::on_shortcutsTree_itemActivated( QTreeWidgetItem * item, int ) {
  ui->keySequenceWidget->setFocus();
  d->edited = item;
}

/**
  */
void Properties::on_keySequenceWidget_shortcutChanged( const QKeySequence & key ) {
  Q_ASSERT( d->edited != 0 );

  // On l'enlève de tous les items avec qui il était en conflit

  if ( d->conflicts.contains( d->edited ) ) {
    foreach( QTreeWidgetItem * item, d->conflicts[d->edited] ) {
      d->conflicts[item].removeOne( d->edited );

      if ( d->conflicts[item].isEmpty() )
        d->conflicts.remove( item );
    }

    d->conflicts.remove( d->edited );
  }

  // On change sa valeur
  d->edited->setData( 1, Qt::DisplayRole, key.toString() );

  d->hash[d->edited]->setShortcut( key );

  // Pour chaque élément de l'arbre
  foreach( QTreeWidgetItem * item, d->hash.keys() ) {
    // Vérifie que les shortcuts sont différents
    // Si ils sont pareils
    if ( item != d->edited && item->data( 1, Qt::DisplayRole ) == d->edited->data( 1, Qt::DisplayRole ) ) {
      // On les ajoute à la hash conflit
      d->conflicts[d->edited] << item;
      d->conflicts[item] << d->edited;
    }

    // Si l'item à des conflits
    if ( d->conflicts.contains( item ) )
      // on le colore en rouge
      item->setTextColor( 1, Qt::red );
    else
      // Sinon en blanc
      item->setTextColor( 1, Qt::black );
  }

  if ( d->conflicts.contains( d->edited ) )
    d->edited->setTextColor( 1, Qt::red );
}

/**
  */
QTreeWidgetItem * Properties::addShortcuts( QAction * a, const QString & parent ) {

  QString name = a->text().remove( "&" );
  QString completeName = parent + name;

  if ( a->menu() != 0 ) {
    QTreeWidgetItem * menu = new QTreeWidgetItem( QStringList() << name << "" << "" << completeName );
    menu->setFlags( menu->flags() ^ Qt::ItemIsSelectable );
    foreach( QAction * action, a->menu()->actions() ) {
      if ( ! action->isSeparator() &&
           !( action == mw->ui->actionRecentFiles ) &&
           !( action == mw->ui->actionRecentProject ) )
        menu->addChild( addShortcuts( action, completeName+"/" ) );
    }

    return menu;

  } else {
    QTreeWidgetItem * item = new QTreeWidgetItem( QStringList() << name << a->shortcut() << a->shortcut() << completeName );
    d->hash[item] = a;
    return item;
  }
}

/**
  */
void Properties::accept() {
  // If there is no exception or user agree.
  if ( ! d->conflicts.isEmpty() && QMessageBox::question( this,
       tr( "Attention" ),
       tr( "Il y a des conflits non résolus, les raccourcis ne fonctionneront pas.\nVoulez vous continuer quand même ?" ),
       QMessageBox::Cancel | QMessageBox::Ok ) != QMessageBox::Ok )
    return;

  QSettings settings;

  settings.beginGroup( "preferences" );

  //
  settings.beginGroup( "editor" );

  settings.setValue( "tabWidth", ui->tabWidthSpinBox->value() );

  settings.setValue( "indentWidth", ui->indentWidthSpinBox->value() );

  settings.setValue( "autoIndent", ui->autoIndentCheckBox->isChecked() );

  settings.setValue( "indentUseTabs", ui->indentUseTabsCheckBox->isChecked() );

  settings.setValue( "tabIndents", ui->tabIndentsCheckBox->isChecked() );

  settings.setValue( "backspaceUnindents", ui->backspaceUnindentsCheckBox->isChecked() );

  settings.endGroup();

  //
  foreach( QsciScintillaExtended * sci, mw->fc->openDocuments() ) {
    sci->setTabWidth( ui->tabWidthSpinBox->value() );
    sci->setIndentationWidth( ui->indentWidthSpinBox->value() );
    sci->setAutoIndent( ui->autoIndentCheckBox->isChecked() );
    sci->setTabIndents( ui->tabIndentsCheckBox->isChecked() );
    sci->setBackspaceUnindents( ui->backspaceUnindentsCheckBox->isChecked() );
    sci->setIndentationsUseTabs( ui->indentUseTabsCheckBox->isChecked() );
  }

  //
  settings.beginGroup( "metadata" );

  settings.setValue( "autor", ui->auteurPlainText->toPlainText() );

  settings.setValue( "licence", ui->licencePlainText->toPlainText() );

  settings.setValue( "updateDateChecked", ui->dateCheck->isChecked() );

  settings.endGroup();

  //
  settings.beginGroup( "shortcuts" );

  foreach( QTreeWidgetItem * item, d->hash.keys() ) {
    settings.setValue( item->data( 3,Qt::DisplayRole ).toString(), item->data( 1,Qt::DisplayRole ) );
    item->setData( 2, Qt::DisplayRole, item->data( 1,Qt::DisplayRole ) );
  }

  settings.endGroup();

  QDialog::accept();
}

/**
  */
void Properties::reject() {
  foreach( QTreeWidgetItem * item, d->hash.keys() )
  d->hash[item]->setShortcut( item->data( 2, Qt::DisplayRole ).toString() );

  QDialog::reject();
}
