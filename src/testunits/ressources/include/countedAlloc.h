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


#ifndef GUM_ACTIVE_TEST_COUNTEDALLOC_H
#define GUM_ACTIVE_TEST_COUNTEDALLOC_H

#include <vector>

namespace gum_tests {

  struct CountedAlloc {
    static std::size_t allocs;
    static std::size_t deallocs;

    static std::size_t nbAllocations() noexcept { return allocs; }

    static std::size_t nbDeallocations() noexcept { return deallocs; }

    static bool hasMemoryLeak() noexcept { return allocs != CountedAlloc::deallocs; }
  };

  std::size_t CountedAlloc::allocs{0};
  std::size_t CountedAlloc::deallocs{0};

  template < typename T >
  class DebugCountedAlloc: public CountedAlloc {
    public:
    using value_type = T;

    template < typename _Tp1 >
    struct rebind {
      using other = DebugCountedAlloc< _Tp1 >;
    };

    DebugCountedAlloc() noexcept {}

    template < typename U >
    DebugCountedAlloc(const DebugCountedAlloc< U >&) noexcept {}

    bool operator==(const DebugCountedAlloc< T >&) const { return true; }

    bool operator!=(const DebugCountedAlloc< T >&) const { return false; }

    T* allocate(std::size_t num) {
      CountedAlloc::allocs += num;
      return static_cast< T* >(::operator new(num * sizeof(T)));
    }

    void deallocate(T* p, std::size_t num) {
      CountedAlloc::deallocs += num;
      ::operator delete(p);
    }

    template < typename... Args >
    void construct(T* p, Args&&... args) {
      ::new ((void*)p) T(std::forward< Args >(args)...);
    }

    void destroy(T* p) { p->~T(); }
  };

} /* namespace gum_tests */

#endif /* GUM_ACTIVE_TEST_COUNTEDALLOC_H */
