/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


/** @file
 * @brief Outlined implementation of ArgMaxSet
 *
 * @author Jean-Christophe MAGNAN
 */

#include <agrum/tools/core/argMaxSet.h>


namespace gum {

  // ###########################################################################
  // CNL
  // ###########################################################################

  // ============================================================================
  // Constructor
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::ArgMaxSet() {
    GUM_CONSTRUCTOR(ArgMaxSet);
    argMaxSeq__ = new Sequence< GUM_SCALAR_SEQ >();
  }

  // ============================================================================
  // Constructor
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::ArgMaxSet(
     const GUM_SCALAR_VAL& val, const GUM_SCALAR_SEQ& elem) {
    GUM_CONSTRUCTOR(ArgMaxSet);
    argMaxSeq__ = new Sequence< GUM_SCALAR_SEQ >();
    argMaxSeq__->insert(elem);
    val__ = val;
  }

  // ============================================================================
  // Copy Constructor
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::ArgMaxSet(
     const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& src) {
    GUM_CONS_CPY(ArgMaxSet);
    argMaxSeq__ = new Sequence< GUM_SCALAR_SEQ >();
    this->operator+=(src);
    val__ = src.value();
  }

  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >&
     ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::operator=(
        const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& src) {
    this->argMaxSeq__->clear();
    this->operator+=(src);
    val__ = src.value();
    return *this;
  }

  // ============================================================================
  // Destructor
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::~ArgMaxSet() {
    GUM_DESTRUCTOR(ArgMaxSet);
    delete argMaxSeq__;
  }

  // ###########################################################################
  // Operators
  // ###########################################################################

  // ============================================================================
  // Ajout d'un élément
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >&
     ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::operator+=(
        const GUM_SCALAR_SEQ& elem) {
    argMaxSeq__->insert(elem);
    return *this;
  }

  // ============================================================================
  // Use to insert the content of another set inside this one
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >&
     ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::operator+=(
        const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& src) {
    for (auto iter = src.beginSafe(); iter != src.endSafe(); ++iter)
      if (!argMaxSeq__->exists(*iter)) argMaxSeq__->insert(*iter);
    return *this;
  }

  // ============================================================================
  // Compares two ArgMaxSet to check if they are equals
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  bool ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::operator==(
     const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& compared) const {
    if (val__ != compared.value()) return false;
    for (auto iter = compared.beginSafe(); iter != compared.endSafe(); ++iter)
      if (!argMaxSeq__->exists(*iter)) return false;
    for (auto iter = this->beginSafe(); iter != this->endSafe(); ++iter)
      if (!compared.exists(*iter)) return false;
    return true;
  }

}   // End of namespace gum
