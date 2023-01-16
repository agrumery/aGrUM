/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief Base node set class for graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_NODE_GRAPH_PART_H
#define GUM_NODE_GRAPH_PART_H

#include <algorithm>
#include <utility>

#include <agrum/agrum.h>

#include <agrum/tools/graphs/graphElements.h>

#include <agrum/tools/core/signal/signalers.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum_tests {

  class NodeGraphPartTestSuite;
}

#endif

namespace gum {

  class NodeGraphPart;

  /**
   * @class NodeGraphPartIterator
   * @brief Unsafe iterator on the node set of a graph.
   */
  class NodeGraphPartIterator {
    friend class NodeGraphPart;

    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using value_type        = NodeId;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /**
     * @brief Default constructor.
     */
    NodeGraphPartIterator(const NodeGraphPart& nodes) noexcept;

    /// copy constructor
    NodeGraphPartIterator(const NodeGraphPartIterator& it) noexcept;

    /// move constructor
    NodeGraphPartIterator(NodeGraphPartIterator&& it) noexcept;

    /// destructor
    virtual ~NodeGraphPartIterator() noexcept;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy assignment operator
    NodeGraphPartIterator& operator=(const NodeGraphPartIterator& it) noexcept;

    /// move assignment operator
    NodeGraphPartIterator& operator=(NodeGraphPartIterator&& it) noexcept;

    /// checks whether two iterators point toward the same node
    bool operator==(const NodeGraphPartIterator& it) const noexcept;

    /// checks whether two iterators point toward different nodes
    bool operator!=(const NodeGraphPartIterator& it) const noexcept;

    /// increment the iterator
    NodeGraphPartIterator& operator++() noexcept;

    /// dereferencing operator
    value_type operator*() const;

    /// @}

    protected:
    /// @brief this function is used by @ref NodeGraphPart to update
    void setPos_(NodeId id) noexcept;

    /// ensure that the nodeId is either end() either a valid NodeId
    void validate_() noexcept;

    /// the nodegraphpart on which points the iterator
    const NodeGraphPart* nodes_;

    /// the nodeid on which the iterator points currently
    NodeId pos_{0};

    // is this iterator still valid ?
    bool valid_{false};
  };

  /**
   * @class NodeGraphPartIteratorSafe
   * @brief Safe iterator on the node set of a graph.
   */
  class NodeGraphPartIteratorSafe: public NodeGraphPartIterator, public Listener {
    friend class NodeGraphPart;

    public:
    /// types for STL compliance
    /// @{
    using iterator_category = std::forward_iterator_tag;
    using value_type        = NodeId;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using difference_type   = std::ptrdiff_t;
    /// @}

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    NodeGraphPartIteratorSafe(const NodeGraphPart& nodes);

    /// copy constructor
    NodeGraphPartIteratorSafe(const NodeGraphPartIteratorSafe& it);

    /// move constructor
    NodeGraphPartIteratorSafe(NodeGraphPartIteratorSafe&& it);

    /// destructor
    ~NodeGraphPartIteratorSafe();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy assignment operator
    NodeGraphPartIteratorSafe& operator=(const NodeGraphPartIteratorSafe& it);

    /// move assignment operator
    NodeGraphPartIteratorSafe& operator=(NodeGraphPartIteratorSafe&& it);

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// called when a node is deleted in the iterated NodeGraphPart
    /**
     * @param src the NodeGraphPart
     * @param id id of deleted node
     */
    void whenNodeDeleted(const void* src, NodeId id);

    /// @}
  };

