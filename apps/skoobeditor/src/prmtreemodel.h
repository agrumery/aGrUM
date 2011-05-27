#ifndef PRMTREEMODEL_H
#define PRMTREEMODEL_H

#include <QStandardItemModel>
#include <QStringList>

#include <agrum/prm/PRM.h>
#include <agrum/prm/skoor/SkoorContext.h>

#include "qsciscintillaextended.h"

class PRMTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:

	/// enum PRM types.
	enum PRMObjects { Unknow, Type, Label, Class, Refererence, Attribute, Function, Aggregate, Interface, System, Instance, Session, Alias, Package };

	/// enum PRM data role.
	/// DisplayRole contains the complete path as a string (ex: classes.Computer.room)
	/// LocalDataRole contains the local data of the item as a QString (ex: room).
	/// ObjectRole is one of PRMObjects types (ex : PRMObjects::Refererence).
	/// TypeRole is a QStringList representing the type of the references, instances and aliases (ex: [classes,Room]).
	/// IsArrayRole is a boolean, use only for references and instances (ex: false).
	enum PRMRoles { LocalDataRole = Qt::UserRole, ObjectRole, TypeRole, IsArrayRole };

	explicit PRMTreeModel( const gum::prm::PRM * prm, const gum::prm::skoor::SkoorContext * context = 0, QObject *parent = 0);
	~PRMTreeModel();

	/// \reimp
	virtual bool canFetchMore ( const QModelIndex & parent ) const;
	/// \reimp
	virtual void fetchMore ( const QModelIndex & parent );

	/// Add keywords to model.
	void setKeywords( const QStringList & keywords );
	///
	void update( QSharedPointer<PRMTreeModel> prm, QsciScintillaExtended * currentDocument );

protected:
	const QString & separator() const { return m_separator; }

	QStandardItem * createChild( const QStringList & datas, QStandardItem * parent = 0 );
	QStandardItem * createChild( const QString & data, QStandardItem * parent = 0 );

	QStandardItem * getChild( const QString & data, QStandardItem * parent = 0 ) const;
	QStandardItem * getChild( const QStringList & path, QStandardItem * parent = 0 ) const;

	void appendSameChildren( const QStandardItem * from , QStandardItem * to );
	void removeSameChildren( const QStandardItem * from , QStandardItem * in );

	/// Set the current package to allow direct access of its members.
	void setCurrentPackage( const QStringList & package );
	/// Set the current class, interface or system, to allow direct access of its members.
	void setCurrentBlock( const QString & block);
	/// Set alias for skoor files.
	/// If alias already exist, it is updated.
	void addAlias( const QString & alias, const QStringList & to );

	///
	QList<QStandardItem *> findItems( const QVariant & data, int role = Qt::DisplayRole ) const;

private:
	static const QString & m_separator;
	QStandardItem * m_package;
	QStandardItem * m_block;
	QStandardItem * m_keywords;
	QList<QStandardItem* > m_aliases;
};

#endif // PRMTREEMODEL_H
