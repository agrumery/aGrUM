/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
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
 * @brief Base node set class for graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#ifndef GUM_NODE_GRAPH_PART_H
#define GUM_NODE_GRAPH_PART_H

#include <algorithm>
#include <utility>
#include <agrum/core/utils.h>
#include <agrum/graphs/graphElements.h>


namespace gum {


  /**
   * @class NodeGraphPart
   * @brief Classes for node sets
   *
   * \ingroup graph_group
   *
   * NodeGraphPart wraps a NodeSet and implements an NodeId factory
   *
   * @author Pierre-Henri WUILLEMIN and Christophe GONZALES*
   *
   * @par Usage example:
   * @code
   * // create an empty node list
   * NodeGraphPart nodes1;
   *
   * // insert 2 elements
   * NodeId id_a=nodes1.insertNode( );
   * NodeId id_b=nodes1.insertNode( );
   *
   * std::cerr<<"a="<<id_a<<"    b="<<id_b<<std::endl;
   *
   * // checks if there exists a node with ID = 6
   *
   * if ( !nodes1.exists( 6 ) ) std::cerr << "no node with ID 6" << std::endl;
   *
   * // check if the list is empty
   * if ( !nodes1.empty() || ( nodes1.size() != 0 ) )
   *   std::cerr << "nodes1 is not empty" << std::endl;
   *
   * // copy the list
   * NodeGraphPart nodes2 = nodes1, nodes3;
   *
   * nodes3 = nodes1;
   *
   * // remove elements from list3
   * nodes3.eraseNode( id_a );
   *
   * nodes3.eraseNode( id_b );
   *
   * // remove all the elements from the list
   * nodes1.clear();
   *
   * for ( NodeGraphPart::iterator it=nodes2.beginNodes();
   *       it!=nodes2.endNodes();++it ) {
   *   std::cerr<<*it<<"  ";
   * }
   *
   * std::cerr<<"list : "<<nodes2.listMapNodes(getDouble)<<std::endl;
   *
   * std::cerr<<"hashmap : "<<nodes2.hashMapNodes(getDouble)<<std::endl;
   * @endcode
   */

  class NodeGraphPart {
  public:
    /** @param nodes_size the size of the hash table used to store all the nodes
     * @param nodes_resize_policy the resizing policy of this hash table**/
    explicit NodeGraphPart( Size nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                            bool nodes_resize_policy    = true );
    NodeGraphPart( const NodeGraphPart& s );
    virtual ~NodeGraphPart();

    /**
     * populateNodes clears *this and take nodes from s or h keys. It is basically
     * the only way to insert nodes with IDs not selected by the internal idFactory.
     */
    void populateNodes( const NodeGraphPart& s );

    /**
     * populateNodes clears *this and take nodes from h keys. It is basically
     * the only way to insert nodes with IDs not selected by the internal idFactory.
     */
    template<typename T> void populateNodesFromProperty( const typename Property<T>::onNodes& h );

    /** insert a new node
     * @return the id chosen by the internal idFactory
     */
    virtual NodeId insertNode ( );
    virtual void insertNode ( const NodeId id );

    virtual void eraseNode( const NodeId id );
    bool exists( const NodeId id ) const;
    bool empty() const;
    virtual void clear();
    Size size() const;
    NodeId maxId () const;
    
    const NodeSet& nodes()  const;
    const NodeSetIterator beginNodes() const;
    const NodeSetIterator& endNodes() const;

    bool operator==( const NodeGraphPart& p ) const;
    bool operator!=( const NodeGraphPart& p ) const;
    const std::string toString() const;

    /** a method to create a HashTable with key:NodeId and value:Assent
     * (i.e. a GUM_NODE_PROPERTY(Assent)) from the nodes (using for all node x,
     * the ASSENT f(x))
     * This method is a wrapper of the same method in HashTable.
     * @see HashTable::map.
     */
    template <typename ASSENT> typename Property<ASSENT>::onNodes nodesProperty( ASSENT( *f )( const NodeId& ), Size size = 0 ) const;

    /** a method to create a hashMap with key:NodeId and value:Assent (i.e. a
     * GUM_NODE_PROPERTY(Assent)) from the nodes (using for all node x, the ASSENT a)
     * This method is a wrapper of the same method in HashTable.
     * @see HashTable::map.
     */
    template <typename ASSENT> typename Property<ASSENT>::onNodes nodesProperty( const ASSENT& a, Size size = 0 ) const;

    /// a method to create a list of Assent from a node list
    template <typename ASSENT> List<ASSENT> listMapNodes( ASSENT( *f )( const NodeId& ) ) const;



  private:
    /// internal idFactory
    NodeId __nextNodeId();
    Set<NodeId> __nodes;
    NodeId __max;
  };

  /// for friendly displaying the content of node set
  std::ostream& operator<< ( std::ostream&, const NodeGraphPart& );
  

} /* namespace gum */

  
#ifndef GUM_NO_INLINE
#include <agrum/graphs/nodeGraphPart.inl>
#endif //GUM_NOINLINE


#include <agrum/graphs/nodeGraphPart.tcc>

#endif // GUM_NODE_GRAPH_PART_H
