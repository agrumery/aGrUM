/****************************************************************************
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Inline implementation of PRMGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/PRM/generator/PRMGenerator.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    INLINE PRMGenerator< GUM_SCALAR >::PRMGenerator() {
      GUM_CONSTRUCTOR(PRMGenerator);
    }

    template < typename GUM_SCALAR >
    INLINE PRMGenerator< GUM_SCALAR >::PRMGenerator(const PRMGenerator& source) :
        name_gen_(source.name_gen_) {
      GUM_CONS_CPY(PRMGenerator);
    }

    template < typename GUM_SCALAR >
    INLINE PRMGenerator< GUM_SCALAR >::~PRMGenerator() {
      GUM_DESTRUCTOR(PRMGenerator);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMGenerator< GUM_SCALAR >::setNameGenerator(const NameGenerator& name_gen) {
      name_gen_ = name_gen;
    }

    template < typename GUM_SCALAR >
    INLINE const NameGenerator& PRMGenerator< GUM_SCALAR >::getNameGenerator() const {
      return name_gen_;
    }

  } /* namespace prm */
} /* namespace gum */
