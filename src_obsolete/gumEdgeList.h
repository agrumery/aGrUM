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
 * @brief Classes for directed or undirected edge lists
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file contains classes providing support for all directed edge lists
 * (gumArcList) and undirected edge lists (gumEdgeList), as well as their iterators
 * (gumArcListIterator and gumEdgeListIterator). As usual in aGrUM,
 * gumEdgeListIterator and gumArcListIterator ensure that whenever an edge is
 * removed from an arc/edge list, the iterators pointing toward this element are
 * informed of this removal. Hence, trying to dereference such iterators will
 * throw an exception instead of resulting in a segmentation fault.
 *
 * Copying gumArcList and gumEdgeList through usual functions like operator= will
 * result in a completely fresh copy to be generated, i.e., the new edges are not
 * shared in memory with those copied. However, this would be inadequate in some
 * cases (for instance during junction tree creations) as this requires copying
 * whole objects. So, to optimize, an alternative copy procedure is available, the
 * so-called "shallow copy". In this mode, only pointers on edges/arcs are copied.
 * As a result, this creates a completely distinct new list containing pointers
 * toward the same edges/arcs as the original list. Thus, as both lists are
 * distinct, we can remove or add elements to one list without modifying the other
 * list. As the lists contain pointers, creating new lists is fast as we do not
 * need to create new edges/arcs. This kind of copy should be used with caution and
 * probably only for temporary usage. Once a gumArcList/gumEdgeList has been copied
 * through the shallowCopy member, the other shallow_ methods can be used. Using
 * the latter without having performed a shallow copy will usually produce
 * erroneous results (but will not crash the application).
 *
 * @par Usage example:
 * @code
 * // create three lists of edges
 * gumEdgeList<gumEdge> edges1, edges2, edges3;
 *
 * // insert elements into edges1
 * edges1.insert (gumEdge (2,3));
 * gumEdge edge (5,6);
 * edges1.insert (edge);
 * edges1.insertEdge (6,8);
 *
 * // copy edges1 into edges2 (deep copy)
 * edges2 = edges1;
 *
 * // copy edges1 into edges3 (shallow copy)
 * edges3.shallowCopy (edges1);
 *
 * // display the content of the lists
 * cerr << edges1 << edges2 << edges3 << endl;
 *
 * // remove some elements from edges1
 * edges1.erase (2,3);
 * edges1.eraseEdge (8,6);
 * edges1.eraseEdge (edge);
 *
 * // remove all the elements and check that the list is empty
 * edges1.clear ();
 * if (edges1.empty ()) cerr << "empty list" << endl;
 *
 * // checks whether a given edge exists
 * if (edges2.exists (edge) && edges2.existsEdge (edge) && edges2.exists (6,5))
 *   cerr << "list contains " << edge << gumEdge (6,5) << endl;
 *
 * // accessing the elements of a list
 * cerr << edges2(5,6) << edges2.get(6,8) << endl;
 *
 * // parsing an edge list
 * for (gumEdgeList<gumEdge>::iterator iter = edges2.begin();
 *        iter != edges2.end(); ++iter)
 *   cerr << *iter << endl;
 * for (gumEdgeList<gumEdge>::iterator iter = edges2.beginneighbours (6);
 *        iter != edges2.end(); ++iter)
 *   cerr << *iter << endl;
 * @endcode
 */
#ifndef GUM_EDGELIST_H
#define GUM_EDGELIST_H
#include <algorithm>
#include <utility>
#include <agrum/gum_utils.h>
#include <agrum/gumGraphElements.h>
#include <agrum/gumList.h>
#include <agrum/utils/gumHashTable.h>
#include <agrum/gumRefPtr.h>
// generic edge lists

template <typename Edge> class gumEdgeListBase; // base for directed/undirected

template <typename Edge> class gumEdgeList;     // undirected edges

template <typename Edge> class gumArcList;      // directed edges

template <typename Edge> class gumEdgeListIterator; // iterators for both
#define gumArcListIterator gumEdgeListIterator
// ================================================================================
/// a << operator for gumEdgeListBase
// ================================================================================
template <typename Edge>
std::ostream& operator<< ( std::ostream&, const gumEdgeListBase<Edge>& );
// ================================================================================
/// a << operator for gumEdgeList
// ================================================================================
template <typename Edge>
std::ostream& operator<< ( std::ostream&, const gumEdgeList<Edge>& );
// ================================================================================
/// a << operator for gumArcList
// ================================================================================
template <typename Arc>
std::ostream& operator<< ( std::ostream&, const gumArcList<Arc>& );
#ifndef DOXYGEN_SHOULD_SKIP_THIS
// gumEdgeList iterators function in two different mode : either they point
// to a hashtable or to a gumList. The modes below represent the different
// possibilities. GUM_EDGELIST_ITER_END_MODE corresponds to the end/rend
// iterators. they are special in the sense that they are defined in
// gumEdgeListBase for the set of all edges and they point both to the
// hashtable containing all the edges and to the lists of adjacent nodes
#define GUM_EDGELIST_ITER_ALL_MODE                 1
#define GUM_EDGELIST_ITER_ALL_DIRECTED_MODE        2
#define GUM_EDGELIST_ITER_ALL_UNDIRECTED_MODE      3
#define GUM_EDGELIST_ITER_ALL_MASK                 3
// below: only multiples of GUM_EDGELIST_ITER_ALL_MASK + 1 = 4
#define GUM_EDGELIST_ITER_ADJ_MODE                 4
#define GUM_EDGELIST_ITER_ADJ_PARENT_MODE          8
#define GUM_EDGELIST_ITER_ADJ_CHILD_MODE          12
#define GUM_EDGELIST_ITER_ADJ_mixedPARENT_MODE   16
#define GUM_EDGELIST_ITER_ADJ_mixedCHILD_MODE    20
#define GUM_EDGELIST_ITER_ADJ_mixedneighbour_MODE 24
#define GUM_EDGELIST_ITER_END_MODE                28
/// type used to type-safe construct iterators begin/rbegin/end/rend
enum gumEdgeListIteratorLocation {
	GUM_EDGELIST_ITER_BEGIN,
	GUM_EDGELIST_ITER_RBEGIN,
	GUM_EDGELIST_ITER_END,
	GUM_EDGELIST_ITER_REND
};
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
/* ============================================================================== */
/* ===              COMMON CLASS FOR ORIENTED AND UNORIENTED ARCS             === */
/* ============================================================================== */
/** @class gumEdgeListBase
 * @brief features common to both directed and undirected edges
 *
 * It mainly acts as an interface for both types of edges and implements the code
 * that they share. Developers should be aware (thanks, J.C. Van Damme) of this
 * class but should prefer using gumEdgeList or gumArcList.
 *
 * @par Usage example:
 * @code
 * // create three lists of edges
 * gumEdgeListBase<gumEdge> edges1, edges2, edges3;
 *
 * // insert elements into edges1
 * edges1.insert (gumEdge (2,3));
 * gumEdge edge (5,6);
 * edges1.insert (edge);
 * edges1.insert (6,8);
 *
 * // copy edges1 into edges2 (deep copy)
 * edges2 = edges1;
 *
 * // copy edges1 into edges3 (shallow copy)
 * edges3.shallowCopy (edges1);
 *
 * // display the content of the lists
 * cerr << edges1 << edges2 << edges3 << endl;
 *
 * // remove some elements from edges1
 * edges1.erase (2,3);
 * edges1.erase (8,6);
 * edges1.erase (edge);
 *
 * // remove all the elements and check that the list is empty
 * edges1.clear ();
 * if (edges1.empty ()) cerr << "empty list" << endl;
 *
 * // checks whether a given edge exists
 * if (edges2.exists (edge) && edges2.exists (6,5))
 *   cerr << "list contains " << edge << gumEdge (6,5) << endl;
 *
 * // accessing the elements of a list
 * cerr << edges2(5,6) << edges2.get(6,8) << endl;
 *
 * // parsing an edge list
 * for (gumEdgeListBase<gumEdge>::iterator iter = edges2.begin();
 *        iter != edges2.end(); ++iter)
 *   cerr << *iter << endl;
 * for (gumEdgeListBase<gumEdge>::iterator iter = edges2.beginneighbours (6);
 *        iter != edges2.end(); ++iter)
 *   cerr << *iter << endl;
 * @endcode
 */
