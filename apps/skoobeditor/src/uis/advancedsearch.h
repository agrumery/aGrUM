#ifndef ADVANCEDSEARCH_H
#define ADVANCEDSEARCH_H

#include <QDialog>

namespace Ui {

  class AdvancedSearch;
}

class AdvancedSearch : public QDialog {
    Q_OBJECT

  public:
    enum Filter {All, Skool, Skoor};

    explicit AdvancedSearch( QWidget *parent = 0 );
    ~AdvancedSearch();

    bool isSearch() const;
    bool isReplaceAll() const;
    bool isDocumentSearch() const;
    bool isProjectSearch() const;
    bool isOpenFilesSearch() const;
    bool isCaseSensitive() const;
    bool isRegex() const;
    bool isWholeWord() const;
    Filter filter() const;
    QString searchText() const;
    QString replaceText() const;

  public slots:
    void setDocumentSearch();
    void setProjectSearch();
    void setOpenFilesSearch();
    void setCaseSensitive( bool checked );
    void setRegex( bool checked );
    void setWholeWord( bool checked );
    void setFilter( Filter filter );
    void setSearchText( const QString & text );
    void setReplaceText( const QString & text );
    void setProjectSearchEnabled( bool enabled );

  protected slots:
    void on_cancelButton_clicked();
    void on_searchButton_clicked();
    void on_replaceAllButton_clicked();

    void on_documentRadio_toggled( bool checked );

  protected:
    void closeEvent( QCloseEvent * event );

  private:
    Ui::AdvancedSearch *ui;

    bool m_isSearch, m_isReplaceAll;
};

#endif // ADVANCEDSEARCH_H
