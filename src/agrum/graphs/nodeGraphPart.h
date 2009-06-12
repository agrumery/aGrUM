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

#include <agrum/core/signal/signaler.h>


namespace gum {


  /**
   * @class NodeGraphPart
   * @brief Classes for node sets
   *
   * \ingroup graph_group
   *
   * NodeGraphPart wraps a NodeSet and implements an NodeId factory
   *
   * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
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
      Signaler1<NodeId> onNodeAdded;
      Signaler1<NodeId> onNodeDeleted;

      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{

      /// default constructor
      /** @param nodes_size the size of the hash table used to store all the nodes
       * @param nodes_resize_policy the resizing policy of this hash table**/
      explicit NodeGraphPart( Size nodes_size = GUM_HASHTABLE_DEFAULT_SIZE,
                              bool nodes_resize_policy    = true );

      /// copy constructor
      NodeGraphPart( const NodeGraphPart& s );

      /// destructor
      virtual ~NodeGraphPart();

      /// @}



      // ############################################################################
      /// @name Operators
      // ############################################################################
      /// @{

      /// check whether two NodeGraphParts contain the same nodes
      bool operator==( const NodeGraphPart& p ) const;

      /// check whether two NodeGraphParts contain different nodes
      bool operator!=( const NodeGraphPart& p ) const;

      /// @}



      // ############################################################################
      /// @name Accessors/Modifiers
      // ############################################################################
      /// @{

      /// populateNodes clears *this and fills it with the same nodes as "s"
      /** It is basically the only way to insert nodes with IDs not selected by the
       * internal idFactory. */
      void populateNodes( const NodeGraphPart& s );

      /// populateNodes clears *this and fills it with the keys of "h"
      /** It is basically the only way to insert nodes with IDs not selected by the
       * internal idFactory. */
      template<typename T>
      void populateNodesFromProperty( const typename Property<T>::onNodes& h );

      /** insert a new node and
       * @return the id chosen by the internal idFactory
       */
      virtual NodeId insertNode( );

      /// try to insert a node with the given id
      /** @throws DuplicateElement exception is thrown if the id already exists
       * @return the id chosen by the internal idFactory
       */
      virtual void insertNode( const NodeId id );

      /// erase the node with the given id
      /** If the NodeGraphPart does not contain the nodeId, then nothing is done. In
       * particular, no exception is raised. */
      virtual void eraseNode( const NodeId id );

      /// returns true iff the NodeGraphPart contains the given nodeId
      bool exists( const NodeId id ) const;

      /// indicates whether there exists nodes in the NodeGraphPart
      bool empty() const;

      /// remove all the nodes from the NodeGraphPart
      virtual void clear();

      /// returns the number of nodes in the NodeGraphPart
      Size size() const;

      /// returns a number such that all node ids are less than or equal to it
      NodeId maxId() const;

      /// returns the set of nodes contained in the NodeGraphPart
      const NodeSet& nodes()  const;

      /// a begin iterator to parse the set of nodes contained in the NodeGraphPart
      const NodeSetIterator beginNodes() const;

      /// the end iterator to parse the set of nodes contained in the NodeGraphPart
      const NodeSetIterator& endNodes() const;

      /// a function to display the set of nodes
      std::string toString() const;


      /// a method to create a HashTable with key:NodeId and value:VAL
      /** VAL are computed from the nodes using for all node x, VAL f(x).
       * This method is a wrapper of the same method in HashTable.
       * @see HashTable::map.
       */
      template <typename VAL>
      typename Property<VAL>::onNodes
      nodesProperty( VAL( *f )( const NodeId& ), Size size = 0 ) const;

      /// a method to create a hashMap with key:NodeId and value:VAL
      /** for all nodes, the value stored is a. This method is a wrapper of the same
       * method in HashTable.
       * @see HashTable::map.
       */
      template <typename VAL>
      typename Property<VAL>::onNodes
      nodesProperty( const VAL& a, Size size = 0 ) const;

      /// a method to create a list of VAL from the set of nodes
      template <typename VAL>
      List<VAL>
      listMapNodes( VAL( *f )( const NodeId& ) ) const;



    private:
      /// internal idFactory
      NodeId __nextNodeId();

      /// the set of nodes contained in the NodeGraphPart
      Set<NodeId> __nodes;

      /** @brief the id above which nodes ids are guaranteed to not belong yet to
       * the NodeGraphPart */
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
