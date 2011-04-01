#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "filecontroller.h"
#include "editcontroller.h"
#include "searchcontroller.h"
#include "viewcontroller.h"
#include "projectcontroller.h"
#include "buildcontroller.h"
#include "qsciscintillaextended.h"

#include <QTextBrowser>
#include <QMessageBox>
#include <QLayout>
#include <QDebug>

struct MainWindow::PrivateData {
	QDialog * dial;
	QTextBrowser * browser;
};

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::MainWindow),
		d(new PrivateData)
{
	ui->setupUi(this);

	fc = new FileController(this,this);
	ec = new EditController(this,this);
	sc = new SearchController(this,this);
	vc = new ViewController(this,this);
	pc = new ProjectController(this,this);
	bc = new BuildController(this,this);

	/* ****************************************************************** */

	//
	ui->projectExplorator->resize(200,ui->projectExplorator->height());

	// Expand the tabwidget
	ui->splitter->setStretchFactor( 1, 128 );
	ui->splitter2->setStretchFactor( 0, 5 );
	ui->splitter2->setStretchFactor( 1, 0 );
	ui->splitter2->setStretchFactor( 2, 0 );

	vc->setSearchDockVisibility(false);
	vc->setBuildDockVisibility(false);

	d->dial = new QDialog(this);
	d->dial->setWindowIcon(QIcon("qrc:/logo"));
	d->dial->setWindowTitle(tr("SkoobEditor -- Aide"));
	QVBoxLayout * layout = new QVBoxLayout(d->dial);
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);
	d->browser = new QTextBrowser(d->dial);
	d->browser->setSource(QUrl("qrc:/doc/index.html"));
	d->browser->setOpenExternalLinks(true);
	layout->addWidget(d->browser);
	d->dial->setLayout(layout);
	d->dial->resize(1024 + 5, 768 + 5);

	//
	connect( ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()) );
	connect( ui->actionHelp, SIGNAL(triggered()), this, SLOT(showHelp()) );
	connect( ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutDialog()) );
}


MainWindow::~MainWindow()
{
	delete d;
	delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
	bool oldAutoSyntax = bc->isAutoSyntaxCheck();
	bc->setAutoSyntaxCheck(false);
	if (fc->quit()) {
		event->accept();
	} else {
		bc->setAutoSyntaxCheck(oldAutoSyntax);
		event->ignore();
	}
}

void MainWindow::showHelp()
{
	d->dial->open();
}

void MainWindow::showAboutDialog()
{
	QString message;
	message += tr("SkoobEditor version 1.0b4\n\n");
	message += tr("Auteurs : Vincent Renaudineau, Pierre-Henri Wuillemin.\n\n");
	message += tr("Copyright 2010 Lip6 (Paris, France). Tous droits réservés.");
	message += tr("Ce logiciel est sous licence GPL v3.\n\n");
	message += tr("Il utilise les librairies :\n");
	message += tr(" - Qt, de Nokia;\n");
	message += tr(" - QScintilla, de Riverbank;\n");
	message += tr(" - aGrUM, du Lip6 (Pierre-Henri Wuillemin, Christophe Gonzales, Lionel Torti, Vincent Renaudineau).\n");
	QMessageBox::about( this, tr("À Propos de SkoobEditor"), message );
}