  /**
   * @class NodeGraphPart
   * @brief Class for node sets in graph
   *
   * @ingroup graph_group
   *
   * NodeGraphPart represents the set of nodes of all the graphs. It is built to
   * be as light as possible and it implements its own NodeId factory.
   * The set of NodeId is 0 ... ( _bound_-1) minus the NodeIds in
   *  _holes_.
   *
   * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
   *
   *
   * @par Usage example:
   * @code
   * // create an empty node list
   * NodeGraphPart nodes1;
   *
   * // add 2 elements
   * NodeId id_a=nodes1.addNode( );
   * NodeId id_b=nodes1.addNode( );
   *
   * // checks if there exists a node with ID = 6
   * if ( !nodes1.exists( 6 ) ) std::cerr << "no node with ID 6" << std::endl;
   *
   * // check if the set of nodes is empty
   * if ( !nodes1.empty() || ( nodes1.size() != 0 ) )
   *   std::cerr << "nodes1 is not empty" << std::endl;
   *
   * // copy a set of nodes
   * NodeGraphPart nodes2 = nodes1, nodes3;
   * nodes3 = nodes1;
   *
   * // remove elements from list3
   * nodes3.eraseNode( id_a );
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
    /// types for STL compliance
    /// @{
    using node_iterator            = NodeGraphPartIterator;
    using node_const_iterator      = NodeGraphPartIterator;
    using node_iterator_safe       = NodeGraphPartIteratorSafe;
    using node_const_iterator_safe = NodeGraphPartIteratorSafe;
    /// @}

    // something strange with SWIG (with "using", these definitions cored dump
    // the
    // swig process)
    using NodeIterator          = NodeGraphPartIterator;
    using NodeConstIterator     = NodeGraphPartIterator;
    using NodeIteratorSafe      = NodeGraphPartIteratorSafe;
    using NodeConstIteratorSafe = NodeGraphPartIteratorSafe;

    Signaler1< NodeId > onNodeAdded;
    Signaler1< NodeId > onNodeDeleted;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** A NodeGrphPart does not store all its nodes. To be lighter in terms of
     * memory consumption, it store its maximal NodeId and the set of NodeIds
     * between 0 and this maximum that do not actually belong to the set of its
     * nodes (the so-called set of holes). In practice, it turns out that the
     * set of holes is most often very small.
     * @param holes_size the size of the hash table used to store all holes
     * @param holes_resize_policy the resizing policy of this hash table**/
    explicit NodeGraphPart(Size holes_size          = HashTableConst::default_size,
                           bool holes_resize_policy = true);

    /// copy constructor
    /** @param s the NodeGraphPart to be copied */
    NodeGraphPart(const NodeGraphPart& s);

    /// destructor
    virtual ~NodeGraphPart();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param p the NodeGraphPart to be copied */
    NodeGraphPart& operator=(const NodeGraphPart& p);

    /// check whether two NodeGraphParts contain the same nodes
    /** @param p the NodeGraphPart to be compared with "this" */
    bool operator==(const NodeGraphPart& p) const;

    /// check whether two NodeGraphParts contain different nodes
    /** @param p the NodeGraphPart to be compared with "this" */
    bool operator!=(const NodeGraphPart& p) const;

    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// populateNodes clears *this and fills it with the same nodes as "s"
    /** populateNodes should basically be the preferred way to insert nodes with
     * IDs not selected by the internal idFactory.
     * @param s the NodeGraphPart to be copied */
    void populateNodes(const NodeGraphPart& s);

    /// populateNodesFromProperty clears *this and fills it with the keys of "h"
    /** populateNodes should basically be the preferred way to insert nodes with
     * IDs not selected by the internal idFactory. */
    template < typename T >
    void populateNodesFromProperty(const NodeProperty< T >& h);

    /** returns a new node id, not yet used by any node
     * @warning a code like @code id=nextNodeId();addNode(id); @endcode is
     * basically not thread safe !!
     * @return a node id not yet used by any node within the NodeGraphPart */
    NodeId nextNodeId() const;

    /// insert a new node and return its id
    /** @return the id chosen by the internal idFactory */
    virtual NodeId addNode();

    /** insert n nodes
     *
     * @param n the number of nodes to add
     * @return the vector of chosen ids
     */
    std::vector< NodeId > addNodes(Size n);

    /// try to insert a node with the given id
    /** @warning This method should be carefully used. Please prefer
     * @ref populateNodes or @ref populateNodesFromProperty when possible
     * @throws DuplicateElement exception if the id already exists
     */
    virtual void addNodeWithId(const NodeId id);

    /// erase the node with the given id
    /** If the NodeGraphPart does not contain the nodeId, then nothing is done.
     * In
     * particular, no exception is raised. However, the signal onNodeDeleted is
     * fired only if a node is effectively removed. */
    virtual void eraseNode(const NodeId id);

    /// returns true iff the NodeGraphPart contains the given nodeId
    bool existsNode(const NodeId id) const;

    /// alias for @ref existsNode
    bool exists(const NodeId id) const;

    /// indicates whether there exists nodes in the NodeGraphPart
    bool emptyNodes() const;

    /// alias for @ref emptyNodes
    bool empty() const;

    /// remove all the nodes from the NodeGraphPart
    virtual void clearNodes();

    /// alias for @ref clearNodes
    virtual void clear();

    /// returns the number of nodes in the NodeGraphPart
    Size sizeNodes() const;

    /// alias for @ref sizeNodes
    Size size() const;

