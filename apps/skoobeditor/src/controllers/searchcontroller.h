#ifndef SEARCHCONTROLLER_H
#define SEARCHCONTROLLER_H

#include <QObject>
#include <QTreeWidgetItem>

class MainWindow;

class QsciScintillaExtended;

class SearchController : public QObject {
    Q_OBJECT


  public:
    explicit SearchController( MainWindow * mw, QObject *parent = 0 );

  public slots:
    void quickSearch();
    void advanceSearch();
    void replace();
    bool next( QsciScintillaExtended * sci = 0 );
    bool previous( QsciScintillaExtended * sci = 0 );
    bool replaceAndNext( QsciScintillaExtended * sci = 0 );
    bool replaceAndPrevious( QsciScintillaExtended * sci = 0 );
    void replaceAll( QsciScintillaExtended * sci = 0 );

    void switchMarker();
    void findNextMarker();
    void findPreviousMarker();

    void hideQuickWidgets();

  private slots:
    void onQuickSearchEditTextChanged( QString );
    void onQuickReplaceEditTextChanged( QString );
    void onResultSearchDoubleClick( QTreeWidgetItem * item, int column );

    void onSearchOptionButtonClicked();
    void onCaseSensitiveOptionChecked( bool checked );
    void onWholeWordOptionChecked( bool checked );
    void onUseRegexOptionChecked( bool checked );

  private:
    void parseFiles( const QList<QString> & filePaths, bool isReplaceAll );
    void parseDocuments( const QList<QsciScintillaExtended *> & documents, bool isReplaceAll );

    MainWindow * mw; // Main Window

    struct PrivateData;
    PrivateData * d;
};

#endif // SEARCHCONTROLLER_H
