#include "prmtreemodel.h"

#include <QStringList>
#include <QDebug>

using namespace gum::prm;

/*
################################################################################
##                                                                            ##
##                        INTERNAL REPRESENTATION                             ##
##                                                                            ##
################################################################################
 */

PRMTreeItem::PRMTreeItem( PRMTreeItem * parent, const QString & data)
	: parent(parent), localData(data)
{
}


PRMTreeItem::PRMTreeItem( PRMTreeItem * newParent, const PRMTreeItem * item )
	: parent(newParent), localData(item->localData)
{
	foreach( PRMTreeItem * child, item->children ) {
		children << new PRMTreeItem( this, child );
	}
}

PRMTreeItem::~PRMTreeItem()
{
	foreach( PRMTreeItem * item, children )
		delete item;
}

PRMTreeItem * PRMTreeItem::createChild( const QStringList & list )
{
	if (list.isEmpty())
		return 0;

	foreach ( PRMTreeItem * child, children )
		if ( child->localData == list.first() )
			return child->createChild( list.mid(1) );

	children.append( new PRMTreeItem(this,list.first()) );
	if ( list.size() == 1 )
		return children.last();
	else
		return children.last()->createChild( list.mid(1) );
}

QString PRMTreeItem::toString(int tab) const
{
	QString s;
	for ( int i = 0; i < tab ; i++ ) s += "    ";
	s += localData + "\n";
	foreach ( PRMTreeItem * item, children )
		s += item->toString(tab + 1);
	return s;
}

QString PRMTreeItem::completeData() const
{
	if ( parent == 0 || parent->localData.isEmpty() )
		return localData;
	else
		return parent->completeData() + "." + localData;
}

PRMTreeItem * PRMTreeItem::getChild( const QStringList & list )
{
	if ( list.isEmpty() )
		return this;
	foreach ( PRMTreeItem * child, children )
		if ( child->localData == list.first() )
			return child->getChild(list.mid(1));
	return 0;
}

int PRMTreeItem::row() const
{
	if ( parent )
		for ( int i = 0, size = parent->children.size() ; i < size ; i++ )
			if ( parent->children[i] == this )
				return i;
		//return parent->children.indexOf( this ); // don't work ?? cast error
	return 0;
}

/*
################################################################################
##                                                                            ##
##                        EXTERNAL REPRESENTATION                             ##
##                                                                            ##
################################################################################

 TODO

	// METHODE setCurrentPackage
	// copie le PRMTreeItem du package à la racine,
	// comme ça, tout ce qui est dedans n'a pas besoin d'être précédé du package

	// METHODE setCurrentType
	// copie le PRMTreeItem de la classe à la racine,
	// comme ça, on a accès aux attributs et ref directement.

*/

/// Constructor
PRMTreeModel::PRMTreeModel( PRM * aPrm, QObject * parent )
	: QAbstractItemModel(parent), prm(aPrm), rootItem(new PRMTreeItem(0, "")), currentPackage(0), currentBlock(0)
{
	// Construction du PRMTreeModel

	foreach ( Type * type, prm->types() ) {
		rootItem->createChild( QString::fromStdString( type->name() ).split(".") );
		const gum::DiscreteVariable & var = type->variable();
		for ( int i = 0 ; i < var.domainSize() ; i++ )
			rootItem->createChild( QStringList() << QString::fromStdString( var.label(i) ) );
	}

	QList< QPair<PRMTreeItem *, ClassElementContainer *> > refList;

	foreach ( Interface * i, prm->interfaces() ) {
		PRMTreeItem * interfaceItem = rootItem->createChild( QString::fromStdString( i->name() ).split(".") );
		if (interfaceItem != 0)
			refList << QPair<PRMTreeItem *, ClassElementContainer *>(interfaceItem, i);
	}

	foreach ( Class * c, prm->classes() ) {
		PRMTreeItem * classItem = rootItem->createChild( QString::fromStdString( c->name() ).split(".") );
		if (classItem != 0)
			refList << QPair<PRMTreeItem *, ClassElementContainer *>(classItem, c);
	}

	foreach ( System * s, prm->systems() ) {
		PRMTreeItem * item = rootItem->createChild( QString::fromStdString( s->name() ).split(".") );
//		if (item != 0) {
//			foreach ( Attribute * a, s->attributes() )
//				item->createChild( QStringList() << QString::fromStdString(a->name()) );
//			foreach ( ReferenceSlot * r, s->referenceSlots() ) {
//				item->createChild( QStringList() << QString::fromStdString(r->name()) );
//		}
	}

	QPair<PRMTreeItem *, ClassElementContainer *> p;
	while ( ! refList.isEmpty() ) {
		p = refList.takeFirst();
		// je recupère le type de la ref,
		Interface * i;
		Class * c;
		switch ( p.second->obj_type() ) {
		case PRMObject::prm_interface:
			i = static_cast<Interface *>( p.second );
			Q_ASSERT( i != 0 );
			//  Pour chaque attibut
			foreach ( Attribute * a, i->attributes() )
				//    je l'ajoute en enfant
				p.first->createChild( QStringList() << QString::fromStdString(a->name()) );
			//  Pour chaque ref
			foreach ( ReferenceSlot * r, i->referenceSlots() ) {
				//    je l'ajoute en enfant
				PRMTreeItem * refItem = p.first->createChild( QStringList() << QString::fromStdString(r->name()) );
				//    je l'ajoute à la liste
				refList << QPair<PRMTreeItem *, ClassElementContainer *>(refItem, &(r->slotType()));
			}
			break;
		case PRMObject::prm_class :
			c = static_cast<Class *>( p.second );
			Q_ASSERT( c != 0 );
			//  Pour chaque attibut
			foreach ( Attribute * a, c->attributes() )
				//    je l'ajoute en enfant
				p.first->createChild( QStringList() << QString::fromStdString(a->name()) );
			//  Pour chaque ref
			foreach ( ReferenceSlot * r, c->referenceSlots() ) {
				//    je l'ajoute en enfant
				PRMTreeItem * refItem = p.first->createChild( QStringList() << QString::fromStdString(r->name()) );
				//    je l'ajoute à la liste
				refList << QPair<PRMTreeItem *, ClassElementContainer *>(refItem, &(r->slotType()));
			}
			break;
		}
	}
}

