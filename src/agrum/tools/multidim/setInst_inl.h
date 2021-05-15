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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/multidim/implementations/multiDimAdressable.h>

// to ease IDE PARSER
#  include <agrum/tools/multidim/setInst.h>

namespace gum {

  // indicates whether a given variable belongs to the SetInst

  INLINE bool SetInst::contains(const DiscreteVariable& v) const {
    return vars__.exists(&v);
  }

  // indicates whether a given variable belongs to the SetInst

  INLINE bool SetInst::contains(const DiscreteVariable* v) const {
    return vars__.exists(v);
  }

  // modifies internally the value of a given variable of the sequence

  INLINE void SetInst::chgVal__(Idx varPos, Idx newVal) {
    //  Size oldVal = vals__[varPos];
    vals__[varPos] = Idx(1) << newVal;

    //    if ( master__ )
    //      master__->changeNotification( *this, vars__[varPos], oldVal, newVal
    //      );
  }

  // modifies the value of a given variable of the sequence (external function)

  INLINE SetInst& SetInst::chgVal(const DiscreteVariable& v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      chgVal__(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::chgVal(const DiscreteVariable* v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      chgVal__(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  // modifies the value of a given variable of the sequence (external function)

  INLINE SetInst& SetInst::chgVal(Idx varPos, Idx newVal) {
    // check that the variable does belong to the SetInst and that the new
    // value is possible.
    if (vals__.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= vars__[varPos]->domainSize()) GUM_ERROR(OutOfBounds, "")

    // if we were in overflow, indicate that we are not anymore
    overflow__ = false;

    chgVal__(varPos, newVal);

    return *this;
  }

  // modifies internally the value of a given variable of the sequence

  INLINE void SetInst::chgVals__(Idx varPos, const Size newVals) {
    //   Size oldVal = vals__[varPos];
    vals__[varPos] = 0;
    vals__[varPos] = newVals;

    //   if ( master__ )
    //     master__->changeNotification( *this, vars__[varPos], oldVal, newVals
    //     );
  }

  // modifies the value of a given variable of the sequence (external function)

  INLINE SetInst& SetInst::chgVals(const DiscreteVariable& v, const Size newVals) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVals >= (Size)1 << v.domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      chgVals__(varPos, newVals);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::chgVals(const DiscreteVariable* v, const Size newVals) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVals >= (Size)1 << v->domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      chgVals__(varPos, newVals);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  // modifies the value of a given variable of the sequence (external function)

  INLINE SetInst& SetInst::chgVals(Idx varPos, const Size newVal) {
    // check that the variable does belong to the SetInst and that the new
    // value is possible.
    if (vals__.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= (Size)1 << vars__[varPos]->domainSize())

      GUM_ERROR(OutOfBounds, "")

    // if we were in overflow, indicate that we are not anymore
    overflow__ = false;

    chgVals__(varPos, newVal);

    return *this;
  }

  INLINE SetInst& SetInst::addVal(Idx varPos, Idx newVal) {
    if (vals__.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= vars__[varPos]->domainSize()) GUM_ERROR(OutOfBounds, "")

    chgVals__(varPos, (Idx(1) << newVal) | vals__[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::addVal(const DiscreteVariable* v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      addVal(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  INLINE SetInst& SetInst::addVal(const DiscreteVariable& v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      addVal(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::addVals(Idx varPos, const Size newVal) {
    if (vals__.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= (Size(1) << vars__[varPos]->domainSize()))
      GUM_ERROR(OutOfBounds, "")

    chgVals__(varPos, newVal | vals__[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::addVals(const DiscreteVariable* v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= (Size(1) << vars__[varPos]->domainSize()))

        GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore

      addVals(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  INLINE SetInst& SetInst::addVals(const DiscreteVariable& v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= (Size(1) << vars__[varPos]->domainSize()))

        GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      addVals(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::remVal(Idx varPos, Idx newVal) {
    if (vals__.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= vars__[varPos]->domainSize()) GUM_ERROR(OutOfBounds, "")

    chgVals__(varPos, ~(1 << newVal) & vals__[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::remVal(const DiscreteVariable* v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore

      remVal(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  INLINE SetInst& SetInst::remVal(const DiscreteVariable& v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      remVal(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::remVals(Idx varPos, const Size newVal) {
    if (vals__.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= (Size(1) << vars__[varPos]->domainSize()))
      GUM_ERROR(OutOfBounds, "")

    chgVals__(varPos, ~newVal & vals__[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::remVals(const DiscreteVariable* v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= (Size(1) << vars__[varPos]->domainSize()))

        GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore

      remVals(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  INLINE SetInst& SetInst::remVals(const DiscreteVariable& v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= (Size(1) << vars__[varPos]->domainSize()))

        GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      remVals(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::chgDifVal(Idx varPos, const Size newVal) {
    if (vals__.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= vars__[varPos]->domainSize()) GUM_ERROR(OutOfBounds, "")

    chgVals__(varPos, newVal ^ vals__[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::interVals(Idx varPos, const Size newVal) {
    if (vals__.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= (Size(1) << vars__[varPos]->domainSize()))
      GUM_ERROR(OutOfBounds, "")

    chgVals__(varPos, newVal & vals__[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::interVals(const DiscreteVariable* v,
                                     const Size              newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= (Size(1) << vars__[varPos]->domainSize()))

        GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore

      interVals(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  INLINE SetInst& SetInst::interVals(const DiscreteVariable& v,
                                     const Size              newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= (Size(1) << vars__[varPos]->domainSize()))

        GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      interVals(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE SetInst& SetInst::interVal(Idx varPos, Idx newVal) {
    if (vals__.size() <= varPos) GUM_ERROR(NotFound, "")

    if (newVal >= vars__[varPos]->domainSize()) GUM_ERROR(OutOfBounds, "")

    chgVals__(varPos, (Size(1) << newVal) & vals__[varPos]);
    return *this;
  }

  INLINE SetInst& SetInst::interVal(const DiscreteVariable* v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore

      interVal(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  INLINE SetInst& SetInst::interVal(const DiscreteVariable& v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = vars__.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize()) GUM_ERROR(OutOfBounds, "")

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      interVal(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  // adds a new var to the sequence of vars

  INLINE void SetInst::add(const DiscreteVariable& v) {
    // if master__ : not allowed
    // if ( master__ ) GUM_ERROR( OperationNotAllowed, "in slave SetInst" )

    // check if the variable already belongs to the tuple of variables
    // of the SetInst
    if (vars__.exists(&v))
      GUM_ERROR(DuplicateElement,
                "Variable '" << v.name() << "' already exists in this SetInst")

    // actually add the new dimension
    add__(v);
  }

  // removes a variable from the sequence of vars

  INLINE void SetInst::erase(const DiscreteVariable& v) {
    // if master__ : not allowed
    // if ( master__ ) GUM_ERROR( OperationNotAllowed, "in slave SetInst" )

    // check that the variable does actually belong to the SetInst
    if (!vars__.exists(&v))
      GUM_ERROR(NotFound, "Var does not exist in this SetInst")

    // actually delete the dimension
    erase__(v);
  }

  // removes everything
  INLINE void SetInst::clear() {
    // if ( master__ ) GUM_ERROR( OperationNotAllowed, "in slave SetInst" )

    vars__.clear();
    vals__.clear();
  }

  /** @brief returns the product of the size of the domains of the variables
   * belonging to the matrix */

  INLINE Size SetInst::domainSize() const {
    Size s = 1;

    for (const auto var: vars__)
      s *= var->domainSize();

    return s;
  }

  // returns the index of a var

  INLINE Idx SetInst::pos(const DiscreteVariable& k) const {
    return vars__.pos(&k);
  }

  // Default constructor

  INLINE SetInst::SetInst() : /*master__( 0 ),*/ overflow__(false) {
    GUM_CONSTRUCTOR(SetInst);
  }

  // destructor

  INLINE SetInst::~SetInst() {
    GUM_DESTRUCTOR(SetInst);
    // unregister the SetInst from its master__

    // if ( master__ )  master__->unregisterSlave( *this );
  }

  // returns the number of vars in the sequence

  INLINE Idx SetInst::nbrDim() const { return vars__.size(); }

  // returns the current value of a given variable

  INLINE Size SetInst::vals(Idx i) const {
    if (i >= vals__.size()) GUM_ERROR(NotFound, "")

    return vals__[i];
  }

  INLINE Size SetInst::vals(const DiscreteVariable& var) const {
    return vals__[vars__.pos(&var)];
  }

  INLINE Size SetInst::vals(const DiscreteVariable* var) const {
    return vals__[vars__.pos(var)];
  }

  INLINE Idx SetInst::val(const DiscreteVariable* var) const {
    Idx  n     = 0;
    Size value = vals__[vars__.pos(var)];

    if (vals__[vars__.pos(var)] % 2 == 0) {
      while (value & 1) {
        n++;
        value >>= 1;
      }

      return n;
    } else
      GUM_ERROR(NotFound, "There is more than one value ")
  }

  INLINE Idx SetInst::nbrOccurences(const DiscreteVariable& var) const {
    Idx  n   = 0;
    Size val = vals__[vars__.pos(&var)];

    while (val) {
      n += (val & 1);
      val >>= 1;
    }

    return n;
  }

  INLINE Idx SetInst::val(const DiscreteVariable& var) const {
    Idx  n     = 0;
    Size value = vals__[vars__.pos(&var)];

    if (nbrOccurences(var) == 1) {
      while (value > 1) {
        n++;
        value >>= 1;
      }

      return n;
    } else
      GUM_ERROR(NotFound, "There is more than one value ")
  }

  // returns the variable at position i in the tuple

  INLINE const DiscreteVariable& SetInst::variable(Idx i) const {
    return *(vars__.atPos(i));
  }

  // indicates whether the current value of the tuple is correct or not

  INLINE bool SetInst::inOverflow() const { return overflow__; }

  // end() just is a synonym for inOverflow()

  INLINE bool SetInst::end() const { return inOverflow(); }

  // rend() just is a synonym for inOverflow()

  INLINE bool SetInst::rend() const { return inOverflow(); }

  // indicates that the current value is correct even if it should be in
  // overflow

  INLINE void SetInst::unsetOverflow() { overflow__ = false; }

  // alias for unsetOverflow

  INLINE void SetInst::unsetEnd() { overflow__ = false; }

  // reorder vars in *this
  INLINE void SetInst::reorder(const SetInst& i) {
    reorder(i.variablesSequence());
  }

  // change values with those in i

  INLINE SetInst& SetInst::chgValIn(const SetInst& i) {
    overflow__ = false;
    Idx s      = i.nbrDim();

    for (Size p = 0; p < s; ++p)
      if (contains(i.variable(p)))
        //__vals[pos( i.variable( p ) )] = i.val( p );
        chgVals__(pos(i.variable(p)), i.vals(i.variable(p)));

    return *this;
  }

  // returns the sequence of DiscreteVariable

  INLINE const Sequence< const DiscreteVariable* >&
               SetInst::variablesSequence() const {
    return vars__;
  }

  // replace 2 vars in the SetInst

  INLINE void SetInst::swap__(Idx i, Idx j) {
    if (i == j) return;

    vars__.swap(i, j);

    Size v;

    v = vals__[i];

    vals__[i] = vals__[j];

    vals__[j] = v;
  }

  // reordering

  INLINE void
     SetInst::reorder(const Sequence< const DiscreteVariable* >& original) {
    Idx max      = original.size();
    Idx position = 0;

    for (Idx i = 0; i < max; ++i) {
      const DiscreteVariable* pv = original.atPos(i);

      if (contains(pv)) {
        GUM_ASSERT(pos(*pv) >= position);   // this var should not be
        // already placed.
        swap__(position, pos(*pv));
        position++;
      }
    }
  }

  // adds a new var to the sequence of vars

  INLINE void SetInst::add__(const DiscreteVariable& v) {
    vars__.insert(&v);
    vals__.push_back(1);
    overflow__ = false;
  }

  // removes a variable from the sequence of vars

  INLINE void SetInst::erase__(const DiscreteVariable& v) {
    // get the position of the variable
    Idx pos = vars__.pos(&v);
    vars__.erase(&v);
    vals__.erase(vals__.begin() + pos);
  }

  // is this empty ?
  INLINE bool SetInst::empty() const { return vals__.empty(); }

  // Replace x by y.
  INLINE void SetInst::replace_(const DiscreteVariable* x,
                                const DiscreteVariable* y) {
    vars__.setAtPos(vars__.pos(x), y);
  }
} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
