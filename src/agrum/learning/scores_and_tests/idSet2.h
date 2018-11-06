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
 * @brief A class used by learning caches to represent uniquely sets of variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_ID_SET2_H
#define GUM_LEARNING_ID_SET2_H

#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/core/sequence.h>
#include <agrum/graphs/graphElements.h>

namespace gum {

  namespace learning {


    template < template < typename > class ALLOC >
    class IdSet2;


    /** @class IdSet2Iterator
     * @brief The iterators for IdSets
     * @headerfile idSet2.h <agrum/learning/scores_and_tests/idSet2.h>
     * @ingroup learning_scores
     */
    template < template < typename > class ALLOC = std::allocator >
    class IdSet2Iterator {
      public:
      /// types for STL compliance
      /// @{
      using iterator_category = std::forward_iterator_tag;
      using value_type = NodeId;
      using reference = NodeId&;
      using const_reference = const NodeId&;
      using pointer = NodeId*;
      using const_pointer = const NodeId*;
      using difference_type = std::ptrdiff_t;
      /// @}


      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @return an iterator pointing toward nothing. */
      IdSet2Iterator();

      /// Constructor for a begin
      /** @param idset The IdSet to iterate over. */
      IdSet2Iterator(const IdSet2< ALLOC >& idset);

      /// Copy constructor.
      IdSet2Iterator(const IdSet2Iterator< ALLOC >& from);

      /// move constructor
      IdSet2Iterator(IdSet2Iterator< ALLOC >&& from);

      /// destructor
      virtual ~IdSet2Iterator();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      IdSet2Iterator< ALLOC >& operator=(const IdSet2Iterator< ALLOC >& from);

      /// move operator
      IdSet2Iterator< ALLOC >& operator=(IdSet2Iterator< ALLOC >&& from);

      /** @brief Gives access to the content of the iterator.
       * @throw UndefinedIteratorValue Raised if the iterator points to nothing.
       * @return Returns the content of the iterator.
       */
      NodeId operator*() const;

      /// Checks whether two iterators point toward different elements.
      bool operator!=(const IdSet2Iterator< ALLOC >& from) const;

      /// Checks whether two iterators point toward the same elements.
      bool operator==(const IdSet2Iterator< ALLOC >& from) const;

      /** @brief Makes the iterator point to the next element in the IdSet
       *
       * @code
       * for (iter = idset.begin(); iter != idset.end(); ++iter) {}
       * @endcode
       *
       * The above loop is guaranteed to parse the whole IdSet as long as no
       * element is added to or deleted from the IdSet while being in the loop.
       *
       * @return Returns this gum::IdSet2Iterator.
       */
      IdSet2Iterator< ALLOC >& operator++();

      /**
       * @brief Makes the iterator point to i elements further in the IdSet
       * @param i The number of steps to move the iterator.
       * @return Returns this gum::IdSet2Iterator.
       */
      IdSet2Iterator< ALLOC >& operator+=(const std::size_t i);

      /**
       * @brief Returns a new iterator pointing to i further elements in the IdSet
       * @param i The number of steps to move the iterator.
       * @return Returns a new gum::IdSet2Iterator.
       */
      IdSet2Iterator< ALLOC > operator+(const std::size_t i);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /**
       * @brief Returns the position of the iterator in the IdSet2.
       * @return Returns the position of the iterator in the IdSet2.
       * @throw UndefinedIteratorValue Raised on end()
       */
      std::size_t pos() const;

      /// @}

      private:
      /// a pointer on the sequence stored in the idSet2
      const Sequence< NodeId, ALLOC< NodeId > >* __seq{nullptr};

      /// The index in the IdSet2's sequence where the iterator is pointing.
      std::size_t __index{std::size_t(0)};


      /// places the index to the end of the sequence
      void __gotoEnd();

      friend class IdSet2< ALLOC >;
    };


