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
 * @brief Inline implementation of gum::PRMAttribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >::PRMAttribute(const std::string& name) :
        PRMClassElement< GUM_SCALAR >(name) {
      GUM_CONSTRUCTOR(PRMAttribute);
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >::PRMAttribute(const PRMAttribute< GUM_SCALAR >& source) :
        PRMClassElement< GUM_SCALAR >(source) {
      GUM_CONS_CPY(PRMAttribute);
      GUM_ERROR(OperationNotAllowed, "Cannot copy Attributes")
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >::~PRMAttribute() {
      GUM_DESTRUCTOR(PRMAttribute);
    }

    template < typename GUM_SCALAR >
    void PRMAttribute< GUM_SCALAR >::overload(PRMAttribute< GUM_SCALAR >* source) {
      auto old_type = this->type_();
      this->type_(source->type_());
      source->type_(old_type);
    }

  } /* namespace prm */
} /* namespace gum */
