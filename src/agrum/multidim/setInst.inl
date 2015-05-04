/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/multidim/multiDimAdressable.h>

// to ease IDE PARSER
#include <agrum/multidim/setInst.h>

namespace gum {

  /// indicates whether a given variable belongs to the SetInst

  INLINE bool SetInst::contains(const DiscreteVariable &v) const {
    return __vars.exists(&v);
  }

  /// indicates whether a given variable belongs to the SetInst

  INLINE bool SetInst::contains(const DiscreteVariable *v) const {
    return __vars.exists(v);
  }

  /// modifies internally the value of a given variable of the sequence

  INLINE void SetInst::__chgVal(Idx varPos, Idx newVal) {

    //  Size oldVal = __vals[varPos];
    __vals[varPos] = 1 << newVal;

    //    if ( __master )
    //      __master->changeNotification( *this, __vars[varPos], oldVal, newVal );
  }

  /// modifies the value of a given variable of the sequence (external function)

  INLINE SetInst &SetInst::chgVal(const DiscreteVariable &v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(&v); // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize())
        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      __chgVal(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name());
    }
  }

  INLINE SetInst &SetInst::chgVal(const DiscreteVariable *v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(v); // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize())
        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      __chgVal(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name());
    }
  }

  /// modifies the value of a given variable of the sequence (external function)

  INLINE SetInst &SetInst::chgVal(Idx varPos, Idx newVal) {
    // check that the variable does belong to the SetInst and that the new
    // value is possible.
    if (__vals.size() <= varPos)
      GUM_ERROR(NotFound, "");

    if (newVal >= __vars[varPos]->domainSize())
      GUM_ERROR(OutOfBounds, "");

    // if we were in overflow, indicate that we are not anymore
    __overflow = false;

    __chgVal(varPos, newVal);

    return *this;
  }

  /// modifies internally the value of a given variable of the sequence

  INLINE void SetInst::__chgVals(Idx varPos, const Size newVals) {

    //   Size oldVal = __vals[varPos];
    __vals[varPos] = 0;
    __vals[varPos] = newVals;

    //   if ( __master )
    //     __master->changeNotification( *this, __vars[varPos], oldVal, newVals );
  }

  /// modifies the value of a given variable of the sequence (external function)

  INLINE SetInst &SetInst::chgVals(const DiscreteVariable &v, const Size newVals) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(&v); // throws NotFound if v doesn't belong to this

      if (newVals >= (Size)1 << v.domainSize())
        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      __chgVals(varPos, newVals);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name());
    }
  }

  INLINE SetInst &SetInst::chgVals(const DiscreteVariable *v, const Size newVals) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(v); // throws NotFound if v doesn't belong to this

      if (newVals >= (Size)1 << v->domainSize())
        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      __chgVals(varPos, newVals);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name());
    }
  }

  /// modifies the value of a given variable of the sequence (external function)

  INLINE SetInst &SetInst::chgVals(Idx varPos, const Size newVal) {
    // check that the variable does belong to the SetInst and that the new
    // value is possible.
    if (__vals.size() <= varPos)
      GUM_ERROR(NotFound, "");

    if (newVal >= (Size)1 << __vars[varPos]->domainSize())

      GUM_ERROR(OutOfBounds, "");

    // if we were in overflow, indicate that we are not anymore
    __overflow = false;

    __chgVals(varPos, newVal);

    return *this;
  }

  INLINE SetInst &SetInst::addVal(Idx varPos, Idx newVal) {
    if (__vals.size() <= varPos)
      GUM_ERROR(NotFound, "");

    if (newVal >= __vars[varPos]->domainSize())
      GUM_ERROR(OutOfBounds, "");

    __chgVals(varPos, (1 << newVal) | __vals[varPos]);
    return *this;
  }

  INLINE SetInst &SetInst::addVal(const DiscreteVariable *v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(v); // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize())
        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      addVal(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name());
    }
  }

  INLINE SetInst &SetInst::addVal(const DiscreteVariable &v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(&v); // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize())
        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      addVal(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name());
    }
  }

  INLINE SetInst &SetInst::addVals(Idx varPos, const Size newVal) {
    if (__vals.size() <= varPos)
      GUM_ERROR(NotFound, "");

    if (newVal >= (1UL << __vars[varPos]->domainSize()))
      GUM_ERROR(OutOfBounds, "");

    __chgVals(varPos, newVal | __vals[varPos]);
    return *this;
  }

  INLINE SetInst &SetInst::addVals(const DiscreteVariable *v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(v); // throws NotFound if v doesn't belong to this

      if (newVal >= (1UL << __vars[varPos]->domainSize()))

        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore

      addVals(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name());
    }
  }

  INLINE SetInst &SetInst::addVals(const DiscreteVariable &v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(&v); // throws NotFound if v doesn't belong to this

      if (newVal >= (1UL << __vars[varPos]->domainSize()))

        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      addVals(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name());
    }
  }

  INLINE SetInst &SetInst::remVal(Idx varPos, Idx newVal) {
    if (__vals.size() <= varPos)
      GUM_ERROR(NotFound, "");

    if (newVal >= __vars[varPos]->domainSize())
      GUM_ERROR(OutOfBounds, "");

    __chgVals(varPos, ~(1 << newVal) & __vals[varPos]);
    return *this;
  }

  INLINE SetInst &SetInst::remVal(const DiscreteVariable *v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(v); // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize())
        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore

      remVal(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name());
    }
  }

  INLINE SetInst &SetInst::remVal(const DiscreteVariable &v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(&v); // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize())
        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      remVal(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name());
    }
  }

  INLINE SetInst &SetInst::remVals(Idx varPos, const Size newVal) {
    if (__vals.size() <= varPos)
      GUM_ERROR(NotFound, "");

    if (newVal >= (1UL << __vars[varPos]->domainSize()))
      GUM_ERROR(OutOfBounds, "");

    __chgVals(varPos, ~newVal & __vals[varPos]);
    return *this;
  }

  INLINE SetInst &SetInst::remVals(const DiscreteVariable *v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(v); // throws NotFound if v doesn't belong to this

      if (newVal >= (1UL << __vars[varPos]->domainSize()))

        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore

      remVals(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name());
    }
  }

  INLINE SetInst &SetInst::remVals(const DiscreteVariable &v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(&v); // throws NotFound if v doesn't belong to this

      if (newVal >= (1UL << __vars[varPos]->domainSize()))

        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      remVals(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name());
    }
  }

  INLINE SetInst &SetInst::chgDifVal(Idx varPos, const Size newVal) {
    if (__vals.size() <= varPos)
      GUM_ERROR(NotFound, "");

    if (newVal >= __vars[varPos]->domainSize())
      GUM_ERROR(OutOfBounds, "");

    __chgVals(varPos, newVal ^ __vals[varPos]);
    return *this;
  }

  INLINE SetInst &SetInst::interVals(Idx varPos, const Size newVal) {
    if (__vals.size() <= varPos)
      GUM_ERROR(NotFound, "");

    if (newVal >= (1UL << __vars[varPos]->domainSize()))
      GUM_ERROR(OutOfBounds, "");

    __chgVals(varPos, newVal & __vals[varPos]);
    return *this;
  }

  INLINE SetInst &SetInst::interVals(const DiscreteVariable *v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(v); // throws NotFound if v doesn't belong to this

      if (newVal >= (1UL << __vars[varPos]->domainSize()))

        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore

      interVals(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name());
    }
  }

  INLINE SetInst &SetInst::interVals(const DiscreteVariable &v, const Size newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(&v); // throws NotFound if v doesn't belong to this

      if (newVal >= (1UL << __vars[varPos]->domainSize()))

        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      interVals(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name());
    }
  }

  INLINE SetInst &SetInst::interVal(Idx varPos, Idx newVal) {
    if (__vals.size() <= varPos)
      GUM_ERROR(NotFound, "");

    if (newVal >= __vars[varPos]->domainSize())
      GUM_ERROR(OutOfBounds, "");

    __chgVals(varPos, (1 << newVal) & __vals[varPos]);
    return *this;
  }

  INLINE SetInst &SetInst::interVal(const DiscreteVariable *v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(v); // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize())
        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore

      interVal(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name());
    }
  }

  INLINE SetInst &SetInst::interVal(const DiscreteVariable &v, Idx newVal) {
    try {
      // check that the variable does belong to the SetInst and that the new
      // value is possible.
      Idx varPos = __vars.pos(&v); // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize())
        GUM_ERROR(OutOfBounds, "");

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      interVal(varPos, newVal);

      return *this;
    } catch (NotFound &) {
      std::string name = "SetInst does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name());
    }
  }

  /// adds a new var to the sequence of vars

  INLINE void SetInst::add(const DiscreteVariable &v) {
    // if __master : not allowed
    // if ( __master ) GUM_ERROR( OperationNotAllowed, "in slave SetInst" );

    // check if the variable already belongs to the tuple of variables
    // of the SetInst
    if (__vars.exists(&v))
      GUM_ERROR(DuplicateElement, "Var already exists in this SetInst");

    // actually add the new dimension
    __add(v);
  }

  /// removes a variable from the sequence of vars

  INLINE void SetInst::erase(const DiscreteVariable &v) {
    // if __master : not allowed
    // if ( __master ) GUM_ERROR( OperationNotAllowed, "in slave SetInst" );

    // check that the variable does actually belong to the SetInst
    if (!__vars.exists(&v))
      GUM_ERROR(NotFound, "Var does not exist in this SetInst");

    // actually delete the dimension
    __erase(v);
  }

  /// removes everything
  INLINE void SetInst::clear(void) {
    // if ( __master ) GUM_ERROR( OperationNotAllowed, "in slave SetInst" );

    __vars.clear();
    __vals.clear();
  }

  /** @brief returns the product of the size of the domains of the variables
   * belonging to the matrix */

  INLINE Size SetInst::domainSize() const {
    // @todo enhance the cplxity with a member domainSize ?
    Size s = 1;

    for (const auto var : __vars)
      s *= var->domainSize();

    return s;
  }

  /// returns the index of a var

  INLINE Idx SetInst::pos(const DiscreteVariable &k) const { return __vars.pos(&k); }

  /// Default constructor

  INLINE SetInst::SetInst() : /*__master( 0 ),*/ __overflow(false) {
    GUM_CONSTRUCTOR(SetInst);
  }

  /// destructor

  INLINE SetInst::~SetInst() {
    GUM_DESTRUCTOR(SetInst);
    // unregister the SetInst from its __master

    // if ( __master )  __master->unregisterSlave( *this );
  }

  /// returns the number of vars in the sequence

  INLINE Idx SetInst::nbrDim() const { return __vars.size(); }

  /// returns the current value of a given variable

  INLINE Size SetInst::vals(Idx i) const {
    if (i >= __vals.size())
      GUM_ERROR(NotFound, "");

    return __vals[i];
  }

  /// returns the current value of a given variable
  /// need to create functions TODO

  INLINE Size SetInst::vals(const DiscreteVariable &var) const {
    return __vals[__vars.pos(&var)];
  }

  /// returns the current value of a given variable

  INLINE Size SetInst::vals(const DiscreteVariable *var) const {
    return __vals[__vars.pos(var)];
  }

  INLINE Idx SetInst::val(const DiscreteVariable *var) const {
    Idx n = 0;
    Size value = __vals[__vars.pos(var)];

    if (!__vals[__vars.pos(var)] % 2) {
      while (value & 1) {
        n++;
        value >>= 1;
      }

      return n;
    } else
      GUM_ERROR(NotFound, "There is more than one value ");
  }

  INLINE Idx SetInst::nbrOccurences(const DiscreteVariable &var) const {
    Idx n = 0;
    Size val = __vals[__vars.pos(&var)];

    while (val) {
      n += (val & 1);
      val >>= 1;
    }

    return n;
  }

  INLINE Idx SetInst::val(const DiscreteVariable &var) const {
    Idx n = 0;
    Size value = __vals[__vars.pos(&var)];

    if (nbrOccurences(var) == 1) {
      while (value > 1) {
        n++;
        value >>= 1;
      }

      return n;
    } else
      GUM_ERROR(NotFound, "There is more than one value ");
  }

  /// returns the variable at position i in the tuple

  INLINE const DiscreteVariable &SetInst::variable(Idx i) const {
    return *(__vars.atPos(i));
  }

  /// indicates whether the current value of the tuple is correct or not

  INLINE bool SetInst::inOverflow() const { return __overflow; }

  /// end() just is a synonym for inOverflow()

  INLINE bool SetInst::end() const { return inOverflow(); }

  /// rend() just is a synonym for inOverflow()

  INLINE bool SetInst::rend() const { return inOverflow(); }

  /// indicates that the current value is correct even if it should be in overflow

  INLINE void SetInst::unsetOverflow() { __overflow = false; }

  /// alias for unsetOverflow

  INLINE void SetInst::unsetEnd() { __overflow = false; }

  /// reorder vars in *this
  INLINE void SetInst::reorder(const SetInst &i) { reorder(i.variablesSequence()); }

  /// change values with those in i

  INLINE SetInst &SetInst::chgValIn(const SetInst &i) {
    __overflow = false;
    Idx s = i.nbrDim();

    for (Size p = 0; p < s; ++p)
      if (contains(i.variable(p)))
        //__vals[pos( i.variable( p ) )] = i.val( p );
        __chgVals(pos(i.variable(p)), i.vals(i.variable(p)));

    return *this;
  }

  /// returns the sequence of DiscreteVariable

  INLINE const Sequence<const DiscreteVariable *> &
  SetInst::variablesSequence() const {
    return __vars;
  }

  /// swap 2 vars in the SetInst

  INLINE void SetInst::__swap(Idx i, Idx j) {
    if (i == j)
      return;

    __vars.swap(i, j);

    Size v;

    v = __vals[i];

    __vals[i] = __vals[j];

    __vals[j] = v;
  }

  /// reordering

  INLINE void SetInst::reorder(const Sequence<const DiscreteVariable *> &original) {
    Idx max = original.size();
    Idx position = 0;

    for (Idx i = 0; i < max; ++i) {
      const DiscreteVariable *pv = original.atPos(i);

      if (contains(pv)) {
        GUM_ASSERT(pos(*pv) >= position); // this var should not be
        // already placed.
        __swap(position, pos(*pv));
        position++;
      }
    }
  }

  /// adds a new var to the sequence of vars

  INLINE void SetInst::__add(const DiscreteVariable &v) {
    __vars.insert(&v);
    __vals.push_back(1);
    __overflow = false;
  }

  /// removes a variable from the sequence of vars

  INLINE void SetInst::__erase(const DiscreteVariable &v) {
    // get the position of the variable
    Idx pos = __vars.pos(&v);
    __vars.erase(&v);
    __vals.erase(__vals.begin() + pos);
  }

  /// is this empty ?
  INLINE bool SetInst::empty() const { return __vals.empty(); }

  /// Replace x by y.
  INLINE void SetInst::_swap(const DiscreteVariable *x, const DiscreteVariable *y) {
    __vars.setAtPos(__vars.pos(x), y);
  }
} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
