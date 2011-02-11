#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "filecontroller.h"
#include "editcontroller.h"
#include "searchcontroller.h"
#include "viewcontroller.h"
#include "projectcontroller.h"
#include "buildcontroller.h"
#include "qsciscintillaextended.h"

#include <QMessageBox>
#include <QDebug>

struct MainWindow::PrivateData {
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

	//
	connect( ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()) );
	connect( ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutDialog()) );
}


MainWindow::~MainWindow()
{
	delete d;
	delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
	bc->setAutoSyntaxCheck(false);
	if (fc->quit()) {
		event->accept();
	} else {
		bc->setAutoSyntaxCheck(true);
		event->ignore();
	}
}


void MainWindow::showAboutDialog()
{
	QString message;
	message += tr("SkoobEditor beta 1.0b1\n\n");
	message += tr("Auteurs : Vincent Renaudineau, Pierre-Henri Wuillemin.\n\n");
	message += tr("Copyright 2010 Lip6 (Paris, France). Tous droits réservés.");
	message += tr("Ce logiciel est sous licence GPL v3.");
	QMessageBox::about( this, tr("À Propos de SkoobEditor"), message );
}
