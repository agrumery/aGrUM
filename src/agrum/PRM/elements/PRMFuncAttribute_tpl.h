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


/**
 * @file
 * @brief Inline implementation of gum::PRMFuncAttribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */


namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    INLINE PRMFuncAttribute< GUM_SCALAR >::PRMFuncAttribute(
        const std::string&                    name,
        const PRMType&                        type,
        MultiDimImplementation< GUM_SCALAR >* impl) :
        PRMScalarAttribute< GUM_SCALAR >(name, type, impl) {
      std::stringstream str;
      str << "In FuncAttribute(" << name << ", " << type.name() << ", " << impl << ")";
      GUM_CONSTRUCTOR(PRMFuncAttribute);
    }

    template < typename GUM_SCALAR >
    INLINE PRMFuncAttribute< GUM_SCALAR >::~PRMFuncAttribute() {
      GUM_DESTRUCTOR(PRMFuncAttribute);
    }

    template < typename GUM_SCALAR >
    INLINE void
        PRMFuncAttribute< GUM_SCALAR >::addParent(const PRMClassElement< GUM_SCALAR >& elt) {}

    template < typename GUM_SCALAR >
    INLINE void PRMFuncAttribute< GUM_SCALAR >::addChild(const PRMClassElement< GUM_SCALAR >& elt) {
    }

    template < typename GUM_SCALAR >
    INLINE PRMFuncAttribute< GUM_SCALAR >::PRMFuncAttribute(
        const PRMFuncAttribute< GUM_SCALAR >& source) : PRMScalarAttribute< GUM_SCALAR >(source) {
      GUM_CONS_CPY(PRMFuncAttribute);
      GUM_ERROR(FatalError, "illegal call to copy constructor")
    }

    template < typename GUM_SCALAR >
    INLINE PRMFuncAttribute< GUM_SCALAR >&
           PRMFuncAttribute< GUM_SCALAR >::operator=(const PRMFuncAttribute< GUM_SCALAR >& from) {
      GUM_ERROR(FatalError, "illegal call to copy operator")
    }

    template < typename GUM_SCALAR >
    INLINE void PRMFuncAttribute< GUM_SCALAR >::setCpf_(Potential< GUM_SCALAR >* cpf) {
      GUM_ERROR(OperationNotAllowed, "Cannot change CPF of a functional attribute")
    }

  } /* namespace prm */
} /* namespace gum */
