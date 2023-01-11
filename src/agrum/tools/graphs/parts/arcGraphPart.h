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


#ifndef GUM_ARC_GRAPH_PART_H
#define GUM_ARC_GRAPH_PART_H

#include <algorithm>
#include <utility>

#include <agrum/agrum.h>

#include <agrum/tools/core/signal/signalers.h>
#include <agrum/tools/graphs/graphElements.h>

namespace gum {

  /** @class ArcGraphPart
   * @brief Classes for directed edge sets
   *
   * \ingroup graph_group
   *
   * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
   *
   * @par Usage example:
   * @code
   * ArcGraphPart arcs1,arcs2,arcs3;
   *
   * // insert elements into arcs1
   * arcs1.addArc( 2,3 );
   * arcs1.addArc( 5,3 );
   *
   * // copy arcs1 into arcs2
   * arcs2=arcs1;
   *
   * // remove some elements from arcs1
   * arcs1.eraseArc( Arc( 5,3 ) );
   * arcs1.eraseArc( arc );
   *
   * if ( arcs1.empty() ) std::cerr<<" arcs1 is empty"<<std::endl;
   *
   * // checks whether a given arc exists
   * if ( arcs2.existArc( arc ) )
   *   cerr << "set contains " << arc << endl;
   *
   * if ( arcs2.existArc( 5,3 ) )
   *   cerr << "set contains " << arc << endl;
   *
   * std::cerr<<arcs2.toString()<<std::endl;
   *
   * std::cerr<<arcs2.parents( 3 )<<std::endl;
   *
   * std::cerr<<arcs2.children( 2 )<<std::endl;
   *
   * std::cerr<<std::endl<<std::endl;
   *
   * std::cerr<<std::endl<<std::endl;
   * @endcode
   */

  class ArcGraphPart {
    public:
    using ArcIterator = ArcSetIterator;

    Signaler2< NodeId, NodeId > onArcAdded;     // onArcAdded(tail,head)
    Signaler2< NodeId, NodeId > onArcDeleted;   // onArcDeleted(tail,head)

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @param arcs_size the size of the hash table used to store all the arcs
     * @param arcs_resize_policy the resizing policy of this hash table*/
    explicit ArcGraphPart(Size arcs_size          = HashTableConst::default_size,
                          bool arcs_resize_policy = true);

    /// copy constructor
    /** @param s the ArcGraphPart to copy */
    ArcGraphPart(const ArcGraphPart& s);

    /// destructor
    virtual ~ArcGraphPart();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param s the ArcGraphPart to copy */
    ArcGraphPart& operator=(const ArcGraphPart& s);

