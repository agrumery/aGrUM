/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/** @file
 * @brief Class for storing sets of nodes.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides class gumNodeList, a container of nodes, and class
 * gumNodeListIterator that allows easy parsing of gumNodeList. The latter is
 * an abstract structure allowing to store a set of nodes as well as sharing
 * these sets among different gumNodeList. You should prefer using gumNodeList<Node>
 * to gumHashTable<gumId,Node> and gumList<Node> as it is written to be efficient
 * in terms of access time. gumNodeListIterator ensure that whenever a node is
 * removed from the list, the iterators pointing toward this element are informed
 * of this removal. Hence, trying to dereference such iterators will throw an
 * exception instead of resulting in a segmentation fault.
 *
 * Copying nodeLists through usual functions like operator= will result in a
 * completely fresh copy to be generated. However, this would be inadequate in some
 * cases (for instance during junction tree creations) as this requires copying
 * whole objects. So, to optimize, an alternative copy procedure is available, the
 * so-called "shallow copy". In this mode, only pointers on nodes are copied. As a
 * result, this creates a completely distinct new list containing pointers toward
 * the same nodes as the original list. Thus, as both lists are distinct, we can
 * remove or add elements to one list without modifying the other list. As the
 * lists contain pointers, creating new lists is fast as we do not need to create
 * new nodes. This kind of copy should be used with caution and probably only for
 * temporary usage. Once a gumNodeList has been copied through the shallowCopy
 * member, the other shallow_ methods can be used. Using the latter without having
 * performed a shallow copy will usually produce erroneous results (but will not
 * crash the application).
 *
 * @par Usage example:
 * @code
 * // create an empty node list
 * gumNodeList<gumNode> liste1;
 *
 * // insert 2 elements
 * list1.insert (gumNode (4,"node4"));
 * gumNode node (5,"node5"));
 * list1.insert (node);
 *
 * // get the node the id of which is 5
 * cerr << list1[5] << " = " << list1.get(5) << endl;
 *
 * // checks if there exists a node with ID = 6
 * if (!list1.exists (6)) cerr << "no node with ID 6" << endl;
 *
 * // check if the list is empty
 * if (!list1.empty() || (list1.size() != 0)) cerr << "not empty" << endl;
 *
 * // copy the list
 * gumNodeList<gumNode> liste2 = list1, list3;
 * list3 = list1;
 *
 * // remove elements from list3
 * list3.erase (4);
 * list3.erase (node);
 *
 * // create a new nodelist by mapping (assuming function f is defined by:
 * // mynode f (gumNode& x) { return mynode(x.getID(), "toto " + x.label()); }
 * gumNodeList<mynode> list4 = list1.map (f);
 *
 * // create similarly a gumHashTable and a gumList
 * gumHashTable<gumId,mynode> hash = list1.hashMap (f);
 * gumList<mynode> list5 = list1.listMap (f);
 *
 * // remove all the elements from the list
 * list4.clear ();
 *
 * // parse all the elements of list1
 * for (gumNodeList<gumNode>::iterator iter=list1.begin();iter!=list1.end();++iter)
 *   cerr << "ID of " << *iter << " = " << iter->getID() << endl;
 * @endcode
 */

#ifndef GUM_NODELIST_H
#define GUM_NODELIST_H


#include <utility>

#include <agrum/gum_utils.h>

#include <agrum/gumNode.h>
#include <agrum/utils/gumHashTable.h>
#include <agrum/gumList.h>
#include <agrum/gumRefPtr.h>

// classes provided by this file

template <typename Node> class gumNodeList;

template <typename Node> class gumNodeListIterator;



#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define GUM_NODELIST_ITERATOR_BEGIN  1
#define GUM_NODELIST_ITERATOR_RBEGIN 2
#define GUM_NODELIST_ITERATOR_END    3
#define GUM_NODELIST_ITERATOR_REND   4

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/// for friendly outpouting the containt of a list of nodes
template <typename Node>
std::ostream& operator<< ( std::ostream&, const gumNodeList<Node>& );




