#ifndef EDITCONTROLLER_H
#define EDITCONTROLLER_H

#include <QObject>
#include <QCompleter>

#include "uis/properties.h"

class MainWindow;

class EditController : public QObject
{
    Q_OBJECT

public:
	explicit EditController(MainWindow * mw, QObject * parent = 0);
	~EditController();

	//! Return the completer
	QCompleter * completer() const;

public slots:

	void undo();
	void redo();

	void cut();
	void copy();
	void paste();

	void remove();
	void selectAll();

	void switchComment();

	void increaseIndentation();
	void decreaseIndentation();

	void autoComplete();

	void editPreferences();

signals:
	void completerChanged();

protected slots:
	void onCurrentDocumentModelChanged();

private slots:
	void triggerInit();

private:
	MainWindow * mw;

	struct PrivateData;
	PrivateData * d;

	Properties * pr;
};

#endif // EDITCONTROLLER_H
