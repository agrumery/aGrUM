/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief A wrapper that enables to store data in a way that prevents false
 * cacheline sharing.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_THREAD_DATA_H
#define GUM_THREAD_DATA_H

#include <new>

#include <agrum/agrum.h>

#include <type_traits>

namespace gum {

  /** @class ThreadData
   * @brief A wrapper that enables to store data in a way that prevents false
   * cacheline sharing.
   * @headerfile threadData.h <agrum/base/core/threads/threadData.h>
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
  // @TODO: for C++17, replace by
  // alignas(std::hardware_destructive_interference_size)
  struct alignas(128) ThreadData {
    /// the return types of accessors: T_DATA if scalar, else T_DATA&
    using data_type = std::conditional< std::is_scalar< T_DATA >::value, T_DATA, T_DATA& >;

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
      data = from.data;
      return *this;
    }

    /// move operator
    ThreadData< T_DATA >& operator=(ThreadData< T_DATA >&& from) {
      data = std::move(from.data);
      return *this;
    }

    /// equality operator
    bool operator==(const ThreadData< T_DATA >& from) { return data == from.data; }

    /// equality operator
    bool operator==(const T_DATA& from) { return data == from; }

    /// inequality operator
    bool operator!=(const ThreadData< T_DATA >& from) { return data != from.data; }

    /// inequality operator
    bool operator!=(const T_DATA& from) { return data != from; }

    /// @}

    // ##########################################################################
    /// @name Accessors
    // ##########################################################################

    /// @{

    /// easy accessor
    data_type operator*() { return data; }

    using data_pointer =
        typename std::conditional< std::is_pointer< T_DATA >::value,
                                   T_DATA,
                                   typename std::add_pointer< T_DATA >::type >::type;

    template < typename X = T_DATA >
    std::enable_if_t< std::is_pointer< X >::value, data_pointer > operator->() {
      return data;
    }

    template < typename X = T_DATA >
    std::enable_if_t< !std::is_pointer< X >::value, data_pointer > operator->() {
      return &data;
    }

    /// the data we wish to store without cacheline parallel problem
    T_DATA data;
  };


} /* namespace gum */

#endif /* GUM_THREAD_DATA_H */