/* ============================================================================== */
/* ===                             LISTS OF NODES                             === */
/* ============================================================================== */
/** @class gumNodeList
 * @brief A basic class for storing lists of nodes.
 *
 * gumNodeList provides an efficient and flexible way to store lists of nodes. It
 * is used as a classical container, like hashtables or lists, and provides
 * iterators to parse the list. Those ensure that whenever a node is removed from
 * the list, the iterators pointing toward this element are informed of this
 * removal. Hence, trying to dereference such iterators will throw an exception
 * instead of resulting in a segmentation fault. Copying nodeLists through usual
 * functions like operator= will result in a completely fresh copy to be generated.
 * However, this would be inadequate in some cases (for instance during junction
 * tree creations) as this requires copying whole objects. So, to optimize, an
 * alternative copy procedure is available, the so-called "shallow copy". In this
 * mode, only pointers on nodes are copied. As a result, this creates a completely
 * distinct new list containing pointers toward the same nodes as the original
 * list. Thus, as both lists are distinct, we can remove or add elements to one
 * list without modifying the other list. As the lists contain pointers, creating
 * new lists is fast as we do not need to create new nodes. This kind of copy
 * should be used with caution and probably only for temporary usage. Once a
 * gumNodeList has been copied through the shallowCopy member, the other shallow_
 * methods can be used. Using the latter without having performed a shallow copy
 * will usually produce erroneous results (but will not crash the application).
 * @par Usage example:
 * @code
 * // create an empty node list
 * gumNodeList<gumNode> liste1;
 *
 * // insert 2 elements
 * list1.insert (gumNode (4,"node4"));
 * gumNode node (5,"node5"));
 * list1.insert (node);
 *
 * // get the node the id of which is 5
 * cerr << list1[5] << " = " << list1.get(5) << endl;
 *
 * // checks if there exists a node with ID = 6
 * if (!list1.exists (6)) cerr << "no node with ID 6" << endl;
 *
 * // check if the list is empty
 * if (!list1.empty() || (list1.size() != 0)) cerr << "not empty" << endl;
 *
 * // copy the list
 * gumNodeList<gumNode> liste2 = list1, list3;
 * list3 = list1;
 *
 * // remove elements from list3
 * list3.erase (4);
 * list3.erase (node);
 *
 * // create a new nodelist by mapping (assuming function f is defined by:
 * // mynode f (gumNode& x) { return mynode(x.getID(), "toto " + x.label()); }
 * gumNodeList<mynode> list4 = list1.map (f);
 *
 * // create similarly a gumHashTable and a gumList
 * gumHashTable<gumId,mynode> hash = list1.hashMap (f);
 * gumList<mynode> list5 = list1.listMap (f);
 *
 * // remove all the elements from the list
 * list4.clear ();
 *
 * // parse all the elements of list1
 * for (gumNodeList<gumNode>::iterator iter=list1.begin();iter!=list1.end();++iter)
 *   cerr << "ID of " << *iter << " = " << iter->getID() << endl;
 * @endcode
 */
/* ============================================================================== */

