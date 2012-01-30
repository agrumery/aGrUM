#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include <QObject>

class MainWindow;

class ViewController : public QObject {
    Q_OBJECT
  public:
    explicit ViewController( MainWindow * mw, QObject *parent = 0 );

    bool isFullScreen() const;
    bool isDocksVisible() const;
    bool isDockVisible() const;
    bool isStatusBarVisible() const;
    bool isToolBarVisible() const;
    bool isProjectExploratorVisible() const;
    bool isSearchDockVisible() const;
    bool isBuildDockVisible() const;
    bool isExecuteDockVisible() const;
    bool isCommandWidgetVisible() const;

    bool isLineNumbersVisible() const;
    bool isBookmarksVisible() const;
    bool isFoldMargingVisible() const;
    bool isIndentationGuidesVisible() const;
    bool isUnprintableCharactersVisible() const;

  public slots:
    void setFullScreen( bool checked );
    void setDocksVisibility( bool visible );
    void setDockVisibility( bool visible );
    void setStatusBarVisibility( bool visible );
    void setToolBarVisibility( bool visible );
    void setProjectExploratorVisibility( bool visible );
    void setSearchDockVisibility( bool visible );
    void setBuildDockVisibility( bool visible );
    void setExecuteDockVisibility( bool visible );
    void setCommandWidgetVisible( bool visible );

    void setLineNumbersVisibility( bool checked );
    void setBookmarksVisibility( bool checked );
    void setFoldMargingVisibility( bool checked );
    void setIndentationGuidesVisibility( bool checked );
    void setUnprintableCharactersVisibility( bool checked );

    void increaseZoom();
    void decreaseZoom();
    void normalZoom();

  private:

    struct PrivateData;
    PrivateData * d;

    MainWindow * mw;
};

#endif // VIEWCONTROLLER_H