    /** @class IdSet2
     * @brief A class for storing a pair of sets of NodeIds, the second one
     * corresponding to a conditional set.
     * @headerfile idSet2.h <agrum/learning/scores_and_tests/idSet2.h>
     * @ingroup learning_scores
     *
     * IdSet2s are used by learning caches to store pairs of sets of nodes,
     * the second ones represent typically conditional sets.
     * This is useful for storing in hashtables the scores assigned to sets
     * of nodes given other nodes. NodeSets are actually not well suited for
     * this purpose because their implementations makes the computation of their
     * hash values quite difficult. IdSet2s fix this issue.
     */
    template < template < typename > class ALLOC = std::allocator >
    class IdSet2 : private ALLOC< NodeId > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;

      using iterator = IdSet2Iterator< ALLOC >;
      using const_iterator = IdSet2Iterator< ALLOC >;
      using iterator_safe = IdSet2Iterator< ALLOC >;
      using const_iterator_safe = IdSet2Iterator< ALLOC >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      IdSet2(const allocator_type& alloc = allocator_type());

      /// default constructor with no variable on the left side
      /** @param ids the set of variables
       * @param rhs_ids indicate whether the ids are on the right side of the
       * conditioning bar or not
       * @param ordered_ids indicates whether the ids in rhs_ids should be
       * considered as an ordered set or an unordered set
       * @param alloc the allocator used to store the data in the IdSet */
      IdSet2(const std::vector< NodeId, ALLOC< NodeId > >& ids,
             const bool                                    rhs_ids,
             const bool                                    ordered_ids,
             const allocator_type& alloc = allocator_type());

      /// default constructor with one variable on the left side
      /** @param var1 the variable on the left side of the conditioning bar
       * @param rhs_ids the set of variables on the right side of the
       * conditioning bar
       * @param ordered_rhs_ids indicates whether the ids in rhs_ids should be
       * considered as an ordered set or an unordered set
       * @param alloc the allocator used to store the data in the IdSet */
      IdSet2(NodeId                                        var1,
             const std::vector< NodeId, ALLOC< NodeId > >& rhs_ids,
             const bool                                    ordered_rhs_ids = false,
             const allocator_type& alloc = allocator_type());

      /// default constructor with two variables on the left side
      /** @param var1 the 1st variable on the left side of the conditioning bar
       * @param var2 the 2nd variable on the left side of the conditioning bar
       * @param rhs_ids the set of variables on the right side of the
       * conditioning bar
       * @param ordered_lhs_vars a Boolean indicating whether {var1,var2} should
       * be considered as an ordered set or not. Typically, in an independence
       * test, this set is unordered. When set to false,
       * @param ordered_rhs_ids indicates whether the ids in rhs_ids should be
       * considered as an ordered set or an unordered set
       * @param ordered_rhs_ids
       * @param alloc the allocator used to store the data in the IdSet */
      IdSet2(NodeId                                        var1,
             NodeId                                        var2,
             const std::vector< NodeId, ALLOC< NodeId > >& rhs_ids,
             const bool                                    ordered_lhs_vars,
             const bool                                    ordered_rhs_ids = false,
             const allocator_type& alloc = allocator_type());

      /// default constructor with three variables on the left side
      /** @param var1 the 1st variable on the left side of the conditioning bar
       * @param var2 the 2nd variable on the left side of the conditioning bar
       * @param var3 the 3rd variable on the left side of the conditioning bar
       * @param rhs_ids the set of variables on the right side of the
       * conditioning bar
       * @param ordered_vars a Boolean indicating whether {var1,var2,var3}
       * should be considered as an ordered set or not.
       * @param ordered_rhs_ids indicates whether the ids in rhs_ids should be
       * considered as an ordered set or an unordered set
       * @param alloc the allocator used to store the data in the IdSet */
      IdSet2(NodeId                                        var1,
             NodeId                                        var2,
             NodeId                                        var3,
             const std::vector< NodeId, ALLOC< NodeId > >& rhs_ids,
             const bool                                    ordered_lhs_vars,
             const bool                                    ordered_rhs_ids = false,
             const allocator_type& alloc = allocator_type());