template <typename Node> class gumNodeList  {
  public:
    /// the iterators on gumCommonEdgeList
    typedef gumNodeListIterator<Node> iterator;


    // ##############################################################################
    /// @name Constructors / Destructors
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// basic constructor: creates an empty list
    /** @param size_param the size of the hash table used to store all the nodes
     * @param resize_pol the resizing policy of this hash table */
    // ==============================================================================
    explicit gumNodeList( gumSize size_param = GUM_HASHTABLE_DEFAULT_SIZE,
                          bool resize_pol = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY );

    // ==============================================================================
    /// copy constructor (by default deep copy)
    /** This copy constructor has two different behaviors, depending on the value of
     * parameter copy_type. If it is equal to GUM_DEEP_COPY (the default), a
     * deep copy is performed, that is, new Node instances are created and inserted
     * into the newly created gumNodeList. Thus, the latter and the copied gumNodeList
     * (from) are completely distinct in terms of memory locations. If, on the
     * contrary, copy_type is equal to GUM_SHALLOW_COPY, only Node pointers
     * are copied. As a result, all the nodes in the new gumNodeList are shared with
     * those of \e from. Thus, modifying the content of a node in one list will also
     * modify it in the other list. However, both lists are distinct in the sense that
     * removing a node from one list does not remove it from the other one. */
    // ==============================================================================
    gumNodeList( const gumNodeList<Node>& from, gumCopyType copy_type = GUM_DEEP_COPY );

    // ==============================================================================
    /// destructor
    // ==============================================================================
    virtual ~gumNodeList() ;

    /// @}



    // ##############################################################################
    /// @name Operators
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator (deep copy)
    /** The copy performed is a deep one, i.e., new Node instances are created and
     * inserted into the newly created gumNodeList. Thus, the latter and the copied
     * gumNodeList (from) are completely distinct in terms of memory locations. */
    // ==============================================================================
    gumNodeList<Node>& operator= ( const gumNodeList<Node>& from );

    // ==============================================================================
    /// returns a reference on the node the id of which is passed in argument
    /** @throw gumNotFound exception is thrown if the node cannot be found. */
    // ==============================================================================
    Node& operator[]( const gumId& id ) ;
    const Node& operator[]( const gumId& id ) const ;

    // ==============================================================================
    /// checks whether two lists of nodes contain the same elements
    /** The nodes are compared according to the Node::operator== */
    // ==============================================================================
    const bool operator== ( const gumNodeList<Node>& from ) const;

    // ==============================================================================
    /// checks whether two lists of nodes contain different elements
    // ==============================================================================
    const bool operator!= ( const gumNodeList<Node>& from ) const;

    /// @}



    // ##############################################################################
    /// @name Shallow members
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator (the new list shares its nodes with \e from)
    /** The current list will share its nodes with the copied one in the sense that
     * modifying the content of a node in one list will also modify it in the other
     * list. However, both lists are distinct in the sense that removing a node from
     * one list does not remove it from the other one. */
    // ==============================================================================
    gumNodeList<Node>& shallowCopy( const gumNodeList<Node>& from );

    // ==============================================================================
    /// checks whether two lists of nodes contain the same elements
    /** This method is faster than operator== but it produces a correct result only
     * if the current instance and \c from are related by shallowCopy, i.e., either
     * \c this has been created by shallowCopy (from), or the converse, or even
     * both are the result of successive shallowCopies of the same gumNodeList. */
    // ==============================================================================
    const bool shallowEqual( const gumNodeList<Node>& from ) const ;

    // ==============================================================================
    /// checks whether two lists of nodes contain different elements
    /** This method is faster than operator!= but it produces a correct result only
     * if the current instance and \c from are related by shallowCopy, i.e., either
     * \c this has been created by shallowCopy (from), or the converse, or even
     * both are the result of successive shallowCopies of the same gumNodeList. */
    // ==============================================================================
    const bool shallowDifferent( const gumNodeList<Node>& from ) const ;

    // ==============================================================================
    /// adds a shallow copy of a node into the node list
    /** @throw gumDuplicateElement exception is thrown if there already exists an
     * element with the same id in the list. */
    // ==============================================================================
    const gumRefPtr<Node>& shallowInsert( const gumRefPtr<Node>& node );

    // ==============================================================================
    /// returns a pointer to a given node (allows shallow copies)
    /** @throw gumNotFound exception is thrown if the node cannot be found */
    // ==============================================================================
    const gumRefPtr<Node>& shallowGet( const gumId& id )
    const ;

    // ==============================================================================
    /// returns a pointer to a given node (allows shallow copies)
    /** @throw gumNotFound exception is thrown if the node cannot be found */
    // ==============================================================================
    const gumRefPtr<Node>& shallowGet( const Node& node )
    const ;

    /// @}



    // ##############################################################################
    /// @name Fine tuning
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// returns the size of the nodes vector of the nodes hashtable
    /** The method runs in constant time. */
    // ==============================================================================
    const gumSize& capacity() const ;

    // ==============================================================================
    /// enables the user to change dynamically the resizing policy.
    /** In most cases, this should be useless. However, when available memory
     * becomes rare, avoiding automatic resizing may speed-up new insertions in
     * the table. */
    // ==============================================================================
    void setResizePolicy( const bool new_policy ) ;

    // ==============================================================================
    /// returns the current resizing policy.
    // ==============================================================================
    const bool getResizePolicy() const ;

    // ==============================================================================
    /// changes the size of the nodes vector of the node list
    /** The method runs in linear time in the size of the list. */
    // ==============================================================================
    void resize( gumSize new_size ) ;

    /// @}



    // ##############################################################################
    /// @name Accessors / Modifiers
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// alias for operator []
    /**
    * @throw gumNotFound ;
    */
    // ==============================================================================
    Node& get( const gumId& id );
    const Node& get( const gumId& id ) const;

    // ==============================================================================
    /// removes all the elements in the nodes list
    /** The method runs in linear time w.r.t. the number of iterators pointing to
     * the nodes list. It removes all the pointers stored in the nodelist. Moreover,
     * as pointers are smart, when no more pointer points onto a gumNode, the latter
     * is removed as well from memory. */
    // ==============================================================================
    void clear();

    // ==============================================================================
    /// indicates whether nodes list is empty
    // ==============================================================================
    const bool empty() const ;

    // ==============================================================================
    /// checks whether there exists a node with a given ID in the nodes list
    // ==============================================================================
    const bool exists( const gumId& id ) const ;

    // ==============================================================================
    /// returns the number of nodes in the nodes list
    /** The method runs in constant time. */
    // ==============================================================================
    const gumSize size() const ;

    // ==============================================================================
    /// adds a copy of \e node into the nodeList
    /** @return As a copy is inserted into the node list instead of the node itself,
     * the method returns a reference on the copy created.
     * @throw gumDuplicateElement exception is thrown if there already exists an
     * element with the same id in the list. */
    // ==============================================================================
    Node& insert( const Node& node );

    // ==============================================================================
    /// removes a node specified by its ID from the list of nodes
    /** If no such element can be found, nothing is done (in particular, it does
     * not throw any exception). */
    // ==============================================================================
    void erase( const gumId& id );

    // ==============================================================================
    /// removes a given node from the list of nodes
    /** If no such element can be found, nothing is done (in particular, it does
     * not throw any exception). */
    // ==============================================================================
    void erase( const Node& node );

    // ==============================================================================
    /// creates a gumNodeList of Assent from a gumNodeList of Node
    /** @param f a function that maps a node into a Assent node.
     * @param size the size of the hashtable resulting nodelist. When equal to 0,
     * a default size is computed that is a good trade-off between space consumption
     * and efficiency of new elements insertions
     * @warning by default, the order of the nodes in the resulting nodelist may
     * not be similar to that of the node list passed in argument. Hence iterators
     * on the former may not parse it in the same order as iterators on the latter. */
    // ==============================================================================
    template <typename Assent> gumNodeList<Assent>
    map( Assent( *f )( Node ), gumSize size = 0 ) const;
    template <typename Assent> gumNodeList<Assent>
    map( Assent( *f )( Node& ), gumSize size = 0 ) const;
    template <typename Assent> gumNodeList<Assent>
    map( Assent( *f )( const Node& ), gumSize size = 0 ) const;

    // ==============================================================================
    /// creates a hashtable of Assent from a node list
    /** @param f a function that maps a node into a Assent
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions
     * @warning by default, the order of the nodes in the resulting hashtable may
     * not be similar to that of the node list. Hence iterators on the former may
     * not parse it in the same order as iterators on the latter. */
    // ==============================================================================
    template <typename Assent> gumHashTable<gumId,Assent>
    hashMap( Assent( *f )( Node ), gumSize size = 0 ) const;
    template <typename Assent> gumHashTable<gumId,Assent>
    hashMap( Assent( *f )( Node& ), gumSize size = 0 ) const;
    template <typename Assent> gumHashTable<gumId,Assent>
    hashMap( Assent( *f )( const Node& ), gumSize size = 0 ) const;

    // ==============================================================================
    /// creates a hashtable of Assent from a node list
    /** @param val the value taken by all the elements of the resulting hashtable
     * @param size the size of the resulting hashtable. When equal to 0, a default
     * size is computed that is a good trade-off between space consumption and
     * efficiency of new elements insertions
     * @warning by default, the order of the nodes in the resulting hashtable may
     * not be similar to that of the node list. Hence iterators on the former may
     * not parse it in the same order as iterators on the latter. */
    // ==============================================================================
    template <typename Assent> gumHashTable<gumId,Assent>
    hashMap( const Assent& val, gumSize size = 0 ) const;

    // ==============================================================================
    /// returns a gumList of Assent generated from a Node list
    /** @param f a function that maps a node into a Assent
     * @warning the order of the Assent elements in the resulting list is
     * arbitrary */
    // ==============================================================================
    template <typename Assent> gumList<Assent>
    listMap( Assent( *f )( Node ) ) const;
    template <typename Assent> gumList<Assent>
    listMap( Assent( *f )( Node& ) ) const;
    template <typename Assent> gumList<Assent>
    listMap( Assent( *f )( const Node& ) ) const;

    // ==============================================================================
    /// returns a gumList of Assent generated from a Node list
    /** @param val the value taken by all the elements of the resulting list
     * @warning the order of the Assent elements in the resulting list is
     *  arbitrary */
    // ==============================================================================
    template <typename Assent> gumList<Assent> listMap( const Assent& val ) const;

    /// @}



    // ##############################################################################
    /// @name Iterators
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// returns an iterator pointing to the end of the gumNodeList
    // ==============================================================================
    const iterator& end() const ;

    // ==============================================================================
    /// returns the iterator pointing to the rend (beginning) of the gumNodeList
    // ==============================================================================
    const iterator& rend() const ;

    // ==============================================================================
    /// returns the iterator pointing to the beginning of the gumNodeList
    // ==============================================================================
    iterator begin() const;

    // ==============================================================================
    /// returns the iterator pointing to the last element of the gumNodeList
    // ==============================================================================
    iterator rbegin() const;

    /// @}


  protected:
    /// the actual set of nodes
    gumHashTable<gumId,gumRefPtr<Node> > nodes;

    /// @name pseudo static iterators
    /// @{
    /** the end and rend iterators are constructed only once per node list
     * so as to optimize for(iter = begin();iter != end(); iter++) loops: this
     * will avoid creating objects end and rend each time we pass in the loop. */
    /* WARNING: the end and rend iterators MUST be declared after the hash table.
     * This ensures that their construction will take place after the hash table
     * has been constructed. Hence they can rely directly on the initialized
     * members of the hash table. */
    gumNodeListIterator<Node> iter_end;
    gumNodeListIterator<Node> iter_rend;
    /// @}

    /// iterators should be able to access the content of the nodelists

    friend class gumNodeListIterator<Node>;
};







