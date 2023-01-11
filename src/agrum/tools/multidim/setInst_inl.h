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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/multidim/implementations/multiDimAdressable.h>

// to ease IDE PARSER
#  include <agrum/tools/multidim/setInst.h>

namespace gum {

  // indicates whether a given variable belongs to the SetInst

  INLINE bool SetInst::contains(const DiscreteVariable& v) const { return _vars_.exists(&v); }

  // indicates whether a given variable belongs to the SetInst

  INLINE bool SetInst::contains(const DiscreteVariable* v) const { return _vars_.exists(v); }

  // modifies internally the value of a given variable of the sequence

  INLINE void SetInst::_chgVal_(Idx varPos, Idx newVal) {
    //  Size oldVal =  _vals_[varPos];
    _vals_[varPos] = Idx(1) << newVal;

    //    if (  _master_ )
    //       _master_->changeNotification( *this,  _vars_[varPos], oldVal, newVal
    //      );
  }

  // modifies the value of a given variable of the sequence (external function)

  INLINE SetInst& SetInst::chgVal(const DiscreteVariable& v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      _chgVal_(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::chgVal(const DiscreteVariable* v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      _chgVal_(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  // modifies the value of a given variable of the sequence (external function)

  INLINE SetInst& SetInst::chgVal(Idx varPos, Idx newVal) {
    // check that the variable does belong to the SetInst and that the new
    // value is possible.
    if (_vals_.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= _vars_[varPos]->domainSize()) GUM_ERROR(OutOfBounds, "")

    // if we were in overflow, indicate that we are not anymore
    _overflow_ = false;

    _chgVal_(varPos, newVal);

    return *this;
  }

  // modifies internally the value of a given variable of the sequence

  INLINE void SetInst::_chgVals_(Idx varPos, const Size newVals) {
    //   Size oldVal =  _vals_[varPos];
    _vals_[varPos] = 0;
    _vals_[varPos] = newVals;

    //   if (  _master_ )
    //      _master_->changeNotification( *this,  _vars_[varPos], oldVal, newVals
    //     );
  }

  // modifies the value of a given variable of the sequence (external function)

  INLINE SetInst& SetInst::chgVals(const DiscreteVariable& v, const Size newVals) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVals >= (Size)1 << v.domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      _chgVals_(varPos, newVals);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::chgVals(const DiscreteVariable* v, const Size newVals) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVals >= (Size)1 << v->domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      _chgVals_(varPos, newVals);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  // modifies the value of a given variable of the sequence (external function)

  INLINE SetInst& SetInst::chgVals(Idx varPos, const Size newVal) {
    // check that the variable does belong to the SetInst and that the new
    // value is possible.
    if (_vals_.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= (Size)1 << _vars_[varPos]->domainSize()) GUM_ERROR(OutOfBounds, "")

    // if we were in overflow, indicate that we are not anymore
    _overflow_ = false;

    _chgVals_(varPos, newVal);

    return *this;
  }

  INLINE SetInst& SetInst::addVal(Idx varPos, Idx newVal) {
    if (_vals_.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= _vars_[varPos]->domainSize()) GUM_ERROR(OutOfBounds, "")

    _chgVals_(varPos, (Idx(1) << newVal) | _vals_[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::addVal(const DiscreteVariable* v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      addVal(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  INLINE SetInst& SetInst::addVal(const DiscreteVariable& v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      addVal(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::addVals(Idx varPos, const Size newVal) {
    if (_vals_.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= (Size(1) << _vars_[varPos]->domainSize())) GUM_ERROR(OutOfBounds, "")

    _chgVals_(varPos, newVal | _vals_[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::addVals(const DiscreteVariable* v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= (Size(1) << _vars_[varPos]->domainSize())) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore

      addVals(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  INLINE SetInst& SetInst::addVals(const DiscreteVariable& v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= (Size(1) << _vars_[varPos]->domainSize())) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      addVals(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::remVal(Idx varPos, Idx newVal) {
    if (_vals_.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= _vars_[varPos]->domainSize()) GUM_ERROR(OutOfBounds, "")

    _chgVals_(varPos, ~(1 << newVal) & _vals_[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::remVal(const DiscreteVariable* v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore

      remVal(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  INLINE SetInst& SetInst::remVal(const DiscreteVariable& v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      remVal(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::remVals(Idx varPos, const Size newVal) {
    if (_vals_.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= (Size(1) << _vars_[varPos]->domainSize())) GUM_ERROR(OutOfBounds, "")

    _chgVals_(varPos, ~newVal & _vals_[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::remVals(const DiscreteVariable* v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= (Size(1) << _vars_[varPos]->domainSize())) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore

      remVals(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  INLINE SetInst& SetInst::remVals(const DiscreteVariable& v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= (Size(1) << _vars_[varPos]->domainSize())) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      remVals(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::chgDifVal(Idx varPos, const Size newVal) {
    if (_vals_.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= _vars_[varPos]->domainSize()) GUM_ERROR(OutOfBounds, "")

    _chgVals_(varPos, newVal ^ _vals_[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::interVals(Idx varPos, const Size newVal) {
    if (_vals_.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= (Size(1) << _vars_[varPos]->domainSize())) GUM_ERROR(OutOfBounds, "")

    _chgVals_(varPos, newVal & _vals_[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::interVals(const DiscreteVariable* v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= (Size(1) << _vars_[varPos]->domainSize())) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore

      interVals(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  INLINE SetInst& SetInst::interVals(const DiscreteVariable& v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= (Size(1) << _vars_[varPos]->domainSize())) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      interVals(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::interVal(Idx varPos, Idx newVal) {
    if (_vals_.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= _vars_[varPos]->domainSize()) GUM_ERROR(OutOfBounds, "")

    _chgVals_(varPos, (Size(1) << newVal) & _vals_[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::interVal(const DiscreteVariable* v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore

      interVal(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  INLINE SetInst& SetInst::interVal(const DiscreteVariable& v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = _vars_.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      _overflow_ = false;

      interVal(varPos, newVal);

      return *this;
    } catch (NotFound const&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  // adds a new var to the sequence of vars

  INLINE void SetInst::add(const DiscreteVariable& v) {
    // if  _master_ : not allowed
    // if (  _master_ ) GUM_ERROR( OperationNotAllowed, "in slave SetInst" )

    // check if the variable already belongs to the tuple of variables
    // of the SetInst
    if (_vars_.exists(&v))
      GUM_ERROR(DuplicateElement, "Variable '" << v.name() << "' already exists in this SetInst")

    // actually add the new dimension
    _add_(v);
  }

  // removes a variable from the sequence of vars

  INLINE void SetInst::erase(const DiscreteVariable& v) {
    // if  _master_ : not allowed
    // if (  _master_ ) GUM_ERROR( OperationNotAllowed, "in slave SetInst" )

    // check that the variable does actually belong to the SetInst
    if (!_vars_.exists(&v)) GUM_ERROR(NotFound, "Var does not exist in this SetInst")

    // actually delete the dimension
    _erase_(v);
  }

  // removes everything
  INLINE void SetInst::clear() {
    // if (  _master_ ) GUM_ERROR( OperationNotAllowed, "in slave SetInst" )

    _vars_.clear();
    _vals_.clear();
  }

  /** @brief returns the product of the size of the domains of the variables
   * belonging to the matrix */

  INLINE Size SetInst::domainSize() const {
    Size s = 1;

    for (const auto var: _vars_)
      s *= var->domainSize();

    return s;
  }

  // returns the index of a var

  INLINE Idx SetInst::pos(const DiscreteVariable& k) const { return _vars_.pos(&k); }

  // Default constructor

  INLINE SetInst::SetInst() : /* _master_( 0 ),*/ _overflow_(false) { GUM_CONSTRUCTOR(SetInst); }

  // destructor

  INLINE SetInst::~SetInst() {
    GUM_DESTRUCTOR(SetInst);
    // unregister the SetInst from its  _master_

    // if (  _master_ )   _master_->unregisterSlave( *this );
  }

  // returns the number of vars in the sequence

  INLINE Idx SetInst::nbrDim() const { return _vars_.size(); }

  // returns the current value of a given variable

  INLINE Size SetInst::vals(Idx i) const {
    if (i >= _vals_.size()) GUM_ERROR(NotFound, "")

    return _vals_[i];
  }

  INLINE Size SetInst::vals(const DiscreteVariable& var) const { return _vals_[_vars_.pos(&var)]; }

  INLINE Size SetInst::vals(const DiscreteVariable* var) const { return _vals_[_vars_.pos(var)]; }

  INLINE Idx SetInst::val(const DiscreteVariable* var) const {
    Idx  n     = 0;
    Size value = _vals_[_vars_.pos(var)];

    if (_vals_[_vars_.pos(var)] % 2 == 0) {
      while (value & 1) {
        n++;
        value >>= 1;
      }

      return n;
    } else GUM_ERROR(NotFound, "There is more than one value ")
  }

  INLINE Idx SetInst::nbrOccurences(const DiscreteVariable& var) const {
    Idx  n   = 0;
    Size val = _vals_[_vars_.pos(&var)];

    while (val) {
      n += (val & 1);
      val >>= 1;
    }

    return n;
  }

  INLINE Idx SetInst::val(const DiscreteVariable& var) const {
    Idx  n     = 0;
    Size value = _vals_[_vars_.pos(&var)];

    if (nbrOccurences(var) == 1) {
      while (value > 1) {
        n++;
        value >>= 1;
      }

      return n;
    } else GUM_ERROR(NotFound, "There is more than one value ")
  }

  // returns the variable at position i in the tuple

  INLINE const DiscreteVariable& SetInst::variable(Idx i) const { return *(_vars_.atPos(i)); }

  // indicates whether the current value of the tuple is correct or not

  INLINE bool SetInst::inOverflow() const { return _overflow_; }

  // end() just is a synonym for inOverflow()

  INLINE bool SetInst::end() const { return inOverflow(); }

  // rend() just is a synonym for inOverflow()

  INLINE bool SetInst::rend() const { return inOverflow(); }

  // indicates that the current value is correct even if it should be in
  // overflow

  INLINE void SetInst::unsetOverflow() { _overflow_ = false; }

  // alias for unsetOverflow

  INLINE void SetInst::unsetEnd() { _overflow_ = false; }

  // reorder vars in *this
  INLINE void SetInst::reorder(const SetInst& i) { reorder(i.variablesSequence()); }

  // change values with those in i

  INLINE SetInst& SetInst::chgValIn(const SetInst& i) {
    _overflow_ = false;
    Idx s      = i.nbrDim();

    for (Size p = 0; p < s; ++p)
      if (contains(i.variable(p)))
        // __vals[pos( i.variable( p ) )] = i.val( p );
        _chgVals_(pos(i.variable(p)), i.vals(i.variable(p)));

    return *this;
  }

  // returns the sequence of DiscreteVariable

  INLINE const Sequence< const DiscreteVariable* >& SetInst::variablesSequence() const {
    return _vars_;
  }

  // replace 2 vars in the SetInst

  INLINE void SetInst::_swap_(Idx i, Idx j) {
    if (i == j) return;

    _vars_.swap(i, j);

    Size v;

    v = _vals_[i];

    _vals_[i] = _vals_[j];

    _vals_[j] = v;
  }

  // reordering

  INLINE void SetInst::reorder(const Sequence< const DiscreteVariable* >& original) {
    Idx max      = original.size();
    Idx position = 0;

    for (Idx i = 0; i < max; ++i) {
      const DiscreteVariable* pv = original.atPos(i);

      if (contains(pv)) {
        GUM_ASSERT(pos(*pv) >= position);   // this var should not be
                                            // already placed.
        _swap_(position, pos(*pv));
        position++;
      }
    }
  }

  // adds a new var to the sequence of vars

  INLINE void SetInst::_add_(const DiscreteVariable& v) {
    _vars_.insert(&v);
    _vals_.push_back(1);
    _overflow_ = false;
  }

  // removes a variable from the sequence of vars

  INLINE void SetInst::_erase_(const DiscreteVariable& v) {
    // get the position of the variable
    Idx pos = _vars_.pos(&v);
    _vars_.erase(&v);
    _vals_.erase(_vals_.begin() + pos);
  }

  // is this empty ?
  INLINE bool SetInst::empty() const { return _vals_.empty(); }

  // Replace x by y.
  INLINE void SetInst::replace_(const DiscreteVariable* x, const DiscreteVariable* y) {
    _vars_.setAtPos(_vars_.pos(x), y);
  }
} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