PRMTreeModel::~PRMTreeModel()
{
	delete rootItem;
	delete prm;
}



/// Set the current package to allow direct access of member.
bool PRMTreeModel::setCurrentPackage( const QString & package )
{
	// If package is already currentPackage
	if ( currentPackage && currentPackage->completeData() == package )
		return true;

	// Delete previous currentPackage
	if ( currentPackage ) {
		foreach( PRMTreeItem * item, currentPackage->children ) {
			rootItem->children.removeOne(item);
			delete item;
		}
		currentPackage = 0;
	}

	// Search new currentPackage
	currentPackage = rootItem->getChild( package.split(QChar('.')) );
	if ( currentPackage == 0 )
		return false;

	// Set currentPackage;
	foreach( PRMTreeItem * item, currentPackage->children )
		rootItem->children.append( new PRMTreeItem(rootItem, item) );

	return true;
}

/// Set the current class, interface or system, to allow direct access of member.
bool PRMTreeModel::setCurrentBlock( const QString & block)
{
	// If block is already currentBlock
	if ( currentBlock && currentBlock->completeData() == block )
		return true;

	// Delete previous currentBlock
	if ( currentBlock ) {
		rootItem->children.removeOne(currentBlock);
		delete currentBlock;
		currentBlock = 0;
	}

	// Search new currentBlock
	PRMTreeItem * item = rootItem->getChild( block.split(QChar('.')) );
	if ( item == 0 )
		return false;

	// Set currentBlock
	currentBlock = new PRMTreeItem(rootItem, item);
	rootItem->children.append( currentBlock );

	return true;
}

/// \reimp
QModelIndex PRMTreeModel::parent( const QModelIndex & index ) const
{
	if ( ! index.isValid() )
		return QModelIndex();

	PRMTreeItem * child = static_cast<PRMTreeItem *>( index.internalPointer() );
	PRMTreeItem * parent = child->parent;

	if ( parent == rootItem )
		return QModelIndex();

	return createIndex( parent->row(), 0, parent );
}

/// \reimp
int PRMTreeModel::rowCount( const QModelIndex & index ) const
{
	if ( ! index.isValid() )
		return rootItem->children.size();
	return static_cast<PRMTreeItem *>( index.internalPointer() )->children.size();
}

/// \reimp
int PRMTreeModel::columnCount( const QModelIndex & index ) const
{
	return 1;
}

/// \reimp
QVariant PRMTreeModel::data( const QModelIndex & index, int role ) const
{
	if ( index.isValid() && role == Qt::DisplayRole )
		return static_cast<PRMTreeItem *>( index.internalPointer() )->completeData();
	else if ( index.isValid() && role == Qt::EditRole )
		return static_cast<PRMTreeItem *>( index.internalPointer() )->localData;
	else
		return QVariant();
}

/// \reimp
QModelIndex PRMTreeModel::index( int row, int column, const QModelIndex & parent) const
{
	if ( column != 0 )
		return QModelIndex();

	if ( ! parent.isValid() )
		return createIndex( row, 0, rootItem->children.at(row) );

	return createIndex( row, 0, static_cast<PRMTreeItem *>( parent.internalPointer() )->children.at(row) );
}