      /// copy constructor
      IdSet2(const IdSet2< ALLOC >& from);

      /// copy constructor with a given allocator
      IdSet2(const IdSet2< ALLOC >& from, const allocator_type& alloc);

      /// move constructor
      IdSet2(IdSet2< ALLOC >&& from);

      /// move constructor with a given allocator
      IdSet2(IdSet2< ALLOC >&& from, const allocator_type& alloc);

      /// virtual copy constructor
      virtual IdSet2< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual IdSet2< ALLOC >* clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~IdSet2();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      IdSet2< ALLOC >& operator=(const IdSet2< ALLOC >& from);

      /// move operator
      IdSet2< ALLOC >& operator=(IdSet2< ALLOC >&& from);

      /// returns the node id stored at a given index
      NodeId operator[](const std::size_t index) const;

      /// returns true if both sets are equal
      bool operator==(const IdSet2< ALLOC >& from) const;

      /// returns true if the sets differ
      bool operator!=(const IdSet2< ALLOC >& from) const;

      /// @}


      // ##########################################################################
      /// @name Iterators
      // ##########################################################################
      /// @{

      /**
       * @brief Returns a safe begin iterator.
       * @return Returns a safe begin iterator.
       */
      iterator_safe beginSafe() const;

      /**
       * @brief Returns the safe end iterator.
       * @return Returns the safe end iterator.
       */
      const iterator_safe& endSafe() const;

      /**
       * @brief Returns an unsafe begin iterator.
       * @return Returns an unsafe begin iterator.
       */
      iterator begin() const;

      /**
       * @brief Returns the unsafe end iterator.
       * @return Returns the unsafe end iterator.
       */
      const iterator& end() const;

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the set of ids
      const Sequence< NodeId, ALLOC< NodeId > >& ids() const;

      /// returns the idSet at the right hand side of the conditioning bar
      IdSet2< ALLOC > conditionalIdSet() const;

      /// returns the number of left hand side ids
      std::size_t nbLHSIds() const;

      /// indicates whether the IdSet contains the IdSet passed in argument
      bool contains(const IdSet2< ALLOC >& set) const;

      /// removes all the nodes from the IdSet
      void clear();

      /// returns the number of variables (both left and right hand side)
      std::size_t size() const;

      /// returns the position of a given node in the IdSet
      /** @throw NotFound Raised if the element does not exist. */
      std::size_t pos(const NodeId id) const;

      /// indicates whether a given id is contained in the IdSet
      bool exists(const NodeId id) const;

      /// erase a node in the idset
      /** If the element cannot be found, the function does nothing. In
       * particular, it throws no exception. */
      void erase(const NodeId id);

      /// indicates whether the idset contains a non-empty conditioning set
      bool hasConditioningSet() const;

      /// indicates whether the IdSet contains some nodes or not
      bool empty() const;

      /// returns the content of the set as a string
      std::string toString() const;

      /// returns the allocator used
      allocator_type getAllocator() const;

      /// @}

      private:
      /// the ordered set of ids on the right side of the conditioning bar
      Sequence< NodeId, ALLOC< NodeId > > __ids;

      /// the number of left ids
      std::size_t __nb_lhs_ids{std::size_t(0)};

      /// Stores the end iterator for fast access.
      IdSet2Iterator< ALLOC > __end_safe;
    };


    /// the display operator
    template < template < typename > class ALLOC >
    std::ostream& operator<<(std::ostream& stream, const IdSet2< ALLOC >& idset);

  } /* namespace learning */


  /// the hash function for idSets
  template < template < typename > class ALLOC >
  class HashFunc< learning::IdSet2< ALLOC > >
      : public HashFuncBase< learning::IdSet2< ALLOC > > {
    public:
    /// computes the hashed value of a key
    Size operator()(const learning::IdSet2< ALLOC >& key) const;
  };


} /* namespace gum */


// always include the template implementation
#include <agrum/learning/scores_and_tests/idSet2_tpl.h>

#endif /* GUM_LEARNING_ID_SET2_H */
