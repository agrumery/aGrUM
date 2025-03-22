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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Implementation of the MultiDimReadOnly class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

namespace gum {

  // Default constructor

  template < typename GUM_SCALAR >
  INLINE MultiDimReadOnly< GUM_SCALAR >::MultiDimReadOnly() :
      MultiDimImplementation< GUM_SCALAR >() {
    GUM_CONSTRUCTOR(MultiDimReadOnly);
  }

  // Default constructor

  template < typename GUM_SCALAR >
  INLINE
      MultiDimReadOnly< GUM_SCALAR >::MultiDimReadOnly(const MultiDimReadOnly< GUM_SCALAR >& from) :
      MultiDimImplementation< GUM_SCALAR >(from) {
    GUM_CONS_CPY(MultiDimReadOnly);
  }

  // destructor

  template < typename GUM_SCALAR >
  INLINE MultiDimReadOnly< GUM_SCALAR >::~MultiDimReadOnly() {
    GUM_DESTRUCTOR(MultiDimReadOnly);
  }

  // @throw OperationNotAllowed Raised because this is a read only table.

  template < typename GUM_SCALAR >
  INLINE void MultiDimReadOnly< GUM_SCALAR >::set(const Instantiation& i,
                                                  const GUM_SCALAR&    value) const {
    GUM_ERROR(OperationNotAllowed, "Write access to an aggregator")
  }

  // @throw OperationNotAllowed Raised because this is a read only table.

  template < typename GUM_SCALAR >
  void MultiDimReadOnly< GUM_SCALAR >::fill(const GUM_SCALAR&) const {
    GUM_ERROR(OperationNotAllowed, "Write access to an aggregator")
  }

  // @throw OperationNotAllowed Raised because this is a read only table.

  template < typename GUM_SCALAR >
  GUM_SCALAR& MultiDimReadOnly< GUM_SCALAR >::get_(const Instantiation& i) const {
    GUM_ERROR(OperationNotAllowed, "No (unconst) access to an aggregator")
  }

} /* namespace gum */