    /// returns a number n such that all node ids are strictly lower than n
    NodeId bound() const;

    /// returns a copy of the set of nodes represented by the NodeGraphPart
    /** @warning this function is o(n) where n is the number of nodes. In space
     * and
     * in time. Usually, when you need to parse the nodes of a NodeGraphPart,
     * prefer
     * using @code for(const auto n : nodes()) @endcode rather than
     * @code for(const auto n : asNodeSet()) @endcode as this is faster and
     * consumes much less memory. */
    NodeSet asNodeSet() const;

    /// return *this as a NodeGraphPart
    const NodeGraphPart& nodes() const;

    /// a begin iterator to parse the set of nodes contained in the
    /// NodeGraphPart
    node_iterator_safe beginSafe() const;

    /// the end iterator to parse the set of nodes contained in the
    /// NodeGraphPart
    const node_iterator_safe& endSafe() const noexcept;

    /// a begin iterator to parse the set of nodes contained in the
    /// NodeGraphPart
    node_iterator begin() const noexcept;

    /// the end iterator to parse the set of nodes contained in the
    /// NodeGraphPart
    const node_iterator& end() const noexcept;

    virtual   /// a function to display the set of nodes
       std::string
       toString() const;

    /// a method to create a HashTable with key:NodeId and value:VAL
    /** VAL are computed from the nodes using for all node x, VAL f(x).
     * This method is a wrapper of the same method in HashTable.
     * @see HashTable::map.
     * @param f a function assigning a VAL to any node
     * @param size an optional parameter enabling to fine-tune the returned
     * Property. Roughly speaking, it is a good practice to have a size equal to
     * half the number of nodes. If you do not specify this parameter, the
     * method
     * will assign it for you. */
    template < typename VAL >
    NodeProperty< VAL > nodesProperty(VAL (*f)(const NodeId&), Size size = 0) const;

    /// a method to create a hashMap with key:NodeId and value:VAL
    /** for all nodes, the value stored is a. This method is a wrapper of the
     * same
     * method in HashTable.
     * @see HashTable::map.
     * @param a the default value assigned to each edge in the returned Property
     * @param size an optional parameter enabling to fine-tune the returned
     * Property. Roughly speaking, it is a good practice to have a size equal to
     * half the number of nodes. If you do not specify this parameter, the
     * method
     * will assign it for you. */
    template < typename VAL >
    NodeProperty< VAL > nodesProperty(const VAL& a, Size size = 0) const;

    /** @brief a method to create a list of VAL from a set of nodes
     * (using for every nodee, say x, the VAL f(x))
     * @param f a function assigning a VAL to any node */
    template < typename VAL >
    List< VAL > listMapNodes(VAL (*f)(const NodeId&)) const;

    /// @}

    private:
    friend class NodeGraphPartIterator;
    friend class NodeGraphPartIteratorSafe;

    /// to enable testunits to use  _check_

    friend class gum_tests::NodeGraphPartTestSuite;

    /// updating endIterator (always at  _max_+1)
    void _updateEndIteratorSafe_();

    /// code for clearing nodes (called twice)
    void _clearNodes_();

    /// to delete hole.
    /// @warning the hole is assumed to be existing.
    void _eraseHole_(NodeId id);

    /// to add a hole.
    /// @warning id is assumed not to be already a hole
    void _addHole_(NodeId id);

    // ############################################################################
    /// @name Introspection
    // ############################################################################
    /// @{

    /// @return true if id is part of  _holes_
    bool _inHoles_(NodeId id) const;

    /// @return the size of  _holes_
    Size _sizeHoles_() const;

    /// @}

    /** @brief the set of nodes not contained in the NodeGraphPart in the
     * interval 1.. _max_
     * @warning  _holes_ may be nullptr. */
    NodeSet* _holes_;

    /// value for  _holes_ configuration
    Size _holes_size_;

    /// value for  _holes_ configuration
    bool _holes_resize_policy_;

    /// the end iterator (used to speed-up parsings of the NodeGraphPart)
    NodeGraphPartIteratorSafe _endIteratorSafe_;

    /** @brief the id below which NodeIds may belong to the NodeGraphPart */
    NodeId _boundVal_;
  };

  /// for friendly displaying the content of node set
  std::ostream& operator<<(std::ostream&, const NodeGraphPart&);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/parts/nodeGraphPart_inl.h>
#endif   // GUM_NOINLINE

#include <agrum/tools/graphs/parts/nodeGraphPart_tpl.h>

#endif   // GUM_NODE_GRAPH_PART_H
