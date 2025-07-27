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


#include <sstream>

#include <agrum/base/multidim/implementations/multiDimAdressable.h>
#include <agrum/base/multidim/setInst.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/multidim/setInst_inl.h>
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  void SetInst::_init_(MultiDimAdressable* master) {
    // for speed issues
    const Sequence< const DiscreteVariable* >& v = master->variablesSequence();
    _vars_.resize(v.size());
    _vals_.reserve(v.size());
    // fill the SetInst

    for (const auto var: v)
      _add_(*var);

    // if ( master ) actAsSlave( master->getMasterRef() );
  }

  // constructor for a SetInst contained into a MultiDimInterface

  SetInst::SetInst(MultiDimAdressable& d) : /* _master_( 0 ),*/ _overflow_(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(SetInst);
    _init_(&d);
  }

  SetInst::SetInst(const MultiDimAdressable& d) : /* _master_( 0 ),*/ _overflow_(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(SetInst);
    _init_(const_cast< MultiDimAdressable* >(&d));
  }

  // constructor for a SetInst contained into a MultiDimInterface

  SetInst::SetInst(MultiDimAdressable* d) : /* _master_( 0 ),*/ _overflow_(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(SetInst);

    if (d) _init_(d);
  }

  // constructor for a SetInst contained into a MultiDimInterface
  /** this constructor is needed in order to allow creation of SetInst(this)
   * in MultiDimAdressable and below */

  SetInst::SetInst(const MultiDimAdressable* const_d) : /* _master_( 0 ),*/ _overflow_(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(SetInst);

    if (const_d) _init_(const_cast< MultiDimAdressable* >(const_d));
  }

  // copy constructor

  SetInst::SetInst(const SetInst& aI) :
      /* MultiDimInterface(),  _master_( 0 ),*/ _overflow_(false) {
    // for debugging purposes
    GUM_CONS_CPY(SetInst);
    // copy the content of aI
    _vars_     = aI._vars_;
    _vals_     = aI._vals_;
    _overflow_ = aI._overflow_;

    // if ( aI. _master_ && notifyMaster ) actAsSlave( *aI. _master_ );
  }

  SetInst::SetInst(const Instantiation& aI) :
      /* MultiDimInterface(),  _master_( 0 ),*/ _overflow_(false) {
    // for debugging purposes
    GUM_CONS_CPY(SetInst);
    const Sequence< const DiscreteVariable* >& v = aI.variablesSequence();
    _vars_.resize(v.size());
    // __vals.reserve( v.size() );
    // fill the SetInst

    for (const auto var: v) {
      _add_(*var);
      _vals_[_vars_.pos(var)] = (Size(1) << (aI.val(*var)));
    }
  }

  // operator=
  SetInst& SetInst::operator=(const SetInst& aI) {
    // copy the content of aI
    _vars_     = aI._vars_;
    _vals_     = aI._vals_;
    _overflow_ = aI._overflow_;

    return *this;
  }

  // Gives a string version of a SetInst
  std::string SetInst::toString() const {
    std::stringstream sstr;
    // check if the value of the SetInst is correct

    if (_overflow_) { sstr << "<invalid>"; }

    sstr << "<";

    Sequence< const DiscreteVariable* >::iterator_safe iter = _vars_.begin();

    if (iter != _vars_.end()) {
      std::stringstream sstr2;
      sstr2.str("");
      Size si   = variable(iter.pos()).domainSize();
      Size valb = vals(iter.pos());

      while (si-- != 0) {
        std::stringstream sstr4;
        sstr4 << ((valb & 1) ? "1" : "0") << sstr2.str();
        valb >>= 1;
        sstr2.str("");
        ;
        sstr2 << sstr4.str();
      }

      sstr << variable(iter.pos()).name() << ":" << sstr2.str();
      ++iter;

      while (iter != _vars_.end()) {
        std::stringstream sstr3;
        sstr3 << "";
        si = variable(iter.pos()).domainSize();

        valb = vals(iter.pos());

        while (si-- != 0) {
          std::stringstream sstr4;

          sstr4 << ((valb & 1) ? "1" : "0") << sstr3.str();
          valb >>= 1;
          sstr3.str("");
          sstr3 << sstr4.str();
        }

        sstr << "|" << variable(iter.pos()).name() << ":" << sstr3.str();
        ++iter;
      }
    }

    sstr << ">";

    return sstr.str();
  }

  // an operator for user-friendly displaying the content of a SetInst
  std::ostream& operator<<(std::ostream& aStream, const SetInst& i) {
    aStream << i.toString();
    return aStream;
  }

  gum::SetInst& operator<<(gum::SetInst& inst, const gum::DiscreteVariable& i) {
    inst.add(i);
    return inst;
  }

  gum::SetInst& operator>>(gum::SetInst& inst, const gum::DiscreteVariable& i) {
    inst.erase(i);
    return inst;
  }

  void gum::SetInst::assign_values(
      gum::Bijection< const gum::DiscreteVariable*, const gum::DiscreteVariable* >& bij,
      const gum::SetInst&                                                           i,
      gum::SetInst&                                                                 j) {
    try {
      for (const auto var: i.variablesSequence())
        j.chgVal(bij.second(var), i.val(var));
    } catch (gum::NotFound&) {
      GUM_ERROR(gum::NotFound, "missing variable in bijection or SetInst")
    }
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
