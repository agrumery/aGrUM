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
 * @brief Inline implemenation of gum::Instantiation.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/tools/multidim/implementations/multiDimAdressable.h>

namespace gum {

  // indicates whether a given variable belongs to the Instantiation
  INLINE bool Instantiation::contains(const DiscreteVariable& v) const { return _vars_.exists(&v); }

  INLINE bool Instantiation::contains(const std::string& name) const {
    return contains(variable(name));
  }

  // indicates whether a given variable belongs to the Instantiation
  INLINE bool Instantiation::contains(const DiscreteVariable* v) const { return _vars_.exists(v); }

  // modifies internally the value of a given variable of the sequence
  INLINE void Instantiation::_chgVal_(Idx varPos, Idx newVal) {
    Idx oldVal     = _vals_[varPos];
    _vals_[varPos] = newVal;

    _masterChangeNotification_(varPos, newVal, oldVal);
  }

  // modifies the value of a given variable of the sequence (external function)
  INLINE Instantiation& Instantiation::chgVal(const DiscreteVariable& v, Idx newVal) {
    try {
      // check that the variable does belong to the instantiation and that the
      // new
      // value is possible.
      Idx varPos = _vars_.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize()) { GUM_ERROR(OutOfBounds, "") }

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      _chgVal_(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "instantiation does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE Instantiation& Instantiation::chgVal(const DiscreteVariable* v, Idx newVal) {
    try {
      // check that the variable does belong to the instantiation and that the
      // new
      // value is possible.
      Idx varPos = _vars_.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize()) { GUM_ERROR(OutOfBounds, "") }

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      _chgVal_(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "instantiation does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  // modifies the value of a given variable of the sequence (external function)
  INLINE Instantiation& Instantiation::chgVal(Idx varPos, Idx newVal) {
    // check that the variable does belong to the instantiation and that the new
    // value is possible.
    if (_vals_.size() <= varPos) { GUM_ERROR(NotFound, "") }

    if (newVal >= _vars_[varPos]->domainSize()) { GUM_ERROR(OutOfBounds, "") }

    // if we were in overflow, indicate that we are not anymore
    _overflow_ = false;

    _chgVal_(varPos, newVal);

    return *this;
  }

  INLINE Instantiation& Instantiation::chgVal(const std::string& var, Idx newVal) {
    return chgVal(variable(var), newVal);
  }

  INLINE Instantiation& Instantiation::chgVal(const std::string& var, const std::string& newVal) {
    const auto& vv  = variable(var);
    Idx         pos = vv.index(newVal);
    return chgVal(vv, pos);
  }

  // adds a new var to the sequence of vars
  INLINE void Instantiation::add(const DiscreteVariable& v) {
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

  // removes a variable from the sequence of vars
  INLINE void Instantiation::erase(const DiscreteVariable& v) {
    // if  _master_ : not allowed
    if (_master_) { GUM_ERROR(OperationNotAllowed, "in slave Instantiation") }

    // check that the variable does actually belong to the Instantiation
    if (!_vars_.exists(&v)) { GUM_ERROR(NotFound, "Var does not exist in this instantiation") }

    // actually delete the dimension
    _erase_(v);
  }

  INLINE void Instantiation::erase(const std::string& name) { erase(variable(name)); }

  // removes everything
  INLINE void Instantiation::clear() {
    if (_master_) { GUM_ERROR(OperationNotAllowed, "in slave Instantiation") }

    _vars_.clear();
    _vals_.clear();
  }

  // @brief returns the product of the size of the domains of the variables
  // belonging to the matrix
  INLINE Size Instantiation::domainSize() const {
    Size s = 1;

    for (const auto var: _vars_)
      s *= var->domainSize();

    return s;
  }

  // returns the index of a var
  INLINE Idx Instantiation::pos(const DiscreteVariable& k) const { return _vars_.pos(&k); }

  // returns the number of vars in the sequence
  INLINE Idx Instantiation::nbrDim() const { return _vars_.size(); }

  // returns the current value of a given variable
  INLINE Idx Instantiation::val(Idx i) const {
    if (i >= _vals_.size()) {
      GUM_ERROR(NotFound, i << " is out of bound index for the instantiation.")
    }

    return _vals_[i];
  }

  // returns the current value of a given variable
  INLINE Idx Instantiation::val(const DiscreteVariable& var) const {
    return _vals_[_vars_.pos(&var)];
  }
  // returns the current value of a given variable
  INLINE Idx Instantiation::val(const std::string& name) const { return val(variable(name)); }


  // returns the current value of a given variable
  INLINE Idx Instantiation::valFromPtr(const DiscreteVariable* pvar) const {
    return _vals_[_vars_.pos(pvar)];
  }

  // returns the variable at position i in the tuple
  INLINE const DiscreteVariable& Instantiation::variable(Idx i) const { return *(_vars_.atPos(i)); }
  // returns the variable with name in the tuple
  INLINE const DiscreteVariable& Instantiation::variable(const std::string& name) const {
    for (const auto& v: _vars_) {
      if (v->name() == name) return *v;
    }

    GUM_ERROR(NotFound, "'" << name << "' can not be found in the instantiation.")
  }

  // indicates whether the current value of the tuple is correct or not
  INLINE bool Instantiation::inOverflow() const { return _overflow_; }

  // end() just is a synonym for inOverflow()
  INLINE bool Instantiation::end() const { return inOverflow(); }

  // rend() just is a synonym for inOverflow()
  INLINE bool Instantiation::rend() const { return inOverflow(); }

  // indicates that the current value is correct even if it should be in
  // overflow
  INLINE void Instantiation::unsetOverflow() { _overflow_ = false; }

  // alias for unsetOverflow
  INLINE void Instantiation::unsetEnd() { _overflow_ = false; }

  // operator ++
  INLINE void Instantiation::inc() {
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

  // operator --
  INLINE void Instantiation::dec() {
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

  // operator ++
  INLINE Instantiation& Instantiation::operator++() {
    inc();
    return *this;
  }

  // operator --
  INLINE Instantiation& Instantiation::operator--() {
    dec();
    return *this;
  }

  // operator +=
  INLINE Instantiation& Instantiation::operator+=(Size depl) {
    for (Idx i = 0; i < depl; i++)
      inc();

    return *this;
  }

  // operator -=
  INLINE Instantiation& Instantiation::operator-=(Size depl) {
    for (Idx i = 0; i < depl; i++)
      dec();

    return *this;
  }

  // assign the (0,0,...) first value to the tuple of the Instantiation.
  INLINE void Instantiation::setFirst() {
    _overflow_ = false;
    Size s     = nbrDim();

    for (Idx p = 0; p < s; ++p)
      _vals_[p] = 0;

    _masterFirstNotification_();
  }

  // put the (D1-1,D2-1,...) last value in the Instantiation
  INLINE void Instantiation::setLast() {
    _overflow_ = false;
    Size s     = nbrDim();

    for (Idx p = 0; p < s; ++p)
      _vals_[p] = _vars_[p]->domainSize() - 1;

    _masterLastNotification_();
  }

  // operator ++ limited only to the variables in i
  INLINE void Instantiation::incIn(const Instantiation& i) {
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

  // operator -- limited only to the variables in i
  INLINE void Instantiation::decIn(const Instantiation& i) {
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

  // reorder vars in *this
  INLINE void Instantiation::reorder(const Instantiation& i) { reorder(i.variablesSequence()); }

  // put the (0,0,...) first value in the Instantiation for the variables in i
  INLINE void Instantiation::setFirstIn(const Instantiation& i) {
    _overflow_ = false;
    Idx s      = nbrDim();

    for (Size p = 0; p < s; ++p)
      if (i.contains(_vars_[p])) _chgVal_(p, 0);
  }

  // change values with those in i
  INLINE Instantiation& Instantiation::setVals(const Instantiation& i) {
    _overflow_ = false;
    Idx s      = i.nbrDim();

    for (Size p = 0; p < s; ++p)
      if (contains(i.variable(p))) _chgVal_(pos(i.variable(p)), i.val(p));

    return *this;
  }

  // put the (D1-1,D2-1,...) lastvalue in the Instantiation for variables in i
  INLINE void Instantiation::setLastIn(const Instantiation& i) {
    _overflow_ = false;
    Idx s      = nbrDim();

    for (Size p = 0; p < s; ++p)
      if (i.contains(_vars_[p])) _chgVal_(p, _vars_[p]->domainSize() - 1);
  }

  // operator ++ for the variables not in i
  INLINE void Instantiation::incOut(const Instantiation& i) {
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

  // operator -- for the variables not in i
  INLINE void Instantiation::decOut(const Instantiation& i) {
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

  // put the (0,0,...) first val in the Instantiation for the variables not in
  // i
  INLINE void Instantiation::setFirstOut(const Instantiation& i) {
    _overflow_ = false;
    Idx s      = nbrDim();

    for (Size p = 0; p < s; ++p)
      if (!i.contains(_vars_[p])) _chgVal_(p, 0);
  }

  // put the (D1-1,D2-1,...) lastvalue in the Instantiation for vars not in i
  INLINE void Instantiation::setLastOut(const Instantiation& i) {
    _overflow_ = false;
    Idx s      = nbrDim();

    for (Size p = 0; p < s; ++p)
      if (!i.contains(_vars_[p])) _chgVal_(p, _vars_[p]->domainSize() - 1);
  }

  // operator ++ for vars which are not v.
  INLINE void Instantiation::incNotVar(const DiscreteVariable& v) {
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

  // operator -- for vars which are not v.
  INLINE void Instantiation::decNotVar(const DiscreteVariable& v) {
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

  // assign the (0,0,...) first value to variables which are not v.
  INLINE void Instantiation::setFirstNotVar(const DiscreteVariable& v) {
    _overflow_ = false;
    Idx s      = nbrDim();

    for (Size p = 0; p < s; ++p) {
      if (_vars_[p] == &v) {
        Idx oldval = _vals_[p];
        setFirst();
        _chgVal_(p, oldval);
        return;
      }
    }

    setFirst();
  }

  // put the (D1-1,D2-1,...) lastvalue in the Instantiation for vars != v
  INLINE void Instantiation::setLastNotVar(const DiscreteVariable& v) {
    _overflow_ = false;
    Idx s      = nbrDim();

    for (Size p = 0; p < s; ++p) {
      if (_vars_[p] == &v) {
        Idx oldval = _vals_[p];
        setLast();
        _chgVal_(p, oldval);
        return;
      }
    }

    setLast();
  }

  // operator ++ for variable v only
  INLINE void Instantiation::incVar(const DiscreteVariable& v) {
    // get the position of the variable
    Idx cpt = _vars_.pos(&v);
    // if we are in overflow, do nothing

    if (_overflow_) return;

    Idx p = _vals_[cpt];

    if (p + 1 == v.domainSize()) {
      _chgVal_(cpt, 0);
      _overflow_ = true;
    } else {
      _chgVal_(cpt, p + 1);
    }
  }

  // operator -- for variable v only
  INLINE void Instantiation::decVar(const DiscreteVariable& v) {
    // get the position of the variable
    Idx cpt = _vars_.pos(&v);
    // if we are in overflow, do nothing

    if (_overflow_) return;

    Idx p = _vals_[cpt];

    if (p == 0) {
      _chgVal_(cpt, v.domainSize() - 1);
      _overflow_ = true;
    } else {
      _chgVal_(cpt, p - 1);
    }
  }

  // assign the first value in the Instantiation for var v.
  INLINE void Instantiation::setFirstVar(const DiscreteVariable& v) {
    _overflow_ = false;
    _chgVal_(_vars_.pos(&v), 0);
  }

  // assign the last value to var v.
  INLINE void Instantiation::setLastVar(const DiscreteVariable& v) {
    _overflow_ = false;
    _chgVal_(_vars_.pos(&v), v.domainSize() - 1);
  }

  // indicates whether the Instantiation has a master MultiDimAdressable
  INLINE bool Instantiation::isSlave() const { return (_master_ != nullptr); }

  // indicates wether the MultiDimAdressable* m is the master
  INLINE bool Instantiation::isMaster(const MultiDimAdressable* m) const { return (_master_ == m); }

  // indicates wether the MultiDimAdressable* m is the master
  INLINE bool Instantiation::isMaster(const MultiDimAdressable& m) const { return isMaster(&m); }

  // returns the sequence of DiscreteVariable
  INLINE const Sequence< const DiscreteVariable* >& Instantiation::variablesSequence() const {
    return _vars_;
  }


  // replace 2 vars in the Instantiation
  INLINE void Instantiation::_swap_(Idx i, Idx j) {
    if (i == j) return;

    _vars_.swap(i, j);

    Idx v;
    v         = _vals_[i];
    _vals_[i] = _vals_[j];
    _vals_[j] = v;
  }

  // reordering
  INLINE
  void Instantiation::reorder(const Sequence< const DiscreteVariable* >& original) {
    if (_master_ != nullptr) {
      GUM_ERROR(OperationNotAllowed, "Reordering impossible in slave instantiation")
    }

    _reorder_(original);
  }

  INLINE
  void Instantiation::_reorder_(const Sequence< const DiscreteVariable* >& original) {
    Idx max      = original.size();
    Idx position = 0;
    for (Idx i = 0; i < max; ++i) {
      const DiscreteVariable* pv = original.atPos(i);

      if (contains(pv)) {
        auto p = pos(*pv);
        GUM_ASSERT(p >= position);   // this var should not be
                                     // already placed.
        _swap_(position, p);
        position++;
      }
    }
  }


  // add new dim by master
  INLINE void Instantiation::addWithMaster(const MultiDimAdressable* m, const DiscreteVariable& v) {
    if (m != _master_) { GUM_ERROR(OperationNotAllowed, "only master can do this") }

    _add_(v);
  }


  // adds a new var to the sequence of vars
  INLINE void Instantiation::_add_(const DiscreteVariable& v) {
    _vars_.insert(&v);
    _vals_.push_back(0);
    _overflow_ = false;
  }

  // removes a variable from the sequence of vars
  INLINE void Instantiation::_erase_(const DiscreteVariable& v) {
    // get the position of the variable
    Idx pos = _vars_.pos(&v);
    _vars_.erase(&v);
    _vals_.erase(_vals_.begin() + pos);
  }

  // is this empty ?
  INLINE bool Instantiation::empty() const { return _vals_.empty(); }

  // Replace x by y.
  INLINE void Instantiation::replace_(const DiscreteVariable* x, const DiscreteVariable* y) {
    _vars_.setAtPos(_vars_.pos(x), y);
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by vectors of Idx
  INLINE Size HashFunc< Instantiation >::castToSize(const Instantiation& key) {
    Size h = Size(0);
    for (const DiscreteVariable* k:
         key.variablesSequence())   // k are unique only by address (not by name)
      h += HashFunc< const DiscreteVariable* >::castToSize(k) * Size(key.val(*k));

    return h;
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by vectors of Idx
  INLINE Size HashFunc< Instantiation >::operator()(const Instantiation& key) const {
    return castToSize(key) & this->hash_mask_;
  }

  INLINE bool Instantiation::operator==(const Instantiation& other) const {
    if (inOverflow() && other.inOverflow()) return true;
    if (other.nbrDim() != nbrDim()) return false;
    for (const auto& k: variablesSequence()) {
      if (!other.contains(k)) return false;
      if (val(*k) != other.val(*k)) return false;
    }
    return true;
  }
} /* namespace gum */
