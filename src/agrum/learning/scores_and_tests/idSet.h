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
 * @brief A class used by RecordCounter to detect subsets of variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_ID_SET_H
#define GUM_LEARNING_ID_SET_H

#include <iostream>
#include <type_traits>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <initializer_list>
#include <utility>

#include <agrum/config.h>

namespace gum {

  namespace learning {

    /** @class IdSet
     * @ingroup learning_group
     * @brief transforms an unordered set of ids into an (increasingly) ordered one
     *
     * IdSets are used by RecordCounters to detect which sets of variables are
     * included in other sets. With this knowledge, RecordCounters only parse the
     * database for counting the number of observations of non-included sets of
     * variables and they subsequently deduce from these counts those for the
     * included sets.
     */
    template <typename Alloc = std::allocator<unsigned int>> class IdSet {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      IdSet();

      /// initializer list constructor
      IdSet(const std::initializer_list<unsigned int> list);

      /// default constructor
      template <typename OtherAlloc>
      IdSet(const std::vector<unsigned int, OtherAlloc> &ids, unsigned int sz);

      /// copy constructor
      IdSet(const IdSet<Alloc> &from);

      /// generalized copy constructor
      template <typename OtherAlloc> IdSet(const IdSet<OtherAlloc> &from);

      /// move constructor
      IdSet(IdSet<Alloc> &&from);

      /// destructor
      ~IdSet();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      IdSet<Alloc> &operator=(const IdSet<Alloc> &from);

      /// generalized copy operator
      template <typename OtherAlloc>
      IdSet<Alloc> &operator=(const IdSet<OtherAlloc> &from);

      /// move operator
      IdSet<Alloc> &operator=(IdSet<Alloc> &&from);

      /// returns the id stored at a given index
      unsigned int operator[](unsigned int index) const noexcept;

      /// inserts a new element into the set (assuming it is a Boolean)
      IdSet<Alloc> &operator<<(unsigned int id);

      /// returns true if both sets are equal
      template <typename OtherAlloc>
      bool operator==(const IdSet<OtherAlloc> &from) const noexcept;

      /// returns true if the sets differ
      template <typename OtherAlloc>
      bool operator!=(const IdSet<OtherAlloc> &from) const noexcept;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the set of ids contained in the object
      const std::vector<unsigned int, Alloc> &ids() const noexcept;

      /// returns the domain size of the id set
      unsigned int size() const noexcept;

      /// sets the domain size of the set
      void setSize(unsigned int) noexcept;

      /// returns the content of the set as a string
      std::string toString() const noexcept;

      /// indicates wether the current object is a subset of 'otherset'
      template <typename OtherAlloc>
      bool isSubset(const IdSet<OtherAlloc> &otherset) const noexcept;

      /// @}

      private:
      /// the ordered set of ids
      std::vector<unsigned int, Alloc> __ids;

      /// the domain size of the set
      unsigned int __size{0};
    };

    /// the display operator
    template <typename Alloc>
    std::ostream &operator<<(std::ostream &stream, const IdSet<Alloc> &idset);

  } /* namespace learning */

  /// the hash function for idSets
  template <typename Alloc>
  class HashFunc<learning::IdSet<Alloc>>
      : public HashFuncBase<learning::IdSet<Alloc>> {
    public:
    /// computes the hashed value of a key
    Size operator()(const learning::IdSet<Alloc> &key) const;
  };

  /// the hash function for pairs (idSet,unsigned int)
  template <typename Alloc>
  class HashFunc<std::pair<learning::IdSet<Alloc>, unsigned int>>
      : public HashFuncBase<std::pair<learning::IdSet<Alloc>, unsigned int>> {
    public:
    /// computes the hashed value of a key
    Size
    operator()(const std::pair<learning::IdSet<Alloc>, unsigned int> &key) const;
  };

  /// the hash function for pairs (idSet,pair<unsigned int,unsigned int>)
  template <typename Alloc>
  class HashFunc<std::tuple<learning::IdSet<Alloc>, unsigned int, unsigned int>>
      : public HashFuncBase<
            std::tuple<learning::IdSet<Alloc>, unsigned int, unsigned int>> {
    public:
    /// computes the hashed value of a key
    Size operator()(const std::tuple<learning::IdSet<Alloc>, unsigned int,
                                     unsigned int> &key) const;
  };

} /* namespace gum */

// always include the template implementation
#include <agrum/learning/scores_and_tests/idSet.tcc>

#endif /* GUM_LEARNING_ID_SET_H */