/* ============================================================================== */

template <typename Edge> class gumEdgeListBase  {
	public:
		/// the iterators on gumEdgeListBase
		typedef gumEdgeListIterator<Edge> iterator;
		// ##############################################################################
		/// @name Iterators
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// returns an iterator pointing to the beginning of the list of edges
		// ==============================================================================
		iterator begin() const;
		// ==============================================================================
		/// returns an iterator pointing to the last element of the list of edges
		// ==============================================================================
		iterator rbegin() const;
		// ==============================================================================
		/// returns an iterator pointing just after the last element of the list of edges
		// ==============================================================================
		const iterator& end() const ;
		// ==============================================================================
		/// returns an iterator pointing just before the first element of the edge list
		// ==============================================================================
		const iterator& rend() const ;
		// ==============================================================================
		/// returns an iterator pointing to the first adjacent edge of a node
		// ==============================================================================
		virtual iterator beginneighbours( const gumId& node ) const = 0;
		// ==============================================================================
		/// returns an iterator pointing to the last adjacent edge of a node
		// ==============================================================================
		virtual iterator rbeginneighbours( const gumId& node ) const = 0;
		/// @}
		// ##############################################################################
		/// @name Fine tuning
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// changes the size of the hashtables within the edge list
		// ==============================================================================
		virtual void resize( gumSize new_size );
		// ==============================================================================
		/// changes the resizing policy of the hashtables contained in the edge list
		/** In most cases, this should be useless. However, when available memory
		 * becomes rare, avoiding automatic resizing may speed-up new insertions in
		 * the table. */
		// ==============================================================================
		virtual void setResizePolicy( const bool new_policy ) ;
		// ==============================================================================
		/// returns the current resizing policy
		// ==============================================================================
		const bool getResizePolicy() const ;
		// ==============================================================================
		/// returns the size of the hashtables within the edge list
		// ==============================================================================
		const gumSize& capacity() const ;
		/// @}
		// ##############################################################################
		/// @name Accessors / Modifiers
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// indicates whether the list is empty
		// ==============================================================================
		const bool empty() const ;
		// =============================================================================
		/// returns the edge in the list specified by its extremal nodes
		/** @throw gumNotFound exception is thrown if the element cannot
		 * be found. */
		// =============================================================================
		virtual Edge& get( const gumId&, const gumId& ) = 0;
		virtual const Edge& get( const gumId&, const gumId& ) const = 0;
		// =============================================================================
		/// checks whether an edge specified by its extremal nodes belongs to the list
		// =============================================================================
		virtual const bool exists( const gumId&, const gumId& ) const  = 0;
		// =============================================================================
		/// checks whether a given edge belongs to the list of edges
		// =============================================================================
		virtual const bool exists( const Edge& edge ) const  = 0;
		// ==============================================================================
		/// adds a copy of \e edge into the list of edges
		/** @return As a copy is inserted into the edge list instead of the edge itself,
		 * the method returns a reference on the copy created.
		 * @throw gumDuplicateElement exception is thrown if there already exists a
		 * similar edge in the list. */
		// ==============================================================================
		virtual Edge& insert( const Edge& edge );
		// ==============================================================================
		/// returns the number of edges in the list
		// ==============================================================================
		const gumSize size() const ;
		// ==============================================================================
		/// removes all the elements from the edge list
		// ==============================================================================
		virtual void clear();
		// ==============================================================================
		/// removes the given edge from the list of edges
		/** If the edge cannot be found, it does nothing (in particular, it throws no
		 * exception). */
		// ==============================================================================
		virtual void erase( const Edge& edge );
		// ==============================================================================
		/// removes an edge specified by its extremal nodes from the list of edges
		/** If the edge cannot be found, it does nothing (in particular, it throws no
		 * exception). */
		// ==============================================================================
		virtual void erase( const gumId& node1, const gumId& node2 ) = 0;
		// ==============================================================================
		/// removes the edges adjacent to a node
		// ==============================================================================
		virtual void eraseneighbours( const gumId& node );
		// ==============================================================================
		/// creates a hashtable of Border from an edge list
		/** @param f a function that maps an edge into a Border
		 * @param size the size of the resulting hashtable. When equal to 0, a default
		 * size is computed that is a good trade-off between space consumption and
		 * efficiency of new elements insertions
		 * @warning by default, the order of the edges in the resulting hashtable may
		 * not be similar to that of the edge list. Hence iterators on the former may
		 * not parse it in the same order as iterators on the latter. */
		// ==============================================================================
		template <typename Border> gumHashTable<std::pair<gumId,gumId>,Border>
		hashMap( Border( *f )( Edge ), gumSize size = 0 ) const;
		// ==============================================================================
		/// creates a hashtable of Border from an edge list
		/** @param f a function that maps an edge into a Border
		 * @param size the size of the resulting hashtable. When equal to 0, a default
		 * size is computed that is a good trade-off between space consumption and
		 * efficiency of new elements insertions
		 * @warning by default, the order of the edges in the resulting hashtable may
		 * not be similar to that of the edge list. Hence iterators on the former may
		 * not parse it in the same order as iterators on the latter. */
		// ==============================================================================
		template <typename Border> gumHashTable<std::pair<gumId,gumId>,Border>
		hashMap( Border( *f )( Edge& ), gumSize size = 0 ) const;
		// ==============================================================================
		/// creates a hashtable of Border from an edge list
		/** @param f a function that maps an edge into a Border
		 * @param size the size of the resulting hashtable. When equal to 0, a default
		 * size is computed that is a good trade-off between space consumption and
		 * efficiency of new elements insertions
		 * @warning by default, the order of the edges in the resulting hashtable may
		 * not be similar to that of the edge list. Hence iterators on the former may
		 * not parse it in the same order as iterators on the latter. */
		// ==============================================================================
		template <typename Border> gumHashTable<std::pair<gumId,gumId>,Border>
		hashMap( Border( *f )( const Edge& ), gumSize size = 0 ) const;
		// ==============================================================================
		/// creates a hashtable of Border from an edge list
		/** @param val the value taken by all the elements of the resulting list
		 * @param size the size of the resulting hashtable. When equal to 0, a default
		 * size is computed that is a good trade-off between space consumption and
		 * efficiency of new elements insertions
		 * @warning by default, the order of the edges in the resulting hashtable may
		 * not be similar to that of the edge list. Hence iterators on the former may
		 * not parse it in the same order as iterators on the latter. */
		// ==============================================================================
		template <typename Border> gumHashTable<std::pair<gumId,gumId>,Border>
		hashMap( const Border& val, gumSize size = 0 ) const;
		// ==============================================================================
		/// creates a list of Border from an edge list
		/** @param f a function that maps an edge into a Border
		 * @warning the order of the Border elements in the resulting list is
		 * arbitrary */
		// ==============================================================================
		template <typename Border> gumList<Border>
		listMap( Border( *f )( Edge ) ) const;
		// ==============================================================================
		/// creates a list of Border from an edge list
		/** @param f a function that maps an edge into a Border
		 * @warning the order of the Border elements in the resulting list is
		 * arbitrary */
		// ==============================================================================
		template <typename Border> gumList<Border>
		listMap( Border( *f )( Edge& ) ) const;
		// ==============================================================================
		/// creates a list of Border from an edge list
		/** @param f a function that maps an edge into a Border
		 * @warning the order of the Border elements in the resulting list is
		 * arbitrary */
		// ==============================================================================
		template <typename Border> gumList<Border>
		listMap( Border( *f )( const Edge& ) ) const;
		// ==============================================================================
		/// creates a list of Border from an edge list
		/** @param val the value taken by all the elements of the resulting list
		 * @warning the order of the Border elements in the resulting list is
		 * arbitrary */
		// ==============================================================================
		template <typename Border> gumList<Border> listMap( const Border& val ) const;
		/// @}
		// ##############################################################################
		/// @name Operators
		// ##############################################################################
		/// @{
		// =============================================================================
		/// returns the edge in the list specified by its extremal nodes
		/**
		* @throw gumNotFound
		*/
		// =============================================================================
		virtual Edge& operator()( const gumId&, const gumId& )
		= 0;
		virtual const Edge& operator()( const gumId&, const gumId& )
		const  = 0;
		// =============================================================================
		/// checks whether two lists of edges are similar
		// =============================================================================
		const bool operator== ( const gumEdgeListBase<Edge>& from ) const;
		// =============================================================================
		/// checks whether two lists of edges contain different elements
		// =============================================================================
		const bool operator!= ( const gumEdgeListBase<Edge>& from ) const;
		/// @}
		// ##############################################################################
		/// @name Shallow members
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// copy operator (the new list shares its edges with \e from)
		// ==============================================================================
		gumEdgeListBase<Edge>& shallowCopy( const gumEdgeListBase<Edge>& from );
		// ==============================================================================
		/// checks whether two lists of edges are similar
		// ==============================================================================
		const bool shallowEqual( const gumEdgeListBase<Edge>& from ) const ;
		// ==============================================================================
		/// checks whether two lists of edges contain different elements
		// ==============================================================================
		const bool shallowDifferent( const gumEdgeListBase<Edge>& from ) const ;
		// ==============================================================================
		/// adds a shallow copy of an edge into the edge list
		/** @throw gumDuplicateElement exception is thrown if there already exists a
		 * similar edge in the list. */
		// ==============================================================================
		virtual const gumRefPtr<Edge>& shallowInsert( const gumRefPtr<Edge>& edge );
		// ==============================================================================
		/// returns a pointer to a given edge (allows shallow copies)
		/** @throw gumNotFound exception is thrown if the edge cannot be found */
		// ==============================================================================
		virtual const gumRefPtr<Edge>& shallowGet( const gumId& node1, const gumId& node2 )
		const  = 0;
		// ==============================================================================
		/// returns a pointer to a given edge (allows shallow copies)
		/** @throw gumNotFound exception is thrown if the edge cannot be found */
		// ==============================================================================
		virtual const gumRefPtr<Edge>& shallowGet( const Edge& edge )
		const  = 0;
		/// @}
	private:
		/// the set of (directed or undirected) edges
		gumHashTable< std::pair<gumId,gumId>,gumRefPtr<Edge> > edges;
		/// @name pseudo static iterators
		/// @{
		/** the end and rend iterators are constructed only once per edge list
		 * so as to optimize for(iter = begin();iter != end(); ++iter) loops: this
		 * will avoid creating objects end and rend each time we pass in the loop. */
		/* WARNING: the end and rend iterators MUST be declared after the hash table.
		 * This ensures that their construction will take place after the hash table
		 * has been constructed. Hence they can rely directly on the initialized
		 * members of the hash table. */
		iterator iter_end;
		iterator iter_rend;
		/// @}
		/// gumEdgeListIterator must be a friend to optimize access time

		friend class gumEdgeListIterator<Edge>;
		/// friends that can construct this class

		friend class gumEdgeList<Edge>;

		friend class gumArcList<Edge>;
		// ##############################################################################
		/// @name Constructors / Destructors
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// Default constructor
		/** @param nb_edges the size of the hash table used to store all the edges
		 * @param resize_policy the resizing policy of this hash table */
		// ==============================================================================
		gumEdgeListBase( const gumSize nb_edges = GUM_HASHTABLE_DEFAULT_SIZE,
		                 bool resize_policy = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY );
		// ==============================================================================
		/// Copy constructor (by default deep copy)
		/** This copy constructor has two different behaviors, depending on the value of
		 * parameter copy_type. If it is equal to GUM_DEEP_COPY (the default), a
		 * deep copy is performed, that is, new Edge instances are created and inserted
		 * into the newly created gumEdgeListBase. Thus, the latter and the copied
		 * gumEdgeListBase (from) are completely distinct in terms of memory locations.
		 * If, on the contrary, copy_type is equal to GUM_SHALLOW_COPY, only Edge
		 * pointers are copied. As a result, all the edges in the new gumEdgeListBase are
		 * shared with those of \e from. Thus, modifying the content of an edge in one
		 * list will also modify it in the other list. However, both lists are distinct in
		 * the sense that removing an edge from one list does not remove it from the
		 * other one. */
		// ==============================================================================
		gumEdgeListBase( const gumEdgeListBase<Edge>& from,
		                 gumCopyType copy_type = GUM_DEEP_COPY );
		// ==============================================================================
		/// copy operator (deep copy)
		/** The copy performed is a deep one, i.e., new Edge instances are created and
		 * inserted into the newly created gumEdgeListBase. Thus, the latter and the
		 * copied gumEdgeListBase (from) are completely distinct in terms of memory
		 * locations. */
		// ==============================================================================
		gumEdgeListBase<Edge>& operator= ( const gumEdgeListBase<Edge>& from );
		// ==============================================================================
		/// destructor
		// ==============================================================================
		virtual ~gumEdgeListBase();
		/// @}
};

