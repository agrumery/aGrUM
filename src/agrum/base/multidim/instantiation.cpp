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
    std::string sstr;
    // check if the value of the instantiation is correct

    if (_overflow_) sstr = "<invalid>";

    sstr += "<";

    bool first = true;

    for (const auto var: _vars_) {
      if (!first) sstr += "|";

      first = false;
      sstr += var->name();
      sstr += ":";
      sstr += var->label(val(*var));
    }

    sstr += ">";

    return sstr;
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

      if (!external.contains(*elt.first)) {
        GUM_ERROR(NotFound, var.name() << " : missing variable in external instantiation")
      }

      Idx val = external.val(*elt.first);

      if (!contains(var)) {
        GUM_ERROR(NotFound, var.name() << " : missing variable in instantiation")
      }

      chgVal(var, val);
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

  void Instantiation::add(const DiscreteVariable& v) {
    // if  _master_ : not allowed
    if (_master_) { GUM_ERROR(OperationNotAllowed, "in slave Instantiation") }

    // check if the variable already belongs to the tuple of variables
    // of the Instantiation
    if (_vars_.exists(&v)) {
      GUM_ERROR(DuplicateElement, "Var <" << v.name() << "> already exists in this instantiation")
    }

    for (const auto& vv: _vars_) {
      if (vv->name() == v.name()) {
        GUM_ERROR(InvalidArgument,
                  "Var with name <" << v.name() << "> already exists in this instantiation");
      }
    }

    // actually add the new dimension
    _add_(v);
  }

  void Instantiation::inc() {
    Size p = nbrDim();
    if (p == 0) { _overflow_ = true; }

    if (_overflow_) return;
    p -= 1;
    Idx cpt = 0;
    // if we are in overflow, do nothing

    // perform the increment
    while (true) {
      Idx v = _vals_[cpt];

      if (v + 1 == _vars_[cpt]->domainSize()) {
        _vals_[cpt] = 0;

        if (cpt == p) {
          _overflow_ = true;
          _masterFirstNotification_();
          return;
        } else ++cpt;
      } else {
        ++_vals_[cpt];
        break;
      }
    }

    _masterIncNotification_();
  }

  void Instantiation::dec() {
    Size p = nbrDim();
    if (p == 0) { _overflow_ = true; }

    if (_overflow_) return;
    p -= 1;
    Idx cpt = 0;
    // if we are in overflow, do nothing

    // perform the increment
    while (true) {
      Idx v = _vals_[cpt];

      if (v == 0) {
        _vals_[cpt] = _vars_[cpt]->domainSize() - 1;

        if (cpt == p) {
          _overflow_ = true;

          _masterLastNotification_();

          return;
        } else ++cpt;
      } else {
        --_vals_[cpt];
        break;
      }
    }

    _masterDecNotification_();
  }

  void Instantiation::incIn(const Instantiation& i) {
    // if i is empty, overflow and do nothing
    if (i.nbrDim() == 0) {
      _overflow_ = true;
      return;
    }

    // if we are in overflow, do nothing
    if (_overflow_) return;

    Size p = i.nbrDim() - 1;

    Idx i_cpt = 0;

    while (true) {
      // verify that  _vars_[cpt] belongs to i before incrementing its value
      const DiscreteVariable& v = i.variable(i_cpt);

      if (!contains(v)) {
        if (i_cpt == p) {
          _overflow_ = true;
          return;
        } else ++i_cpt;
      } else {
        Idx cpt = pos(v);
        Idx iv  = _vals_[cpt];

        if (iv + 1 == _vars_[cpt]->domainSize()) {
          _chgVal_(cpt, 0);

          if (i_cpt == p) {
            _overflow_ = true;
            return;
          } else ++i_cpt;
        } else {
          _chgVal_(cpt, iv + 1);
          return;
        }
      }
    }
  }

  void Instantiation::decIn(const Instantiation& i) {
    if (i.nbrDim() == 0) {
      _overflow_ = true;
      return;
    }
    Size p     = i.nbrDim() - 1;
    Idx  i_cpt = 0;
    // if we are in overflow, do nothing

    if (_overflow_) return;

    while (true) {
      // verify that  _vars_[cpt] belongs to i before incrementing its value
      const DiscreteVariable& v = i.variable(i_cpt);

      if (!contains(v)) {
        if (i_cpt == p) {
          _overflow_ = true;
          return;
        } else ++i_cpt;
      } else {
        Idx cpt = pos(v);
        Idx iv  = _vals_[cpt];

        if (iv == 0) {
          _chgVal_(cpt, _vars_[cpt]->domainSize() - 1);

          if (i_cpt == p) {
            _overflow_ = true;
            return;
          } else ++i_cpt;
        } else {
          _chgVal_(cpt, iv - 1);
          return;
        }
      }
    }
  }

  void Instantiation::incOut(const Instantiation& i) {
    if (nbrDim() == 0) {
      _overflow_ = true;
      return;
    }
    Size p   = nbrDim() - 1;
    Idx  cpt = 0;
    // if we are in overflow, do nothing

    if (_overflow_) return;

    while (true) {
      if (i.contains(_vars_[cpt])) {
        if (cpt == p) {
          _overflow_ = true;
          return;
        } else ++cpt;
      } else {
        Idx v = _vals_[cpt];

        if (v + 1 == _vars_[cpt]->domainSize()) {
          _chgVal_(cpt, 0);

          if (cpt == p) {
            _overflow_ = true;
            return;
          } else ++cpt;
        } else {
          _chgVal_(cpt, v + 1);
          return;
        }
      }
    }
  }

  void Instantiation::decOut(const Instantiation& i) {
    if (nbrDim() == 0) {
      _overflow_ = true;
      return;
    }
    Size p   = nbrDim() - 1;
    Idx  cpt = 0;
    // if we are in overflow, do nothing

    if (_overflow_) return;

    while (true) {
      if (i.contains(_vars_[cpt])) {
        if (cpt == p) {
          _overflow_ = true;
          return;
        } else ++cpt;
      } else {
        Idx v = _vals_[cpt];

        if (v == 0) {
          _chgVal_(cpt, _vars_[cpt]->domainSize() - 1);

          if (cpt == p) {
            _overflow_ = true;
            return;
          } else ++cpt;
        } else {
          _chgVal_(cpt, v - 1);
          return;
        }
      }
    }
  }

  void Instantiation::incNotVar(const DiscreteVariable& v) {
    if (nbrDim() == 0) {
      _overflow_ = true;
      return;
    }
    Size p   = nbrDim() - 1;
    Idx  cpt = 0;
    // if we are in overflow, do nothing

    if (_overflow_) return;

    while (true) {
      if (_vars_[cpt] == &v) {
        if (cpt == p) {
          _overflow_ = true;
          return;
        } else ++cpt;
      } else {
        Idx iv = _vals_[cpt];

        if (iv + 1 == _vars_[cpt]->domainSize()) {
          _chgVal_(cpt, 0);

          if (cpt == p) {
            _overflow_ = true;
            return;
          } else ++cpt;
        } else {
          _chgVal_(cpt, iv + 1);
          return;
        }
      }
    }
  }

  void Instantiation::decNotVar(const DiscreteVariable& v) {
    if (nbrDim() == 0) {
      _overflow_ = true;
      return;
    }
    Size p   = nbrDim() - 1;
    Idx  cpt = 0;
    // if we are in overflow, do nothing

    if (_overflow_) return;

    while (true) {
      if (_vars_[cpt] == &v) {
        if (cpt == p) {
          _overflow_ = true;
          return;
        } else ++cpt;
      } else {
        Idx iv = _vals_[cpt];

        if (iv == 0) {
          _chgVal_(cpt, _vars_[cpt]->domainSize() - 1);

          if (cpt == p) {
            _overflow_ = true;
            return;
          } else ++cpt;
        } else {
          _chgVal_(cpt, iv - 1);
          return;
        }
      }
    }
  }
} /* namespace gum */
