/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#pragma once

#include <agrum/base/core/threads/threadData.h>

namespace gum {

  template < typename T_DATA >
  ThreadData< T_DATA >::ThreadData(const T_DATA& theData) : data(theData) {}

  template < typename T_DATA >
  ThreadData< T_DATA >::ThreadData(T_DATA&& theData) : data(std::move(theData)) {}

  template < typename T_DATA >
  ThreadData< T_DATA >::ThreadData(const ThreadData< T_DATA >& from) : data(from.data) {}

  template < typename T_DATA >
  ThreadData< T_DATA >::ThreadData(ThreadData< T_DATA >&& from) : data(std::move(from.data)) {}

  template < typename T_DATA >
  ThreadData< T_DATA >& ThreadData< T_DATA >::operator=(const ThreadData< T_DATA >& from) = default;

  template < typename T_DATA >
  ThreadData< T_DATA >& ThreadData< T_DATA >::operator=(ThreadData< T_DATA >&& from) {
    data = std::move(from.data);
    return *this;
  }

  template < typename T_DATA >
  bool ThreadData< T_DATA >::operator==(const ThreadData< T_DATA >& from) const {
    return data == from.data;
  }

  template < typename T_DATA >
  bool ThreadData< T_DATA >::operator==(const T_DATA& from) const {
    return data == from;
  }

  template < typename T_DATA >
  bool ThreadData< T_DATA >::operator!=(const ThreadData< T_DATA >& from) const {
    return data != from.data;
  }

  template < typename T_DATA >
  bool ThreadData< T_DATA >::operator!=(const T_DATA& from) const {
    return data != from;
  }

  template < typename T_DATA >
  typename ThreadData< T_DATA >::data_type ThreadData< T_DATA >::operator*() {
    return data;
  }

  template < typename T_DATA >
  template < typename X >
  std::enable_if_t< std::is_pointer< X >::value, typename ThreadData< T_DATA >::data_pointer >
      ThreadData< T_DATA >::operator->() {
    return data;
  }

  template < typename T_DATA >
  template < typename X >
  std::enable_if_t< !std::is_pointer< X >::value, typename ThreadData< T_DATA >::data_pointer >
      ThreadData< T_DATA >::operator->() {
    return &data;
  }

}   // namespace gum
