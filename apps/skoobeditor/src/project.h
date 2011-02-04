#ifndef PROJECT_H
#define PROJECT_H

#include <QFileSystemModel>

class Project : public QFileSystemModel
{
	Q_OBJECT

public:
	Project(const QString & dir, QObject * parent = 0);
	~Project();

	QString name();
	QString dir();

	///
	bool isInside( const QString & filePath ) const;

	void addPath( const QString & path );
	QList<QString> paths() const;

	QModelIndex root() const;
	int columnCount(const QModelIndex &parent) const;
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/** Close the project. */
	void close();

private:
	struct PrivateData;
	PrivateData * d;
};

#endif // PROJECT_H
