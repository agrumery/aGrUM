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
 * @brief Implementation of gum::Instantiation.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/base/multidim/implementations/multiDimAdressable.h>
#include <agrum/base/multidim/instantiation.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/multidim/instantiation_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  // Default constructor
  Instantiation::Instantiation() : _master_(nullptr), _overflow_(false) {
    GUM_CONSTRUCTOR(Instantiation);
  }

  // destructor
  Instantiation::~Instantiation() {
    GUM_DESTRUCTOR(Instantiation);
    // unregister the Instantiation from its  _master_

    if (_master_) _master_->unregisterSlave(*this);
  }

  void Instantiation::_init_(MultiDimAdressable* master) {
    // for speed issues
    GUM_ASSERT(master != nullptr);

    const Sequence< const DiscreteVariable* >& v = master->variablesSequence();
    _vars_.resize(v.size());
    _vals_.reserve(v.size());
    // fill the instantiation

    for (const auto var: v)
      _add_(*var);

    actAsSlave(master->getMasterRef());
  }

  // constructor for a Instantiation contained into a MultiDimInterface
  Instantiation::Instantiation(MultiDimAdressable& d) : _master_(0), _overflow_(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Instantiation);
    _init_(&d);
  }

  Instantiation::Instantiation(const MultiDimAdressable& d) : _master_(0), _overflow_(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Instantiation);
    _init_(const_cast< MultiDimAdressable* >(&d));
  }

  // constructor for a Instantiation contained into a MultiDimInterface
  Instantiation::Instantiation(MultiDimAdressable* d) : _master_(0), _overflow_(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Instantiation);

    if (d) _init_(d);
  }

  // constructor for a Instantiation contained into a MultiDimInterface this
  // constructor is needed in order to allow creation of Instantiation(this) in
  // MultiDimAdressable and below
  Instantiation::Instantiation(const MultiDimAdressable* const_d) : _master_(0), _overflow_(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Instantiation);

    if (const_d) _init_(const_cast< MultiDimAdressable* >(const_d));
  }

  // copy constructor
  Instantiation::Instantiation(const Instantiation& aI, const bool notifyMaster) :
      MultiDimInterface(), _master_(0), _overflow_(false) {
    // for debugging purposes
    GUM_CONS_CPY(Instantiation);
    // copy the content of aI
    _vars_     = aI._vars_;
    _vals_     = aI._vals_;
    _overflow_ = aI._overflow_;

    if (aI._master_ && notifyMaster) actAsSlave(*aI._master_);
  }

  // operator=
  Instantiation& Instantiation::operator=(const Instantiation& aI) {
    if (_master_) {
      if (!aI.isMaster(_master_)) {   // aI as the same master.
        if (nbrDim() != aI.nbrDim()) { GUM_ERROR(OperationNotAllowed, "in slave Instantiation") }

        for (Idx i = 0; i < nbrDim(); i++) {
          if ((!contains(aI.variable(i))) || (!aI.contains(variable(i)))) {
            GUM_ERROR(OperationNotAllowed, "in slave Instantiation")
          }
        }
      }

      setVals(aI);
    } else {
      // copy the content of aI
      _vars_     = aI._vars_;
      _vals_     = aI._vals_;
      _overflow_ = aI._overflow_;

      if (aI._master_) actAsSlave(*aI._master_);
    }

    return *this;
  }

  // Gives a string version of a Instantiation
  std::string Instantiation::toString() const {
    std::stringstream sstr;
    // check if the value of the instantiation is correct

    if (_overflow_) { sstr << "<invalid>"; }

    sstr << "<";

    bool first = true;

    for (const auto var: _vars_) {
      if (!first) sstr << "|";

      first = false;
      sstr << var->name() << ":" << var->label(val(*var));
    }

    sstr << ">";

    return sstr.str();
  }

  // give a Id value for Hamming distance
  Idx Instantiation::hamming() const {
    Idx res = 0;

    for (const auto var: _vars_)
      res += val(*var);

    return res;
  }

  void Instantiation::setValsFrom(
      const HashTable< const DiscreteVariable*, const DiscreteVariable* >& map,
      const Instantiation&                                                 external) {
    for (const auto& elt: map) {
      const DiscreteVariable& var = *elt.second;

      try {
        Idx val = external.val(*elt.first);

        try {
          chgVal(var, val);
        } catch (NotFound const&) {
          GUM_ERROR(NotFound, var.name() << " : missing variable in instantiation")
        }
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, var.name() << " : missing variable in external instantiation")
      }
    }
  }

  void Instantiation::_masterChangeNotification_(Idx varPos, Idx newVal, Idx oldVal) const {
    if (_master_) _master_->changeNotification(*this, _vars_[varPos], oldVal, newVal);
  }

  void Instantiation::_masterFirstNotification_() const {
    if (_master_) _master_->setFirstNotification(*this);
  }

  void Instantiation::_masterIncNotification_() const {
    if (_master_) _master_->setIncNotification(*this);
  }

  void Instantiation::_masterLastNotification_() const {
    if (_master_) _master_->setLastNotification(*this);
  }

  void Instantiation::_masterDecNotification_() const {
    if (_master_) _master_->setDecNotification(*this);
  }

  // deassociate the master MultiDimAdressable, if any
  bool Instantiation::forgetMaster() {
    if (_master_) {
      _master_->unregisterSlave(*this);
      _master_ = nullptr;
    }
    return true;
  }

  // force the variables sequence order to be the same as the master one
  void Instantiation::synchronizeWithMaster(const MultiDimAdressable* m) {
    if (m != _master_) { GUM_ERROR(OperationNotAllowed, "only master can do this") }

    _reorder_(_master_->variablesSequence());
  }

  // erase new dim by master
  void Instantiation::eraseWithMaster(const MultiDimAdressable* m, const DiscreteVariable& v) {
    if (m != _master_) { GUM_ERROR(OperationNotAllowed, "only master can do this") }

    _erase_(v);

    if (_master_) _master_->setChangeNotification(*this);
  }

  // tries to register the Instantiation to a MultiDimAdressable
  bool Instantiation::actAsSlave(MultiDimAdressable& aMD) {
    // if  _master_ : not allowed
    if (_master_ != nullptr) { GUM_ERROR(OperationNotAllowed, "in slave Instantiation") }

    _master_ = &aMD;

    // perform the registration
    if (aMD.registerSlave(*this)) {
      return true;
    } else {
      _master_ = nullptr;
      return false;
    }
  }

  // an operator for user-friendly displaying the content of a Instantiation
  std::ostream& operator<<(std::ostream& aStream, const Instantiation& i) {
    aStream << i.toString();
    return aStream;
  }

} /* namespace gum */