/* ============================================================================== */
/* ===                       ITERATORS ON LISTS OF NODES                      === */
/* ============================================================================== */
/** @class gumNodeListIterator
 * @brief Iterators for gumNodeList
 *
 * Developers may consider using gumNodeList<X>::iterator instead of
 * gumNodeListIterator<X>. These iterators are safe in the sense that they are
 * updated whenever the element they point to is deleted. Hence, trying to access
 * to this element through the iterator cannot crash the program. Instead, a
 * gumInvalidIteratorValue exception will be thrown.
 *
 * @par Usage example:
 * @code
 * // parse all the elements of list
 * for (gumNodeList<gumNode>::iterator iter=list.begin(); iter!=list.end(); ++iter)
 *   cerr << "ID of " << *iter << " = " << iter->label() << endl;
 * for (gumNodeList<gumNode>::iterator iter=list.rbegin();iter!=list.rend();--iter)
 *   cerr << "ID of " << *iter << " = " << iter->label() << endl;
 *
 * // display the ID of the node pointed to by the iterator
 * cerr << iter.getID() << endl;
 * @endcode
 */
/* ============================================================================== */

template <typename Node> class gumNodeListIterator {
  public:
    // ##############################################################################
    /// @name Constructors / Destructors
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// basic constructor: an iterator that points to nothing
    // ==============================================================================
    gumNodeListIterator() ;

    // ==============================================================================
    /** @brief constructor for an iterator pointing to the 'ind_elt'th element of
     * a gumNodeList.
     *
     * If that element does not exists, a gumUndefinedIteratorValue is thrown.
     * @throw gumUndefinedIteratorValue
     */
    // ==============================================================================
    gumNodeListIterator( gumNodeList<Node>& list, gumSize ind_elt );

    // ==============================================================================
    /// copy constructor
    // ==============================================================================
    gumNodeListIterator( const gumNodeListIterator<Node>& from ) ;

    // ==============================================================================
    /// destructor
    // ==============================================================================
    ~gumNodeListIterator() ;

    /// @}



    // ##############################################################################
    /// @name Accessors
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// returns the ID of the node pointed to by the current iterator
    /**
    * @throw gumUndefinedIteratorValue
    */
    // ==============================================================================
    const gumId& getID() const ;

    /// @}



    // ##############################################################################
    /// @name Operators
    // ##############################################################################
    /// @{

    // ==============================================================================
    /// copy operator
    // ==============================================================================
    gumNodeListIterator<Node>& operator= ( const gumNodeListIterator<Node>& from )
    ;

    // ==============================================================================
    /// makes the iterator point to the next element in the gumNodeList.
    /** for (iter=begin(); iter!=end(); ++iter) loops are guaranteed to parse the
     * whole node list as long as no element is added to or deleted from the
     * list while being in the loop. Deleting elements during the loop is
     * guaranteed to never produce a segmentation fault. */
    // ==============================================================================
    gumNodeListIterator<Node>& operator++() ;

    // ==============================================================================
    /// makes the iterator point to the preceding element in the gumNodeList
    /** for (iter=rbegin(); iter!=rend(); --iter) loops are guaranteed to
     * parse the whole nodelist as long as no element is added to or deleted from
     * the list while being in the loop. Deleting elements during the loop is
     * guaranteed to never produce a segmentation fault. */
    // ==============================================================================
    gumNodeListIterator<Node>& operator--() ;

    // ==============================================================================
    /// checks whether two iterators point to different nodes
    // ==============================================================================
    const bool operator!= ( const gumNodeListIterator<Node>& from ) const ;

    // ==============================================================================
    /// checks whether two iterators point to the same node
    // ==============================================================================
    const bool operator== ( const gumNodeListIterator<Node>& from ) const ;

    // ==============================================================================
    /// returns a reference on the node pointed to by the iterator
    /** @throws gumUndefinedIteratorValue exception is thrown if the iterator does
     * not point to a valid node */
    // ==============================================================================
    Node& operator*() ;
    const Node& operator*() const ;

    // ==============================================================================
    /// returns a shallow reference on the node pointed to by the iterator
    /**
    * @throw gumUndefinedIteratorValue
    */
    // ==============================================================================
    const gumRefPtr<Node>& shallowGet() const;

    // ==============================================================================
    /// dereferences the value pointed to by the iterator
    /** @throws gumUndefinedIteratorValue exception is thrown if the iterator does
     * not point to a valid node */
    // ==============================================================================
    Node* operator->() const ;

    /// @}



  private:
    /// gumNodeList must be a friend to properly set begin/rbegin/end/rend

    friend class gumNodeList<Node>;

    /** @brief as gumNodeLists are mainly hashtables, we need an iterator on
     * hashtables to iterate over the list of nodes */
    gumHashTableIterator<gumId,gumRefPtr<Node> > *iter;

    /** @brief indicates whether iter (above) belongs to this or to the hashtable
     * (is_end = true means that it belongs to the hashtable) */
    bool is_end;

    // ==============================================================================
    /** this constructor is only used to construct the begin/rbegin/end/rend
     * iterators of the gumNodeLists */
    // ==============================================================================
    gumNodeListIterator( const gumHashTable<gumId,gumRefPtr<Node> >& nodes, int type )
    ;

    // ==============================================================================
    /** this method is only used to initialize properly the begin/rbegin/end/rend
     * iterators of the gumNodeLists */
    // ==============================================================================
    void _initialize( const gumHashTable<gumId,gumRefPtr<Node> >& nodes, int type )
    ;
};


// always include the .tcc as it contains only templates
#include "gumNodeList.tcc"


#endif /* GUM_NODELIST_H */
