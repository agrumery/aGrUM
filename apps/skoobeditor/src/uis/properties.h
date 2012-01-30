#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QDialog>
#include <QTreeWidgetItem>

class MainWindow;

namespace Ui {

  class Properties;
}

class Properties : public QDialog {
    Q_OBJECT

  public:
    explicit Properties( MainWindow * mw, QWidget *parent = 0 );
    ~Properties();

  protected slots:
    void on_shortcutsTree_itemSelectionChanged();
    void on_shortcutsTree_itemActivated( QTreeWidgetItem *, int );
    void on_keySequenceWidget_shortcutChanged( const QKeySequence & key );

  protected:
    void accept();
    void reject();

  private:
    Q_DISABLE_COPY( Properties );

    QTreeWidgetItem * addShortcuts( QAction * a, const QString & parent = "" );

    struct PrivateData;
    PrivateData * d;

    Ui::Properties *ui;
    MainWindow * mw;
};

#endif // PROPERTIES_H