/* ============================================================================== */
/* ===                         UNDIRECTED EDGES LISTS                         === */
/* ============================================================================== */
/** @class gumEdgeList
 * @brief Basic class for undirected edge lists
 *
 * gumEdgeList is an efficient container for undirected edges. It should be used
 * rather than gumList or gumHashTables when you want to store undirected edges
 * as this container allows fast access to edges defined by their extremities as
 * well as those defined as the neighbours of a given node. If you want to store
 * arcs (i.e., asymmetric edges), prefer structure gumArcList. Indeed, gumEdgeList
 * are defined to make no difference between edge (x,y) and edge (y,x). Thus using
 * method get() to retrieve edge (x,y) when the gumEdgeList only contains edge (y,x)
 * will result in edge (y,x) to be returned. Similarly, after inserting edge (x,y)
 * into a gumEdgeList, trying to insert edge (y,x) will fail as gumEdgeList being
 * designed to store symmetric edges, it will think edge (y,x) is already belonging
 * to the gumEdgeList. As usual in aGrUM, gumEdgeList<>::Iterator ensure that
 * whenever an edge is removed from a gumEdgeList, the iterators pointing toward
 * this element are informed of this removal. Hence, trying to dereference such
 * iterators will throw an exception instead of resulting in a segmentation fault.
 *
 * Copying gumEdgeList through usual functions like operator= will result in a
 * completely fresh copy to be generated, i.e., the new edges are not shared in
 * memory with those copied. However, this would be inadequate in some cases (for
 * instance during junction tree creations) as this requires copying whole objects.
 * So, to optimize, an alternative copy procedure is available, the so-called
 * "shallow copy". In this mode, only pointers on edges are copied. As a result,
 * this creates a completely distinct new list containing pointers toward the same
 * edges as the original list. Thus, as both lists are distinct, we can remove or
 * add elements to one list without modifying the other list. As the lists contain
 * pointers, creating new lists is fast as we do not need to create new edges. This
 * kind of copy should be used with caution and probably only for temporary usage.
 * Once a gumEdgeList has been copied through the shallowCopy member, the other
 * shallow_ methods can be used. Using the latter without having performed a
 * shallow copy will usually produce erroneous results (but will not crash the
 * application).
 *
 * @par Usage example:
 * @code
 * // create three lists of edges
 * gumEdgeList<gumEdge> edges1, edges2, edges3;
 *
 * // insert elements into edges1
 * edges1.insert (gumEdge (2,3));
 * gumEdge edge (5,6);
 * edges1.insert (edge);
 * edges1.insertEdge (6,8);
 *
 * // copy edges1 into edges2 (deep copy)
 * edges2 = edges1;
 *
 * // copy edges1 into edges3 (shallow copy)
 * edges3.shallowCopy (edges1);
 *
 * // display the content of the lists
 * cerr << edges1 << edges2 << edges3 << endl;
 *
 * // remove some elements from edges1
 * edges1.erase (2,3);
 * edges1.eraseEdge (8,6);
 * edges1.eraseEdge (edge);
 *
 * // remove all the elements and check that the list is empty
 * edges1.clear ();
 * if (edges1.empty ()) cerr << "empty list" << endl;
 *
 * // checks whether a given edge exists
 * if (edges2.exists (edge) && edges2.existsEdge (edge) && edges2.exists (6,5))
 *   cerr << "list contains " << edge << gumEdge (6,5) << endl;
 *
 * // accessing the elements of a list
 * cerr << edges2(5,6) << edges2.get(6,8) << endl;
 *
 * // parsing an edge list
 * for (gumEdgeList<gumEdge>::iterator iter = edges2.begin();
 *        iter != edges2.end(); ++iter)
 *   cerr << *iter << endl;
 * for (gumEdgeList<gumEdge>::iterator iter = edges2.beginneighbours (6);
 *        iter != edges2.end(); ++iter)
 *   cerr << *iter << endl;
 * @endcode
 */
