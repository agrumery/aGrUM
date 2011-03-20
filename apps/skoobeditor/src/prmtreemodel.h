#ifndef PRMTREEMODEL_H
#define PRMTREEMODEL_H

#include <QAbstractItemModel>

#include <agrum/prm/PRM.h>

class PRMTreeItem;

class PRMTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	PRMTreeModel( const gum::prm::PRM * prm, QObject * parent = 0 );
	~PRMTreeModel();

	/// Set the current package to allow direct access of member.
	bool setCurrentPackage( const QString & package );
	/// Set the current class, interface or system, to allow direct access of member.
	bool setCurrentBlock( const QString & block);
	/// Add keyworsd to model.
	void addKeywords( const QStringList & keywords );

	/// \reimp
	virtual QModelIndex parent( const QModelIndex & index ) const;
	virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual QModelIndex index( int row, int column, const QModelIndex & parent) const;
	virtual void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder );

private:
	PRMTreeItem * rootItem;
	PRMTreeItem * currentPackage;
	PRMTreeItem * currentBlock;
};

class PRMTreeItem {
public:
	PRMTreeItem( PRMTreeItem * parent, const QString & data);
	PRMTreeItem( PRMTreeItem * newParent, const PRMTreeItem * item );
	~PRMTreeItem();

	PRMTreeItem * createChild( const QStringList & list );
	QString toString(int tab = 0) const;
	QString completeData() const;
	PRMTreeItem * getChild( const QStringList & list );
	int row() const;
	void sort( Qt::SortOrder order = Qt::AscendingOrder );

	PRMTreeItem * parent;
	QList<PRMTreeItem *> children;
	QString localData;
};

#endif // PRMTREEMODEL_H