    /// tests whether two ArcGraphParts contain the same arcs
    /** @param p the ArcGraphPart that we compare with this */
    bool operator==(const ArcGraphPart& p) const;
    /// @}

    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{

    /// insert a new arc into the ArcGraphPart
    /** @param tail the id of the tail of the new arc to be inserted
     * @param head the id of the head of the new arc to be inserted
     * @warning if the arc already exists, nothing is done. In particular, no
     * exception is raised. */
    virtual void addArc(NodeId tail, NodeId head);

    /// removes an arc from the ArcGraphPart
    /** @param arc the arc to be removed
     * @warning if the arc does not exist, nothing is done. In particular, no
     * exception is thrown. However, the signal onArcDeleted is fired
     * only if a node is effectively removed. */
    virtual void eraseArc(const Arc& arc);

    /// indicates whether a given arc exists
    /** @param arc the arc we test whether or not it belongs to the ArcGraphPart
     */
    bool existsArc(const Arc& arc) const;

    /// indicates whether a given arc exists
    /** @param tail the tail of the arc we test the existence in the
     * ArcGraphPart
     * @param head the head of the arc we test the existence in the ArcGraphPart
     */
    bool existsArc(NodeId tail, NodeId head) const;

    /// indicates wether the ArcGraphPart contains any arc
    bool emptyArcs() const;

    /// removes all the arcs from the ArcGraphPart
    void clearArcs();

    /// indicates the number of arcs stored within the ArcGraphPart
    Size sizeArcs() const;

    /// returns the set of arcs stored within the ArcGraphPart
    const ArcSet& arcs() const;

    /// returns the set of nodes with arc ingoing to a given node
    /** Note that the set of arcs returned may be empty if no arc within the
     * ArcGraphPart is ingoing into the given node.
     * @param id the node toward which the arcs returned are pointing */
    const NodeSet& parents(NodeId id) const;

    /// returns the set of nodes which consists in the node and its parents
    /** Note that the set of nodes returned may be empty if no path within the
     * ArcGraphPart is outgoing from the given node.
     * @param id the node which is the tail of a directed path with the returned
     * nodes
     **/
    NodeSet family(NodeId id) const;

    /// returns the set of nodes with directed path outgoing from a given node
    /** Note that the set of nodes returned may be empty if no path within the
     * ArcGraphPart is outgoing from the given node.
     * @param id the node which is the tail of a directed path with the returned
     * nodes
     **/
    NodeSet descendants(NodeId id) const;


    /// returns the set of nodes with directed path ingoing to a given node
    /** Note that the set of nodes returned may be empty if no path within the
     * ArcGraphPart is ingoing to the given node.
     * @param id the node which is the head of a directed path with the returned
     * nodes
     **/
    NodeSet ancestors(NodeId id) const;

    /// returns the set of children of a set of nodes
    NodeSet children(const NodeSet& ids) const;

    /// returns the set of parents of a set of nodes
    NodeSet parents(const NodeSet& ids) const;

    /// returns the set of family nodes of a set of nodes
    NodeSet family(const NodeSet& ids) const;


    /// returns the set of nodes with arc outgoing from a given node
    /** Note that the set of arcs returned may be empty if no arc within the
     * ArcGraphPart is outgoing from the given node.
     * @param id the node which is the tail of the arcs returned */
    const NodeSet& children(NodeId id) const;

    /// erase all the parents of a given node
    /** @param id the node all the parents of which will be removed
     * @warning although this method is not virtual, it calls method
     * eraseArc( const Arc& arc ) and, as such, has a "virtual" behaviour. If
     * you do not wish it to have this "virtual" behaviour, call instead
     * method @ref unvirtualizedEraseParents
     * @warning if no arc is a parent of id, nothing is done. In particular, no
     * exception is thrown. */
    void eraseParents(NodeId id);

    /// same function as eraseParents but without any virtual call to an erase
    /** @param id the node whose ingoing arcs will be removed */
    void unvirtualizedEraseParents(NodeId id);

    /// removes all the children of a given node
    /** @param id the node all the children of which will be removed
     * @warning although this method is not virtual, it calls method
     * eraseArc( const Arc& arc ) and, as such, has a "virtual" behaviour. If
     * you do not wish it to have this "virtual" behaviour, call instead
     * method @ref unvirtualizedEraseChildren
     * @warning if no arc is a parent of id, nothing is done. In particular, no
     * exception is thrown. */
    void eraseChildren(NodeId id);

    /// same function as eraseChildren but without any virtual call to an erase
    /** @param id the node whose outgoing arcs will be removed */
    void unvirtualizedEraseChildren(NodeId id);

    /// to friendly display the content of the ArcGraphPart
    std::string toString() const;

    /** @brief a method to create a hashMap of VAL from a set of arcs
     * (using for every arc, say x, the VAL f(x))
     * @param f a function assigning a VAL to any arc
     * @param size an optional parameter enabling to fine-tune the returned
     * Property. Roughly speaking, it is a good practice to have a size equal to
     * half the number of arcs. If you do not specify this parameter, the method
     * will assign it for you. */
    template < typename VAL >
    ArcProperty< VAL > arcsProperty(VAL (*f)(const Arc&), Size size = 0) const;

    /** @brief a method to create a hashMap of VAL from a set of arcs
     * (using for every arc, say x, the VAL a)
     * @param a the default value assigned to each arc in the returned Property
     * @param size an optional parameter enabling to fine-tune the returned
     * Property. Roughly speaking, it is a good practice to have a size equal to
     * half the number of arcs. If you do not specify this parameter, the method
     * will assign it for you. */
    template < typename VAL >
    ArcProperty< VAL > arcsProperty(const VAL& a, Size size = 0) const;

    /** @brief a method to create a list of VAL from a set of arcs
     * (using for every arc, say x, the VAL f(x))
     * @param f a function assigning a VAL to any arc */
    template < typename VAL >
    List< VAL > listMapArcs(VAL (*f)(const Arc&)) const;

    /// returns a directed path from node1 to node2 belonging to the set of arcs
    /** @param node1 the id from which the path begins
     * @param node2 the id to which the path ends
     * @throw NotFound exception is raised if no path can be found between the
     * two nodes */
    std::vector< NodeId > directedPath(NodeId node1, NodeId node2) const;

    /// returns an unoriented (directed) path from node1 to node2 in the arc set
    /** @param node1 the id from which the path begins
     * @param node2 the id to which the path ends
     * @throw NotFound exception is raised if no path can be found between the
     * two nodes */
    std::vector< NodeId > directedUnorientedPath(NodeId node1, NodeId node2) const;

    /// @}

    protected:
    /// a (virtualized) function to remove a given set of arcs
    /** @warning this function uses eraseArc, which is a virtual function. Hence
     * the behaviour of this function is that of a virtual function */
    void eraseSetOfArcs_(const ArcSet& set);

    /// similar to @ref eraseSetOfArcs_ except that it is unvirtualized
    /** @warning this function uses ArcGraphPart::eraseArc, hence, as compared
     * with eraseSetOfArcs_, it removes the arcs without calling a virtual
     * eraseArc */
    void unvirtualizedEraseSetOfArcs_(const ArcSet& set);

    private:
    /// the set of all the arcs contained within the ArcGraphPart
    Set< Arc > _arcs_;

    /// for each arc, the sets of its parents
    NodeProperty< NodeSet* > _parents_;

    /// for each arc, the set of its children
    NodeProperty< NodeSet* > _children_;

    /** @brief when the ArcGraphPart contains no arc ingoing into a given node,
     * this function adds an empty set entry to  _parents_[id]
     * @param id the node whose  _parents_[id] is checked */
    void _checkParents_(NodeId id);

    /** @brief when the ArcGraphPart contains no arc outgoing from a given node,
     * this function adds an empty set entry to  _children_[id]
     * @param id the node whose  _children_[id] is checked */
    void _checkChildren_(NodeId id);
  };

  /// for friendly displaying the content of arc set
  /** @param s the stream to which we display the content of a
   * @param a the ArcGraphPart to be displayed */
  std::ostream& operator<<(std::ostream& s, const ArcGraphPart& a);

} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/graphs/parts/arcGraphPart_inl.h>
#endif   // GUM_NOINLINE

#include <agrum/tools/graphs/parts/arcGraphPart_tpl.h>

#endif   // GUM_ARC_GRAPH_PART_H
