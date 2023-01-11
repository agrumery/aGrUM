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
 * @brief A class used by learning caches to represent uniquely sets of variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_ID_SET_H
#define GUM_LEARNING_ID_SET_H

#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/core/sequence.h>
#include <agrum/tools/graphs/graphElements.h>

namespace gum {

  namespace learning {


    class IdCondSet;


    /** @class IdCondSetIterator
     * @brief The iterators for IdSets
     * @headerfile idCondSet.h <agrum/BN/learning/scores_and_tests/idSet.h>
     * @ingroup learning_scores
     */
    class IdCondSetIterator {
      public:
      /// types for STL compliance
      /// @{
      using iterator_category = std::forward_iterator_tag;
      using value_type        = NodeId;
      using reference         = NodeId&;
      using const_reference   = const NodeId&;
      using pointer           = NodeId*;
      using const_pointer     = const NodeId*;
      using difference_type   = std::ptrdiff_t;
      /// @}


      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @return an iterator pointing toward nothing. */
      IdCondSetIterator();

      /// Constructor for a begin
      /** @param idset The IdCondSet to iterate over. */
      IdCondSetIterator(const IdCondSet& idset);

      /// Copy constructor.
      IdCondSetIterator(const IdCondSetIterator& from);

      /// move constructor
      IdCondSetIterator(IdCondSetIterator&& from);

      /// destructor
      virtual ~IdCondSetIterator();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      IdCondSetIterator& operator=(const IdCondSetIterator& from);

      /// move operator
      IdCondSetIterator& operator=(IdCondSetIterator&& from);

      /** @brief Gives access to the content of the iterator.
       * @throw UndefinedIteratorValue Raised if the iterator points to nothing.
       * @return Returns the content of the iterator.
       */
      NodeId operator*() const;

      /// Checks whether two iterators point toward different elements.
      bool operator!=(const IdCondSetIterator& from) const;

      /// Checks whether two iterators point toward the same elements.
      bool operator==(const IdCondSetIterator& from) const;

      /** @brief Makes the iterator point to the next element in the IdCondSet
       *
       * @code
       * for (iter = idset.begin(); iter != idset.end(); ++iter) {}
       * @endcode
       *
       * The above loop is guaranteed to parse the whole IdCondSet as long as no
       * element is added to or deleted from the IdCondSet while being in the loop.
       *
       * @return Returns this gum::IdCondSetIterator.
       */
      IdCondSetIterator& operator++();

      /**
       * @brief Makes the iterator point to i elements further in the IdCondSet
       * @param i The number of steps to move the iterator.
       * @return Returns this gum::IdCondSetIterator.
       */
      IdCondSetIterator& operator+=(const std::size_t i);

      /**
       * @brief Returns a new iterator pointing to i further elements in the
       * IdCondSet
       * @param i The number of steps to move the iterator.
       * @return Returns a new gum::IdCondSetIterator.
       */
      IdCondSetIterator operator+(const std::size_t i);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /**
       * @brief Returns the position of the iterator in the IdCondSet.
       * @return Returns the position of the iterator in the IdCondSet.
       * @throw UndefinedIteratorValue Raised on end()
       */
      std::size_t pos() const;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// a pointer on the sequence stored in the IdCondSet
      const Sequence< NodeId >* _seq_{nullptr};

      /// The index in the IdCondSet's sequence where the iterator is pointing.
      std::size_t _index_{std::size_t(0)};


      /// places the index to the end of the sequence
      void _gotoEnd_();

      friend class IdCondSet;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };


    /** @class IdCondSet
     * @brief A class for storing a pair of sets of NodeIds, the second one
     * corresponding to a conditional set.
     * @headerfile idSet.h <agrum/BN/learning/scores_and_tests/idSet.h>
     * @ingroup learning_scores
     *
     * IdCondSets are used by learning caches to store pairs of sets of nodes,
     * the second ones represent typically conditional sets.
     * This is useful for storing in hashtables the scores assigned to sets
     * of nodes given other nodes. NodeSets are actually not well suited for
     * this purpose because their implementations makes the computation of their
     * hash values quite difficult. IdCondSets fix this issue.
     */
    class IdCondSet {
      public:
      using iterator            = IdCondSetIterator;
      using const_iterator      = IdCondSetIterator;
      using iterator_safe       = IdCondSetIterator;
      using const_iterator_safe = IdCondSetIterator;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      IdCondSet();

