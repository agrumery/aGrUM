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
       const PRMFuncAttribute< GUM_SCALAR >& source) :
        PRMScalarAttribute< GUM_SCALAR >(source) {
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