/* ============================================================================== */
template <typename Edge>

class gumEdgeList : public gumEdgeListBase<Edge> {
	public:
		/// iterators for lists of undirected edges
		typedef gumEdgeListIterator<Edge> iterator;
		// ##############################################################################
		/// @name Constructors / Destructors
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// Default constructor
		/** @param nb_edges the size of the hash table containing all the edges
		 * @param resize_policy the policy for resizing the hashtable \sa gumHashTable */
		// ==============================================================================
		gumEdgeList( gumSize nb_edges = GUM_HASHTABLE_DEFAULT_SIZE,
		             bool resize_policy = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY );
		// ==============================================================================
		/// copy constructor (by default deep copy)
		/**  This copy constructor has two different behaviors, depending on the value of
		 * parameter copy_type. If it is equal to GUM_DEEP_COPY (the default), a
		 * deep copy is performed, that is, new Edge instances are created and inserted
		 * into the newly created gumEdgeList. Thus, the latter and the copied
		 * gumEdgeList (from) are completely distinct in terms of memory locations.
		 * If, on the contrary, copy_type is equal to GUM_SHALLOW_COPY, only Edge
		 * pointers are copied. As a result, all the edges in the new gumEdgeList are
		 * shared with those of \e from. Thus, modifying the content of an edge in one
		 * list will also modify it in the other list. However, both lists are distinct in
		 * the sense that removing an edge from one list does not remove it from the
		 * other one.
		 * @param from the edge list we wish to copy
		 * @param copy_type type of copy (deep copy = default, else GUM_SHALLOW_COPY) */
		// ==============================================================================
		gumEdgeList( const gumEdgeList<Edge>& from, gumCopyType copy_type=GUM_DEEP_COPY );
		// ==============================================================================
		/// destructor
		/** Note that all the iterators still pointing to the current list are detached
		 * properly before the list is deleted. */
		// ==============================================================================
		virtual ~gumEdgeList();
		/// @}
		// ##############################################################################
		/// @name Shallow members
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// copy operator (the new list shares its edges with \e from)
		// ==============================================================================
		gumEdgeList<Edge>& shallowCopy( const gumEdgeList<Edge>& from );
		// ==============================================================================
		/// checks whether two lists of edges are similar
		// ==============================================================================
		const bool shallowEqual( const gumEdgeList<Edge>& from ) const ;
		// ==============================================================================
		/// checks whether two lists of edges contain different elements
		// ==============================================================================
		const bool shallowDifferent( const gumEdgeList<Edge>& from ) const ;
		// ==============================================================================
		/// adds a shallow copy of an edge into the edge list
		// ==============================================================================
		const gumRefPtr<Edge>& shallowInsert( const gumRefPtr<Edge>& edge );
		// ==============================================================================
		/// returns a smart pointer on the edge the extremities of which are node1,node2
		/** @throw gumNotFound exception is thrown if the edge cannot be found */
		// ==============================================================================
		const gumRefPtr<Edge>& shallowGet( const gumId& node1, const gumId& node2 )
		const ;
		// ==============================================================================
		/// returns a pointer to a given edge (allows shallow copies)
		/** @throw gumNotFound exception is thrown if the edge cannot be found */
		// ==============================================================================
		const gumRefPtr<Edge>& shallowGet( const Edge& edge )
		const ;
		// ==============================================================================
		/// alias for shallowInsert
		// ==============================================================================
		const gumRefPtr<Edge>& shallowInsertEdge( const gumRefPtr<Edge>& edge );
		// ==============================================================================
		/// alias for shallowGet
		// ==============================================================================
		const gumRefPtr<Edge>& shallowGetEdge( const gumId& node1, const gumId& node2 )
		const ;
		// ==============================================================================
		/// alias for shallowGet
		// ==============================================================================
		const gumRefPtr<Edge>& shallowGetEdge( const Edge& edge )
		const ;
		/// @}
		// ##############################################################################
		/// @name Fine tuning
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// changes the resizing policy of the hashtables contained in the edge list
		/** In most cases, this should be useless. However, when available memory
		 * becomes rare, avoiding automatic resizing may speed-up new insertions in
		 * the table. */
		// ==============================================================================
		void setResizePolicy( const bool new_policy ) ;
		// ==============================================================================
		/// changes the size of the hashtables within the edge list
		// ==============================================================================
		void resize( gumSize new_size );
		/// @}
		// ##############################################################################
		/// @name Accessors / Modifiers
		// ##############################################################################
		/// @{
		using gumEdgeListBase<Edge>::hashMap;
		using gumEdgeListBase<Edge>::listMap;
		// ==============================================================================
		/// creates a gumEdgeList of Border from a gumEdgeList of Edge
		/** @param f a function that maps an edge into a Border edge.
		 * @param size the size of the hashtable resulting edgelist. When equal to 0,
		 * a default size is computed that is a good trade-off between space consumption
		 * and efficiency of new elements insertions
		 * @warning by default, the order of the edges in the resulting edgelist may
		 * not be similar to that of the edge list passed in argument. Hence iterators
		 * on the former may not parse it in the same order as iterators on the latter. */
		// ==============================================================================
		template <typename Border> gumEdgeList<Border>
		map( Border( *f )( Edge ), gumSize size = 0 ) const;
		template <typename Border> gumEdgeList<Border>
		map( Border( *f )( Edge& ), gumSize size = 0 ) const;
		template <typename Border> gumEdgeList<Border>
		map( Border( *f )( const Edge& ), gumSize size = 0 ) const;
		// ==============================================================================
		/// returns the undirected edge in the list specified by its extremal nodes
		/** The latter may be specified in any order.
		 * @throw gumNotFound exception is thrown if the edge cannot be found */
		// ==============================================================================
		Edge& get( const gumId& node1, const gumId& node2 ) ;
		const Edge& get( const gumId& node1, const gumId& node2 )
		const ;
		// ==============================================================================
		/** @brief checks whether an undirected edge specified by its extremal
		 * nodes does belong to the list of edges
		 *
		 * The order in which the nodes are given is unimportant. */
		// ==============================================================================
		const bool exists( const gumId& node1, const gumId& node2 ) const ;
		const bool exists( const Edge& ) const ;
		// ==============================================================================
		/// add a copy of \e edge, an undirected edge, to the list of edges
		/** @return As this is a copy that is inserted into the list, the method
		 * returns a reference to this new copy.
		 * @throw gumDuplicateElement exception is thrown if the list already contains
		 * an edge with the same extremities. */
		// ==============================================================================
		Edge& insert( const Edge& edge );
		// ==============================================================================
		/// removes an edge specified by its extremal nodes from the list of edges
		/** The order in which the nodes are specified is unimportant.
		 * The iterators that pointed on that edge are resetted to rend.
		 * If the edge cannot be found, the function does nothing (in particular, it does
		 * not throw any exception). */
		// ==============================================================================
		void erase( const gumId& node1, const gumId& node2 );
		// ==============================================================================
		/// removes a given edge from the list of edges
		/** The iterators that pointed on that edge are resetted to rend.
		 * If the edge cannot be found, the function does nothing (in particular, it does
		 * not throw any exception). */
		// ==============================================================================
		void erase( const Edge& edge );
		// ==============================================================================
		/// removes all the edges of the gumEdgeList
		// ==============================================================================
		void clear();
		// ==============================================================================
		/// returns the list of the edges adjacent to a given node
		/** If the node has no neighbours, the function returns an empty list. */
		// ==============================================================================
		const gumList<gumRefPtr<Edge> >& getneighbours( const gumId& node )
		const ;
		// ==============================================================================
		/// removes all the edges adjacent to the given node
		// ==============================================================================
		void eraseneighbours( const gumId& node );
		// ==============================================================================
		/* to maintain some coherency between the names of the edgeLists methods and
		 * those of arcLists, we shall add the following methods. */
		// ==============================================================================
		/// alias for method get
		// ==============================================================================
		Edge& getEdge( const gumId& node1, const gumId& node2 ) ;
		const Edge& getEdge( const gumId& node1, const gumId& node2 )
		const ;
		// ==============================================================================
		/// alias for method getneighbours
		// ==============================================================================
		const gumList<gumRefPtr<Edge> >& getneighbourEdges( const gumId& node )
		const ;
		// ==============================================================================
		/// alias for method exists
		// ==============================================================================
		const bool existsEdge( const Edge& edge ) const ;
		// ==============================================================================
		/// alias for method exists
		// ==============================================================================
		const bool existsEdge( const gumId& node1, const gumId& node2 ) const ;
		// ==============================================================================
		/// alias for method insert
		// ==============================================================================
		Edge& insertEdge( const Edge& edge );
		// ==============================================================================
		/// alias for method erase
		// ==============================================================================
		void eraseEdge( const Edge& edge );
		// ==============================================================================
		/// alias for method erase
		// ==============================================================================
		void eraseEdge( const gumId& node1, const gumId& node2 );
		/// @}
		// ##############################################################################
		/// @name Operators
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// copy operator (deep copy)
		/** The copy performed is a deep one, i.e., new Edge instances are created and
		 * inserted into the newly created gumEdgeList. Thus, the latter and the copied
		 * gumEdgeList (from) are completely distinct in terms of memory locations. */
		// ==============================================================================
		gumEdgeList<Edge>& operator= ( const gumEdgeList<Edge>& from );
		// ==============================================================================
		/// returns the undirected edge in the list specified by its extremal nodes
		/** The latter may be specified in any order.
		 * @throw gumNotFound exception is thrown if the edge cannot be found */
		// ==============================================================================
		Edge& operator()( const gumId& node1, const gumId& node2 );
		const Edge& operator()( const gumId& node1, const gumId& node2 )
		const ;
		// =============================================================================
		/// checks whether two lists of edges are similar
		// =============================================================================
		const bool operator== ( const gumEdgeList<Edge>& from ) const;
		// =============================================================================
		/// checks whether two lists of edges contain different elements
		// =============================================================================
		const bool operator!= ( const gumEdgeList<Edge>& from ) const;
		/// @}
		// ##############################################################################
		/// @name Iterators
		// ##############################################################################
		/// @{
		/* iterators on the set of all edges */
		using gumEdgeListBase<Edge>::begin;
		using gumEdgeListBase<Edge>::rbegin;
		using gumEdgeListBase<Edge>::end;
		using gumEdgeListBase<Edge>::rend;
		// ==============================================================================
		/// returns an iterator pointing to the first neighbour of a given node
		// ==============================================================================
		iterator beginneighbours( const gumId& node ) const;
		// ==============================================================================
		/// returns an iterator pointing to the last neighbour of a given node */
		// ==============================================================================
		iterator rbeginneighbours( const gumId& node ) const;
		// ==============================================================================
		/// alias for iterator begin
		// ==============================================================================
		iterator  beginEdges() const;
		// ==============================================================================
		/// alias for iterator rbegin
		// ==============================================================================
		iterator  rbeginEdges() const;
		// ==============================================================================
		/// alias for iterator end
		// ==============================================================================
		const iterator& endEdges() const;
		// ==============================================================================
		/// alias for iterator rend
		// ==============================================================================
		const iterator& rendEdges() const;
		// ==============================================================================
		/// alias for iterator beginneighbours
		// ==============================================================================
		iterator beginneighbourEdges( const gumId& node ) const;
		// ==============================================================================
		/// alias for iterator rbeginneighbours
		// ==============================================================================
		iterator rbeginneighbourEdges( const gumId& node ) const;
		/// @}
	private:
		/// the list of the sets of edges adjacent to a given node
		gumHashTable< gumId,gumList<gumRefPtr<Edge> > > neighbours;
		/// the neighbour's iterators list (shared by all neighbours)
		gumRefPtr< gumListBase<gumListIterator< gumRefPtr<Edge> >*> > neighbours_iter_list;
		/// an empty list that is used to initialize the neighbours of new nodes
		/** WARNING: this member should be declared after neighbours_iter_list as it relies
		 * on the latter in the initialization lists */
		gumList< gumRefPtr<Edge> > empty_neighbours;
};