      /// default constructor with no variable on the left side
      /** @param ids the set of variables
       * @param rhs_ids indicate whether the ids are on the right side of the
       * conditioning bar or not
       * @param ordered_ids indicates whether the ids in rhs_ids should be
       * considered as an ordered set or an unordered set
       */
      IdCondSet(const std::vector< NodeId >& ids, const bool rhs_ids, const bool ordered_ids);

      /// default constructor with one variable on the left side
      /** @param var1 the variable on the left side of the conditioning bar
       * @param rhs_ids the set of variables on the right side of the
       * conditioning bar
       * @param ordered_rhs_ids indicates whether the ids in rhs_ids should be
       * considered as an ordered set or an unordered set
       */
      IdCondSet(NodeId                       var1,
                const std::vector< NodeId >& rhs_ids,
                const bool                   ordered_rhs_ids = false);

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
       */
      IdCondSet(NodeId                       var1,
                NodeId                       var2,
                const std::vector< NodeId >& rhs_ids,
                const bool                   ordered_lhs_vars,
                const bool                   ordered_rhs_ids = false);

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
       */
      IdCondSet(NodeId                       var1,
                NodeId                       var2,
                NodeId                       var3,
                const std::vector< NodeId >& rhs_ids,
                const bool                   ordered_lhs_vars,
                const bool                   ordered_rhs_ids = false);

      /// copy constructor
      IdCondSet(const IdCondSet& from);

      /// move constructor
      IdCondSet(IdCondSet&& from);

      /// virtual copy constructor
      virtual IdCondSet* clone() const;

      /// destructor
      virtual ~IdCondSet();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      IdCondSet& operator=(const IdCondSet& from);

      /// move operator
      IdCondSet& operator=(IdCondSet&& from);

      /// returns the node id stored at a given index
      NodeId operator[](const std::size_t index) const;

      /// returns true if both sets are equal
      bool operator==(const IdCondSet& from) const;

      /// returns true if the sets differ
      bool operator!=(const IdCondSet& from) const;

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
      const Sequence< NodeId >& ids() const;

      /// returns the idSet at the right hand side of the conditioning bar
      IdCondSet conditionalIdCondSet() const;

      /// returns the number of left hand side ids
      std::size_t nbLHSIds() const;

      /// returns the number of right hand side ids
      std::size_t nbRHSIds() const;

      /// indicates whether the IdCondSet contains the IdCondSet passed in argument
      bool contains(const IdCondSet& set) const;

      /// removes all the nodes from the IdCondSet
      void clear();

      /// returns the number of variables (both left and right hand side)
      std::size_t size() const;

      /// returns the position of a given node in the IdCondSet
      /** @throw NotFound Raised if the element does not exist. */
      std::size_t pos(const NodeId id) const;

      /// indicates whether a given id is contained in the IdCondSet
      bool exists(const NodeId id) const;

      /// erase a node in the idset
      /** If the element cannot be found, the function does nothing. In
       * particular, it throws no exception. */
      void erase(const NodeId id);

      /// indicates whether the idset contains a non-empty conditioning set
      bool hasConditioningSet() const;

      /// indicates whether the IdCondSet contains some nodes or not
      bool empty() const;

      /// returns the content of the set as a string
      std::string toString() const;

      /// returns the pair of conditioned gum::NodeSet and conditioning gum::NodeSet
      std::pair< NodeSet, NodeSet > toNodeSets() const;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the ordered set of ids on the right side of the conditioning bar
      Sequence< NodeId > _ids_;

      /// the number of left ids
      std::size_t _nb_lhs_ids_{std::size_t(0)};

      /// Stores the end iterator for fast access.
      IdCondSetIterator _end_safe_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };


    /// the display operator
    std::ostream& operator<<(std::ostream& stream, const IdCondSet& idset);

  } /* namespace learning */


  /// the hash function for idSets
  template <>
  class HashFunc< learning::IdCondSet >: public HashFuncBase< learning::IdCondSet > {
    public:
    /**
     * @brief Returns the value of a key as a Size.
     * @param key The value to return as a Size.
     * @return Returns the value of a key as a Size.
     */
    static Size castToSize(const learning::IdCondSet& key);

    /// computes the hashed value of a key
    Size operator()(const learning::IdCondSet& key) const final;
  };


} /* namespace gum */


// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/stattests/idCondSet_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_ID_SET_H */
