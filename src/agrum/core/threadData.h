
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief A wrapper that enables to store data in a way that prevents false
 * cacheline sharing.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_THREAD_DATA_H
#define GUM_THREAD_DATA_H

#include <new>

#include <agrum/agrum.h>

namespace gum {

  /** @class ThreadData
   * @brief A wrapper that enables to store data in a way that prevents false
   * cacheline sharing.
   * @headerfile threadData.h <agrum/core/threadData.h>
   * @ingroup basicstruct_group
   *
   * When several threads access to some shared containers like vectors, it
   * may be the case that they access some data within these containers that
   * are too close, which results in false sharing of the cacheline. By
   * wrapping the data into a ThreadData, we guarantee that no false sharing
   * can occur.
   *
   * To create a wrapper data, simply use wrapped_data = ThreadData (data) and
   * To get the data wrapped, use wrapped_data.data;
   */
  template < typename T_DATA >
  // #TODO: replace by alignas(std::hardware_destructive_interference_size)
  // when pyAgrum will be compiled in C++17
  struct alignas(128) ThreadData {
    // ##########################################################################
    /// @name Constructors / Destructors
    // ##########################################################################
    /// @{

    /// default constructor
    ThreadData(const T_DATA& theData) : data(theData) {}

    /// default constructor
    ThreadData(T_DATA&& theData) : data(std::move(theData)) {}

    /// copy constructor
    ThreadData(const ThreadData< T_DATA >& from) : data(from.data) {}

    /// move constructor
    ThreadData(ThreadData< T_DATA >&& from) : data(std::move(from.data)) {}

    /// destructor
    ~ThreadData() {}

    /// @}

    // ##########################################################################
    /// @name Operators
    // ##########################################################################

    /// @{

    /// copy operator
    ThreadData< T_DATA >& operator=(const ThreadData< T_DATA >& from) {
      if (this != &from) data = from.data;
      return *this;
    }

    /// move operator
    ThreadData< T_DATA >& operator=(ThreadData< T_DATA >&& from) {
      data = std::move(from.data);
      return *this;
    }

    /// @}

    /// the data we wish to store without cacheline parallel problem
    T_DATA data;
  };

} /* namespace gum */

#endif /* GUM_THREAD_DATA_H */