/* ============================================================================== */
/* ===                          DIRECTED EDGES LISTS                          === */
/* ============================================================================== */
/** @class gumArcList
 * @brief Basic class for directed edge lists
 *
 * gumArcList is an efficient container for directed edges. It should be used
 * rather than gumList or gumHashTables when you want to store directed edges
 * as this container allows fast access to edges defined by their extremities as
 * well as those defined as the parents or children of a given node. If you want to
 * store undirected edges (i.e., symmetric edges), prefer structure gumEdgeList.
 * Indeed, gumArcList are defined to make a difference between arc (x,y) and
 * arc (y,x). Thus using method get() to retrieve arc (x,y) when the gumArcList
 * only contains arc (y,x) will result in an exception to be thrown as arc (y,x)
 * is different from arc (x,y). As usual in aGrUM, gumArcList<>::Iterator ensure
 * that whenever an arc is removed from a gumArcList, the iterators pointing toward
 * this element are informed of this removal. Hence, trying to dereference such
 * iterators will throw an exception instead of resulting in a segmentation fault.
 *
 * Copying gumArcList through usual functions like operator= will result in a
 * completely fresh copy to be generated, i.e., the new arcs are not shared in
 * memory with those copied. However, this would be inadequate in some cases (for
 * instance during junction tree creations) as this requires copying whole objects.
 * So, to optimize, an alternative copy procedure is available, the so-called
 * "shallow copy". In this mode, only pointers on arcs are copied. As a result,
 * this creates a completely distinct new list containing pointers toward the same
 * arcs as the original list. Thus, as both lists are distinct, we can remove or
 * add elements to one list without modifying the other list. As the lists contain
 * pointers, creating new lists is fast as we do not need to create new arcs. This
 * kind of copy should be used with caution and probably only for temporary usage.
 * Once a gumArcList has been copied through the shallowCopy member, the other
 * shallow_ methods can be used. Using the latter without having performed a
 * shallow copy will usually produce erroneous results (but will not crash the
 * application).
 *
 * @par Usage example:
 * @code
 * // create three lists of directed edges
 * gumArcList<gumArc> arcs1, arcs2, arcs3;
 *
 * // insert elements into arcs1
 * arcs1.insert (gumArc (2,3));
 * gumArc arc (5,6);
 * arcs1.insert (arc);
 * arcs1.insertArc (6,8);
 *
 * // copy arcs1 into arcs2 (deep copy)
 * arcs2 = arcs1;
 *
 * // copy arcs1 into arcs3 (shallow copy)
 * arcs3.shallowCopy (arcs1);
 *
 * // display the content of the lists
 * cerr << arcs1 << arcs2 << arcs3 << endl;
 *
 * // remove some elements from arcs1
 * arcs1.erase (2,3);
 * arcs1.eraseArc (6,8);
 * arcs1.eraseArc (arc);
 *
 * // remove all the elements and check that the list is empty
 * arcs1.clear ();
 * if (arcs1.empty ()) cerr << "empty list" << endl;
 *
 * // checks whether a given arc exists
 * if (arcs2.exists (arc) && arcs2.existsArc (arc) && arcs2.exists (5,6))
 *   cerr << "list contains " << arc << gumArc (5,6) << endl;
 *
 * // accessing the elements of a list
 * cerr << arcs2(5,6) << arcs2.get(6,8) << endl;
 *
 * // parsing an arc list
 * for (gumArcList<gumArc>::iterator iter = arcs2.begin();
 *        iter != arcs2.end(); ++iter)
 *   cerr << *iter << endl;
 * for (gumArcList<gumArc>::iterator iter = edges2.beginParents (6);
 *        iter != edges2.end(); ++iter)
 *   cerr << *iter << endl;
 * for (gumArcList<gumArc>::iterator iter = edges2.beginChildren (6);
 *        iter != edges2.end(); ++iter)
 *   cerr << *iter << endl;
 * for (gumArcList<gumArc>::iterator iter = edges2.beginneighbours (6);
 *        iter != edges2.end(); ++iter)
 *   cerr << *iter << endl;
 * @endcode
 */
