#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);

	ui->projectDirectory->setText(QDir::homePath());
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

void NewProjectDialog::on_selectDirectoryButton_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this,
					tr("Sélectionnez le répertoire du nouveau projet"),
					QDir::homePath());

	if (dir.isEmpty())
		return;

	ui->projectDirectory->setText(dir);
}

void NewProjectDialog::on_buttonBox_accepted()
{
	QString dir = QDir::cleanPath( ui->projectDirectory->text() + QDir::separator() + ui->projectName->text() + QDir::separator() );

	if ( dir.isEmpty() )
		return;

	if ( ! QDir(dir).exists() ) {

		int button = QMessageBox::question(this,
						tr("Nouveau projet"),
						tr("Le répertoire '%1' n'existe pas.\nVoulez-vous créer les répertoires ?").arg(dir),
						QMessageBox::Ok,
						QMessageBox::Cancel);

		if ( button == QMessageBox::Ok )
			QDir(QDir::root()).mkpath(dir);
		else
			return;
	} else {

		int button = QMessageBox::question(this,
						tr("Nouveau projet"),
						tr("Le répertoire '%1' existe déjà.\nVoulez-vous l'écraser?").arg(dir),
						QMessageBox::Ok,
						QMessageBox::Cancel);

		if ( button == QMessageBox::Ok ) {
			QDir(QDir::root()).rename(dir,dir+".old");
			QDir(QDir::root()).mkpath(dir);
		} else
			return;
	}

	accept();
}


QString NewProjectDialog::projectName()
{
	if ( result() )
		return ui->projectName->text();
	else
		return QString();

}


QString NewProjectDialog::projectDir()
{
	if ( result() )
		return QDir::cleanPath( ui->projectDirectory->text() + QDir::separator() + ui->projectName->text() + QDir::separator() );
	else
		return QString();

}
