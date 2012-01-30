#ifndef PROJECTPROPERTIES_H
#define PROJECTPROPERTIES_H

#include <QDialog>

#include "project.h"

namespace Ui {

  class ProjectProperties;
}

class ProjectProperties : public QDialog {
    Q_OBJECT

  public:
    explicit ProjectProperties( Project * p, QWidget *parent = 0 );
    ~ProjectProperties();

  protected slots:
    virtual void onAddButtonClicked();
    virtual void onDelButtonClicked();

    virtual void accept();
    virtual void reject();

  private:
    Ui::ProjectProperties *ui;
    Project * p;
    QWidget * parent;
};

#endif // PROJECTPROPERTIES_H