/* ============================================================================== */

template <typename Arc> class gumArcList : public gumEdgeListBase<Arc> {
	public:
		/// iterators for lists of directed edges
		typedef gumEdgeListIterator<Arc> iterator;
		// ##############################################################################
		/// @name Constructors / Destructors
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// Default constructor
		/** @param nb_edges the size of the hash table containing all the arcs
		 * @param resize_policy the policy for resizing the hashtable \sa gumHashTable */
		// ==============================================================================
		gumArcList( gumSize nb_edges = GUM_HASHTABLE_DEFAULT_SIZE,
		            bool resize_policy = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY );
		// ==============================================================================
		/// copy constructor (by default deep copy)
		/** This copy constructor has two different behaviors, depending on the value of
		 * parameter copy_type. If it is equal to GUM_DEEP_COPY (the default), a
		 * deep copy is performed, that is, new Arc instances are created and inserted
		 * into the newly created gumArcList. Thus, the latter and the copied
		 * gumArcList (from) are completely distinct in terms of memory locations.
		 * If, on the contrary, copy_type is equal to GUM_SHALLOW_COPY, only Arc
		 * pointers are copied. As a result, all the edges in the new gumArcList are
		 * shared with those of \e from. Thus, modifying the content of an edge in one
		 * list will also modify it in the other list. However, both lists are distinct in
		 * the sense that removing an edge from one list does not remove it from the
		 * other one.
		 * @param from the edge list we wish to copy
		 * @param copy_type type of copy (deep copy = default, else GUM_SHALLOW_COPY) */
		// ==============================================================================
		gumArcList( const gumArcList<Arc>& from, gumCopyType copy_type = GUM_DEEP_COPY );
		// ==============================================================================
		/// destructor
		/** Note that all the iterators still pointing to the current list are detached
		 * properly before the list is deleted. */
		// ==============================================================================
		virtual ~gumArcList();
		/// @}
		// ##############################################################################
		/// @name Shallow members
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// copy operator (the new list shares its arcs with \e from)
		// ==============================================================================
		gumArcList<Arc>& shallowCopy( const gumArcList<Arc>& from );
		// ==============================================================================
		/// checks whether two lists of arcs are similar
		// ==============================================================================
		const bool shallowEqual( const gumArcList<Arc>& from ) const ;
		// ==============================================================================
		/// checks whether two lists of arcs contain different elements
		// ==============================================================================
		const bool shallowDifferent( const gumArcList<Arc>& from ) const ;
		// ==============================================================================
		/// adds a shallow copy of an edge into the edge list
		/** @throw gumDuplicateElement exception is thrown if there already exists a
		 * similar edge in the list. */
		// ==============================================================================
		const gumRefPtr<Arc>& shallowInsert( const gumRefPtr<Arc>& arc );
		// ==============================================================================
		/// returns a smart pointer on the arc the extremities of which are node1,node2
		/** @throw gumNotFound exception is thrown if the arc cannot be found */
		// ==============================================================================
		const gumRefPtr<Arc>& shallowGet( const gumId& node1, const gumId& node2 )
		const ;
		// ==============================================================================
		/// returns a pointer to a given arc (allows shallow copies)
		/** @throw gumNotFound exception is thrown if the arc cannot be found */
		// ==============================================================================
		const gumRefPtr<Arc>& shallowGet( const Arc& arc ) const ;
		// ==============================================================================
		/// alias for shallowInsert
		// ==============================================================================
		const gumRefPtr<Arc>& shallowinsertArc( const gumRefPtr<Arc>& arc );
		// ==============================================================================
		/// alias for shallowGet
		/**
		* @throw gumNotFound
		*/
		// ==============================================================================
		const gumRefPtr<Arc>& shallowGetArc( const gumId& node1, const gumId& node2 )
		const ;
		// ==============================================================================
		/// alias for shallowGet
		/**
		* @throw gumNotFound
		*/
		// ==============================================================================
		const gumRefPtr<Arc>& shallowGetArc( const Arc& arc )
		const ;
		/// @}
		// ##############################################################################
		/// @name Iterators
		// ##############################################################################
		/// @{
		/* iterators on the set of all edges */
		using gumEdgeListBase<Arc>::begin;
		using gumEdgeListBase<Arc>::rbegin;
		using gumEdgeListBase<Arc>::end;
		using gumEdgeListBase<Arc>::rend;
		// ==============================================================================
		/// returns an iterator pointing to the first parent of a given node
		// ==============================================================================
		iterator beginParents( const gumId& node ) const;
		// ==============================================================================
		/// returns an iterator pointing to the last parent of a given node
		// ==============================================================================
		iterator rbeginParents( const gumId& node ) const;
		// ==============================================================================
		/// returns an iterator pointing to the first child of a given node
		// ==============================================================================
		iterator beginChildren( const gumId& node ) const;
		// ==============================================================================
		/// returns an iterator pointing to the last parent of a given node
		// ==============================================================================
		iterator rbeginChildren( const gumId& node ) const;
		// ==============================================================================
		/// returns an iterator pointing to the first adjacent edge of a node
		/** Actually, this corresponds to the first parent. */
		// ==============================================================================
		iterator beginneighbours( const gumId& node ) const;
		// ==============================================================================
		/// returns an iterator pointing to the last adjacent edge of a node
		/** Actually, this corresponds to the last of its children */
		// ==============================================================================
		iterator rbeginneighbours( const gumId& node ) const;
		// ==============================================================================
		/// alias for iterator begin
		// ==============================================================================
		iterator beginArcs() const;
		// ==============================================================================
		/// alias for iterator rbegin
		// ==============================================================================
		iterator  rbeginArcs() const;
		// ==============================================================================
		/// alias for iterator end
		// ==============================================================================
		const iterator& endArcs() const;
		// ==============================================================================
		/// alias for iterator rend
		// ==============================================================================
		const iterator& rendArcs() const;
		// ==============================================================================
		/// alias for iterator beginneighbours
		// ==============================================================================
		iterator beginneighbourArcs( const gumId& node ) const;
		// ==============================================================================
		/// alias for iterator rbeginneighbours
		// ==============================================================================
		iterator rbeginneighbourArcs( const gumId& node ) const;
		/// @}
		// ##############################################################################
		/// @name Fine tuning
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// changes the size of the hashtables within the arc list
		// ==============================================================================
		void resize( gumSize new_size );
		// ==============================================================================
		/// changes the resizing policy of the hashtables contained in the arc list
		/** In most cases, this should be useless. However, when available memory
		 * becomes rare, avoiding automatic resizing may speed-up new insertions in
		 * the table. */
		// ==============================================================================
		void setResizePolicy( const bool new_policy ) ;
		/// @}
		// ##############################################################################
		/// @name Accessors / Modifiers
		// ##############################################################################
		/// @{
		/* members of base classes are treated as unqualified names, which implies that
		   we cannot use them directly within derived classes. To be able to use them,
		   we shall add the following 'using' directives */
		using gumEdgeListBase<Arc>::hashMap;
		using gumEdgeListBase<Arc>::listMap;
		// ==============================================================================
		/// creates a gumArcList of Curve from a gumArcList of Arc
		/** @param f a function that maps an arc into a Curve.
		 * @param size the size of the hashtable resulting arclist. When equal to 0,
		 * a default size is computed that is a good trade-off between space consumption
		 * and efficiency of new elements insertions
		 * @warning by default, the order of the arcs in the resulting arclist may
		 * not be similar to that of the arc's list passed in argument. Hence iterators
		 * on the former may not parse it in the same order as iterators on the latter. */
		// ==============================================================================
		template <typename Curve> gumArcList<Curve>
		map( Curve( *f )( Arc ), gumSize size = 0 ) const;
		template <typename Curve> gumArcList<Curve>
		map( Curve( *f )( Arc& ), gumSize size = 0 ) const;
		template <typename Curve> gumArcList<Curve>
		map( Curve( *f )( const Arc& ), gumSize size = 0 ) const;
		// ==============================================================================
		/// returns the arc in the list specified by its tail and head
		/** @throw gumNotFound exception is thrown if the arc cannot be found. */
		// ==============================================================================
		Arc& get( const gumId& node1, const gumId& node2 ) ;
		const Arc& get( const gumId& node1, const gumId& node2 )
		const ;
		// ==============================================================================
		/// adds a copy of \e edge to the list of arcs.
		/** @return As this is a copy that is inserted into the list, the method
		 * returns a reference to this new copy
		 * @throw gumDuplicateElement exception is thrown if the list already contains
		 * an arc with the same head and tail. */
		// ==============================================================================
		Arc& insert( const Arc& edge );
		// ==============================================================================
		/** @brief check whether an arc specified by its tail and head does belong
		 * to the list of arcs. */
		// ==============================================================================
		const bool exists( const gumId& node1, const gumId& node2 ) const ;
		const bool exists( const Arc& edge ) const ;
		// ==============================================================================
		/// removes an arc specified by its tail and head from the list of edges
		/** The iterators that pointed on the removed edge are resetted to rend.
		 * If the edge cannot be found, the function does nothing (in particular, it
		 * does not throw any exception). */
		// ==============================================================================
		void erase( const gumId& node1, const gumId& node2 );
		// ==============================================================================
		/// removes a given arc from the list of edges
		/** The iterators that pointed on the removed edge are resetted to rend.
		 * If the edge cannot be found, the function does nothing (in particular, it
		 * does not throw any exception). */
		// ==============================================================================
		void erase( const Arc& edge );
		// ==============================================================================
		/// removes all the arcs adjacent the given node
		// ==============================================================================
		void eraseneighbours( const gumId& node );
		// ==============================================================================
		/// removes all the arcs outgoing from the given node
		// ==============================================================================
		void eraseChildren( const gumId& node );
		// ==============================================================================
		/// removes all the arcs ingoing into the given node
		// ==============================================================================
		void eraseParents( const gumId& node );
		// ==============================================================================
		/// removes all the arcs from the gumArcList
		// ==============================================================================
		void clear();
		// ==============================================================================
		/// returns the list of the edges out-going from a given node
		/** If the node has no children, the function returns an empty list. */
		// ==============================================================================
		const gumList<gumRefPtr<Arc> >& getChildren( const gumId& node ) const ;
		// ==============================================================================
		/// returns the list of the edges in-going to a given node
		/** If the node has no parent, the function returns an empty list. */
		// ==============================================================================
		const gumList<gumRefPtr<Arc> >& getParents( const gumId& node ) const ;
		// ==============================================================================
		/* to maintain some coherency between the names of the edgeLists methods and
		   those of arcLists, we shall add the following methods. */
		// ==============================================================================
		/// alias for method getEdge
		/**
		* @throw gumNotFound
		*/
		// ==============================================================================
		Arc& getArc( const gumId& node1, const gumId& node2 ) ;
		const Arc& getArc( const gumId& node1, const gumId& node2 )
		const ;
		// ==============================================================================
		/// alias for method exists
		// ==============================================================================
		const bool existsArc( const Arc& edge ) const ;
		// ==============================================================================
		/// alias for method exists
		// ==============================================================================
		const bool existsArc( const gumId& node1, const gumId& node2 ) const ;
		// ==============================================================================
		/// alias for method insert
		// ==============================================================================
		Arc& insertArc( const Arc& edge );
		// ==============================================================================
		/// alias for method erase
		// ==============================================================================
		void eraseArc( const Arc& edge );
		// ==============================================================================
		/// alias for method erase
		// ==============================================================================
		void eraseArc( const gumId& node1, const gumId& node2 );
		// ==============================================================================
		/// alias for eraseChildren
		// ==============================================================================
		void eraseChildrenArcs( const gumId& node );
		// ==============================================================================
		/// alias for eraseParents
		// ==============================================================================
		void eraseParentArcs( const gumId& node );
		/// @}
		// ##############################################################################
		/// @name Operators
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// copy operator (deep copy)
		/** The copy performed is a deep one, i.e., new Arc instances are created and
		 * inserted into the newly created gumArcList. Thus, the latter and the copied
		 * gumArcList (from) are completely distinct in terms of memory locations. */
		// ==============================================================================
		gumArcList<Arc>& operator= ( const gumArcList<Arc>& from );
		// ==============================================================================
		/// returns the arc in the list specified by its tail and head
		/** @throw gumNotFound exception is thrown if the arc cannot be found. */
		// ==============================================================================
		Arc& operator()( const gumId& node1, const gumId& node2 )
		;
		const Arc& operator()( const gumId& node1, const gumId& node2 )
		const ;
		// =============================================================================
		/// checks whether two lists of edges are similar
		// =============================================================================
		const bool operator== ( const gumArcList<Arc>& from ) const;
		// =============================================================================
		/// checks whether two lists of edges contain different elements
		// =============================================================================
		const bool operator!= ( const gumArcList<Arc>& from ) const;
		/// @}
	private:
		/// the list of the sets of parents of a given node
		gumHashTable<gumId,gumList<gumRefPtr<Arc> > > parents;
		/// the list of the sets of children of a given node
		gumHashTable<gumId,gumList<gumRefPtr<Arc> > > children;
		/// the neighbour's iterators list (shared for all neighbours)
		gumRefPtr< gumListBase<gumListIterator< gumRefPtr<Arc> >*> > adjacent_iter_list;
		/// an empty list that is used to initialize the neighbours of new nodes
		/** WARNING: this member should be declared after neighbours_iter_list as it relies
		 * on the latter in the initialization lists */
		gumList< gumRefPtr<Arc> > empty_neighbours;
		/// iterators should access gumArcLists to update their parents/children fields

		friend class gumEdgeListIterator<Arc>;
};

/* ============================================================================== */
/* ===                        GENERIC EDGE LIST ITERATORS                     === */
/* ============================================================================== */
/** @class gumEdgeListIterator
 * @brief Iterators on gumEdgeLists and gumArcLists
 *
 * As usual in aGrUM, gumEdgeListIterator and gumArcListIterator ensure that
 * whenever an edge is removed from an arc/edge list, the iterators pointing toward
 * this element are informed of this removal. Hence, trying to dereference such
 * iterators will throw an exception instead of resulting in a segmentation fault.
 *
 * @par Usage example:
 * @code
 * // parsing an edge list
 * for (gumEdgeList<gumEdge>::iterator iter = edges.begin();
 *        iter != edges.end(); ++iter)
 *   cerr << *iter << endl;
 * for (gumEdgeList<gumEdge>::iterator iter = edges.beginneighbours (6);
 *        iter != edges.end(); ++iter)
 *   cerr << *iter << endl;
 * @endcode
 */
/* ============================================================================== */

template <typename Edge> class gumEdgeListIterator {
	public:
		// ##############################################################################
		/// @name Constructors / Destructors
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// default constructor : the iterator points on nothing
		// ==============================================================================
		gumEdgeListIterator() ;
		// ==============================================================================
		/// copy constructor
		// ==============================================================================
		gumEdgeListIterator( const gumEdgeListIterator<Edge>& from );
		// ==============================================================================
		/// destructor
		// ==============================================================================
		virtual ~gumEdgeListIterator();
		/// @}
		// ##############################################################################
		/// @name Operators
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// copy iterator
		// ==============================================================================
		gumEdgeListIterator<Edge>& operator= ( const gumEdgeListIterator<Edge>& from );
		// ==============================================================================
		/// makes the iterator point to the next element in the edge list
		/** when the iterator is used to parse the neighbours of a node and it is currently
		 * pointing on the last parent, ++ returns the first child. */
		// ==============================================================================
		gumEdgeListIterator<Edge>& operator++() ;
		// ==============================================================================
		/// makes the iterator point to the preceding element in the edge list
		/** when the iterator is used to parse the neighbours of a node and it is currently
		 * pointing on the last child, -- returns the first parent. */
		// ==============================================================================
		gumEdgeListIterator<Edge>& operator--() ;
		// ==============================================================================
		/// checks whether two edge list iterators point to the same edge
		// ==============================================================================
		const bool operator==( const gumEdgeListIterator<Edge>& from ) const ;
		// ==============================================================================
		/// checks whether two edge list iterators point to different edges
		// ==============================================================================
		const bool operator!=( const gumEdgeListIterator<Edge>& from ) const ;
		// ==============================================================================
		/// gives access to the edge pointed to by the iterator
		// ==============================================================================
		Edge& operator*();
		const Edge& operator*() const;
		// ==============================================================================
		/// dereference the edge pointed to by the iterator
		// ==============================================================================
		Edge* operator-> () const;
		/// @}
		// ##############################################################################
		/// @name Accessors / Modifiers
		// ##############################################################################
		/// @{
		// ==============================================================================
		/// detach the iterator from its current gumEdgeList
		// ==============================================================================
		void clear() ;
		/// @}
	private:
		/// an iterator on the set of all the edges
		gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> > *iter_all;
		/** @brief indicates whether iter_all (above) belongs to this or to the hashtable
		 * (is_end = true means that it belongs to the hashtable) */
		bool is_end;
		/// an iterator on edges adjacent to a given node (neighbours, parents or children)
		gumListIterator< gumRefPtr<Edge> > iter_adj;
		/** @brief gumEdgeLists can be used in 2 different modes. In the first one, i.e.
		 * GUM_EDGELIST_ITER_ALL, they can parse all the edges of a graph. In the
		 * second mode, i.e. GUM_EDGELIST_ITER_ADJ, they parse only the edges adjacent
		 * to a given node. */
		int mode;
		/// for iterating over the adjacent arcs/edges of a node
		const gumList<gumRefPtr<Edge> >* parents;
		const gumList<gumRefPtr<Edge> >* children;
		const gumList<gumRefPtr<Edge> >* neighbours;
		/// node is used to update parents/children members when appropriate
		/** when an iterator over parents (resp. children) of a node is set, the
		 * children (resp. parents) set of the latter may be empty. However when the
		 * set of parents (resp. children) has been parsed, it may be the case that the
		 * set of children (resp. parents) may not be empty anymore. In this case, if
		 * we wish to iterate over these "new" neighbours, we should first update the
		 * parent/children members. To do so, we need to keep track of the id of the node.
		 * hence the following member. */
		gumId nodeID;
		/// to use member node, we must access the gumArcList the iterator points to
		const gumArcList<Edge>* arcList;
		/* class gumEdgeList must be a friend because it stores iterators end and
		 * rend, and those should be initialized using a protected constructor: indeed,
		 * the hashtable on the edges should be one of the arguments of this constructor
		 * and such implementation detail should be hidden from the user */

		friend class gumEdgeListBase<Edge>;

		friend class gumEdgeList<Edge>;

		friend class gumArcList<Edge>;
		// ==============================================================================
		/** creates an iterator for the begin (forward=1), rbegin (forward=2),
		 * end (forward = 3) and rend (forward = 4) iterators stored into the EdgeLists,
		 * ArcLists. These enable parsing the whole set of either directed or
		 * undirected edges of a graph.
		 * This function should a priori only be used during the construction of
		 * gumEdgeList and gumArcList instances. */
		// ==============================================================================
		gumEdgeListIterator
		( const gumHashTable< std::pair<gumId,gumId>,gumRefPtr<Edge> >& edges,
		  gumEdgeListIteratorLocation forward );
		// ==============================================================================
		/** creates an iterator for a list of adjacent undirected edges. Depending
		 * on 'forward', we create a begin (forward = 1), rbegin(forward = 2),
		 * end (forward = 3) or rend (forward = 4) iterator. */
		// ==============================================================================
		gumEdgeListIterator( const gumList<gumRefPtr<Edge> >& edges,
		                     gumEdgeListIteratorLocation forward );
		// ==============================================================================
		/** creates an iterator for a list of adjacent arcs. Depending on 'forward', we
		 * create a begin (forward = 1), rbegin(forward = 2), end (forward = 3) or
		 * rend (forward = 4) iterator. */
		// ==============================================================================
		gumEdgeListIterator( const gumList<gumRefPtr<Edge> >* par,
		                     const gumList<gumRefPtr<Edge> >* child,
		                     const gumId& node,
		                     const gumArcList<Edge>* list,
		                     gumEdgeListIteratorLocation forward );
		// ==============================================================================
		/// sets an iterator to end/rend (intended for iter_end/iter_rend)
		// ==============================================================================
		void setIteratorEnd( const gumHashTable<std::pair<gumId,gumId>,gumRefPtr<Edge> >&,
		                     gumEdgeListIteratorLocation );
};

// always include the .tcc as it contains only templates
#include "gumEdgeList.tcc"
#endif /* GUM_EDGELIST_H */
