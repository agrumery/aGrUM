
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

#include <agrum/multidim/implementations/multiDimAdressable.h>

namespace gum {

  // indicates whether a given variable belongs to the Instantiation
  INLINE bool Instantiation::contains(const DiscreteVariable& v) const {
    return __vars.exists(&v);
  }

  // indicates whether a given variable belongs to the Instantiation
  INLINE bool Instantiation::contains(const DiscreteVariable* v) const {
    return __vars.exists(v);
  }

  // modifies internally the value of a given variable of the sequence
  INLINE void Instantiation::__chgVal(Idx varPos, Idx newVal) {
    Idx oldVal = __vals[varPos];
    __vals[varPos] = newVal;

    __masterChangeNotification(varPos, newVal, oldVal);
  }

  // modifies the value of a given variable of the sequence (external function)
  INLINE Instantiation& Instantiation::chgVal(const DiscreteVariable& v,
                                              Idx                     newVal) {
    try {
      // check that the variable does belong to the instantiation and that the
      // new
      // value is possible.
      Idx varPos = __vars.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize()) { GUM_ERROR(OutOfBounds, ""); }

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      __chgVal(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "instantiation does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name());
    }
  }

  INLINE Instantiation& Instantiation::chgVal(const DiscreteVariable* v,
                                              Idx                     newVal) {
    try {
      // check that the variable does belong to the instantiation and that the
      // new
      // value is possible.
      Idx varPos = __vars.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize()) { GUM_ERROR(OutOfBounds, ""); }

      // if we were in overflow, indicate that we are not anymore
      __overflow = false;

      __chgVal(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "instantiation does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name());
    }
  }

  // modifies the value of a given variable of the sequence (external function)
  INLINE Instantiation& Instantiation::chgVal(Idx varPos, Idx newVal) {
    // check that the variable does belong to the instantiation and that the new
    // value is possible.
    if (__vals.size() <= varPos) { GUM_ERROR(NotFound, ""); }

    if (newVal >= __vars[varPos]->domainSize()) { GUM_ERROR(OutOfBounds, ""); }

    // if we were in overflow, indicate that we are not anymore
    __overflow = false;

    __chgVal(varPos, newVal);

    return *this;
  }

  INLINE Instantiation& Instantiation::chgVal(const std::string& var, Idx newVal) {
    return chgVal(variable(var), newVal);
  }

  INLINE Instantiation& Instantiation::chgVal(const std::string& var,
                                              const std::string& newVal) {
    const auto& vv = variable(var);
    Idx         pos = vv.index(newVal);
    return chgVal(vv, pos);
  }

  // adds a new var to the sequence of vars
  INLINE void Instantiation::add(const DiscreteVariable& v) {
    // if __master : not allowed
    if (__master) { GUM_ERROR(OperationNotAllowed, "in slave Instantiation"); }

    // check if the variable already belongs to the tuple of variables
    // of the Instantiation
    if (__vars.exists(&v)) {
      GUM_ERROR(DuplicateElement,
                "Var <" << v.name() << "> already exists in this instantiation");
    }

    for (const auto& vv : __vars) {
      if (vv->name() == v.name()) {
        GUM_ERROR(InvalidArgument,
                  "Var with name <" << v.name()
                                    << "> already exists in this instantiation");
      }
    }

    // actually add the new dimension
    __add(v);
  }

  // removes a variable from the sequence of vars
  INLINE void Instantiation::erase(const DiscreteVariable& v) {
    // if __master : not allowed
    if (__master) { GUM_ERROR(OperationNotAllowed, "in slave Instantiation"); }

    // check that the variable does actually belong to the Instantiation
    if (!__vars.exists(&v)) {
      GUM_ERROR(NotFound, "Var does not exist in this instantiation");
    }

    // actually delete the dimension
    __erase(v);
  }

  // removes everything
  INLINE void Instantiation::clear() {
    if (__master) { GUM_ERROR(OperationNotAllowed, "in slave Instantiation"); }

    __vars.clear();
    __vals.clear();
  }

  // @brief returns the product of the size of the domains of the variables
  // belonging to the matrix
  INLINE Size Instantiation::domainSize() const {
    // @todo enhance the cplxity with a member domainSize ?
    Size s = 1;

    for (const auto var : __vars)
      s *= var->domainSize();

    return s;
  }

  // returns the index of a var
  INLINE Idx Instantiation::pos(const DiscreteVariable& k) const {
    return __vars.pos(&k);
  }

  // returns the number of vars in the sequence
  INLINE Idx Instantiation::nbrDim() const { return __vars.size(); }

  // returns the current value of a given variable
  INLINE Idx Instantiation::val(Idx i) const {
    if (i >= __vals.size()) {
      GUM_ERROR(NotFound, i << " is out of bound index for the instantiation.");
    }

    return __vals[i];
  }

  // returns the current value of a given variable
  INLINE Idx Instantiation::val(const DiscreteVariable& var) const {
    return __vals[__vars.pos(&var)];
  }

  // returns the current value of a given variable
  INLINE Idx Instantiation::valFromPtr(const DiscreteVariable* pvar) const {
    return __vals[__vars.pos(pvar)];
  }

  // returns the variable at position i in the tuple
  INLINE const DiscreteVariable& Instantiation::variable(Idx i) const {
    return *(__vars.atPos(i));
  }
  // returns the variable with name in the tuple
  INLINE const DiscreteVariable&
               Instantiation::variable(const std::string& name) const {
    for (const auto& v : __vars) {
      if (v->name() == name) return *v;
    }

    GUM_ERROR(NotFound, "'" << name << "' can not be found in the instantiation.")
  }

  // indicates whether the current value of the tuple is correct or not
  INLINE bool Instantiation::inOverflow() const { return __overflow; }

  // end() just is a synonym for inOverflow()
  INLINE bool Instantiation::end() const { return inOverflow(); }

  // rend() just is a synonym for inOverflow()
  INLINE bool Instantiation::rend() const { return inOverflow(); }

  // indicates that the current value is correct even if it should be in
  // overflow
  INLINE void Instantiation::unsetOverflow() { __overflow = false; }

  // alias for unsetOverflow
  INLINE void Instantiation::unsetEnd() { __overflow = false; }

  // operator ++
  INLINE void Instantiation::inc() {
    Size p = nbrDim();
    if (p == 0) { __overflow = true; }

    if (__overflow) return;
    p -= 1;
    Idx cpt = 0;
    // if we are in overflow, do nothing

    // perform the increment
    while (true) {
      Idx v = __vals[cpt];

      if (v + 1 == __vars[cpt]->domainSize()) {
        __vals[cpt] = 0;

        if (cpt == p) {
          __overflow = true;
          __masterFirstNotification();
          return;
        } else
          ++cpt;
      } else {
        ++__vals[cpt];
        break;
      }
    }

    __masterIncNotification();
  }

  // operator --
  INLINE void Instantiation::dec() {
    Size p = nbrDim();
    if (p == 0) { __overflow = true; }

    if (__overflow) return;
    p -= 1;
    Idx cpt = 0;
    // if we are in overflow, do nothing

    // perform the increment
    while (true) {
      Idx v = __vals[cpt];

      if (v == 0) {
        __vals[cpt] = __vars[cpt]->domainSize() - 1;

        if (cpt == p) {
          __overflow = true;

          __masterLastNotification();

          return;
        } else
          ++cpt;
      } else {
        --__vals[cpt];
        break;
      }
    }

    __masterDecNotification();
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
    //@todo : this code should be improved !!!
    for (Idx i = 0; i < depl; i++)
      inc();

    return *this;
  }

  // operator -=
  INLINE Instantiation& Instantiation::operator-=(Size depl) {
    //@todo : this code should be improved !!!
    for (Idx i = 0; i < depl; i++)
      dec();

    return *this;
  }

  // assign the (0,0,...) first value to the tuple of the Instantiation.
  INLINE void Instantiation::setFirst() {
    __overflow = false;
    Size s = nbrDim();

    for (Idx p = 0; p < s; ++p)
      __vals[p] = 0;

    __masterFirstNotification();
  }

  // put the (D1-1,D2-1,...) last value in the Instantiation
  INLINE void Instantiation::setLast() {
    __overflow = false;
    Size s = nbrDim();

    for (Idx p = 0; p < s; ++p)
      __vals[p] = __vars[p]->domainSize() - 1;

    __masterLastNotification();
  }

  // operator ++ limited only to the variables in i
  INLINE void Instantiation::incIn(const Instantiation& i) {
    // if i is empty, overflow and do nothing
    if (i.nbrDim() == 0) {
      __overflow = true;
      return;
    }

    // if we are in overflow, do nothing
    if (__overflow) return;

    Size p = i.nbrDim() - 1;

    Idx i_cpt = 0;

    while (true) {
      // verify that __vars[cpt] belongs to i before incrementing its value
      const DiscreteVariable& v = i.variable(i_cpt);

      if (!contains(v)) {
        if (i_cpt == p) {
          __overflow = true;
          return;
        } else
          ++i_cpt;
      } else {
        Idx cpt = pos(v);
        Idx iv = __vals[cpt];

        if (iv + 1 == __vars[cpt]->domainSize()) {
          __chgVal(cpt, 0);

          if (i_cpt == p) {
            __overflow = true;
            return;
          } else
            ++i_cpt;
        } else {
          __chgVal(cpt, iv + 1);
          return;
        }
      }
    }
  }

  // operator -- limited only to the variables in i
  INLINE void Instantiation::decIn(const Instantiation& i) {
    Size p = i.nbrDim() - 1;
    Idx  i_cpt = 0;
    // if we are in overflow, do nothing

    if (__overflow) return;

    while (true) {
      // verify that __vars[cpt] belongs to i before incrementing its value
      const DiscreteVariable& v = i.variable(i_cpt);

      if (!contains(v)) {
        if (i_cpt == p) {
          __overflow = true;
          return;
        } else
          ++i_cpt;
      } else {
        Idx cpt = pos(v);
        Idx iv = __vals[cpt];

        if (iv == 0) {
          __chgVal(cpt, __vars[cpt]->domainSize() - 1);

          if (i_cpt == p) {
            __overflow = true;
            return;
          } else
            ++i_cpt;
        } else {
          __chgVal(cpt, iv - 1);
          return;
        }
      }
    }
  }

  // reorder vars in *this
  INLINE void Instantiation::reorder(const Instantiation& i) {
    reorder(i.variablesSequence());
  }

  // put the (0,0,...) first value in the Instantiation for the variables in i
  INLINE void Instantiation::setFirstIn(const Instantiation& i) {
    __overflow = false;
    Idx s = nbrDim();

    for (Size p = 0; p < s; ++p)
      if (i.contains(__vars[p])) __chgVal(p, 0);
  }

  // change values with those in i
  INLINE Instantiation& Instantiation::setVals(const Instantiation& i) {
    __overflow = false;
    Idx s = i.nbrDim();

    for (Size p = 0; p < s; ++p)
      if (contains(i.variable(p))) __chgVal(pos(i.variable(p)), i.val(p));

    return *this;
  }

  // put the (D1-1,D2-1,...) lastvalue in the Instantiation for variables in i
  INLINE void Instantiation::setLastIn(const Instantiation& i) {
    __overflow = false;
    Idx s = nbrDim();

    for (Size p = 0; p < s; ++p)
      if (i.contains(__vars[p])) __chgVal(p, __vars[p]->domainSize() - 1);
  }

  // operator ++ for the variables not in i
  INLINE void Instantiation::incOut(const Instantiation& i) {
    Size p = nbrDim() - 1;
    Idx  cpt = 0;
    // if we are in overflow, do nothing

    if (__overflow) return;

    while (true) {
      if (i.contains(__vars[cpt])) {
        if (cpt == p) {
          __overflow = true;
          return;
        } else
          ++cpt;
      } else {
        Idx v = __vals[cpt];

        if (v + 1 == __vars[cpt]->domainSize()) {
          __chgVal(cpt, 0);

          if (cpt == p) {
            __overflow = true;
            return;
          } else
            ++cpt;
        } else {
          __chgVal(cpt, v + 1);
          return;
        }
      }
    }
  }

  // operator -- for the variables not in i
  INLINE void Instantiation::decOut(const Instantiation& i) {
    Size p = nbrDim() - 1;
    Idx  cpt = 0;
    // if we are in overflow, do nothing

    if (__overflow) return;

    while (true) {
      if (i.contains(__vars[cpt])) {
        if (cpt == p) {
          __overflow = true;
          return;
        } else
          ++cpt;
      } else {
        Idx v = __vals[cpt];

        if (v == 0) {
          __chgVal(cpt, __vars[cpt]->domainSize() - 1);

          if (cpt == p) {
            __overflow = true;
            return;
          } else
            ++cpt;
        } else {
          __chgVal(cpt, v - 1);
          return;
        }
      }
    }
  }

  // put the (0,0,...) first val in the Instantiation for the variables not in
  // i
  INLINE void Instantiation::setFirstOut(const Instantiation& i) {
    __overflow = false;
    Idx s = nbrDim();

    for (Size p = 0; p < s; ++p)
      if (!i.contains(__vars[p])) __chgVal(p, 0);
  }

  // put the (D1-1,D2-1,...) lastvalue in the Instantiation for vars not in i
  INLINE void Instantiation::setLastOut(const Instantiation& i) {
    __overflow = false;
    Idx s = nbrDim();

    for (Size p = 0; p < s; ++p)
      if (!i.contains(__vars[p])) __chgVal(p, __vars[p]->domainSize() - 1);
  }

  // operator ++ for vars which are not v.
  INLINE void Instantiation::incNotVar(const DiscreteVariable& v) {
    Size p = nbrDim() - 1;
    Idx  cpt = 0;
    // if we are in overflow, do nothing

    if (__overflow) return;

    while (true) {
      if (__vars[cpt] == &v) {
        if (cpt == p) {
          __overflow = true;
          return;
        } else
          ++cpt;
      } else {
        Idx iv = __vals[cpt];

        if (iv + 1 == __vars[cpt]->domainSize()) {
          __chgVal(cpt, 0);

          if (cpt == p) {
            __overflow = true;
            return;
          } else
            ++cpt;
        } else {
          __chgVal(cpt, iv + 1);
          return;
        }
      }
    }
  }

  // operator -- for vars which are not v.
  INLINE void Instantiation::decNotVar(const DiscreteVariable& v) {
    Size p = nbrDim() - 1;
    Idx  cpt = 0;
    // if we are in overflow, do nothing

    if (__overflow) return;

    while (true) {
      if (__vars[cpt] == &v) {
        if (cpt == p) {
          __overflow = true;
          return;
        } else
          ++cpt;
      } else {
        Idx iv = __vals[cpt];

        if (iv == 0) {
          __chgVal(cpt, __vars[cpt]->domainSize() - 1);

          if (cpt == p) {
            __overflow = true;
            return;
          } else
            ++cpt;
        } else {
          __chgVal(cpt, iv - 1);
          return;
        }
      }
    }
  }

  // assign the (0,0,...) first value to variables which are not v.
  INLINE void Instantiation::setFirstNotVar(const DiscreteVariable& v) {
    __overflow = false;
    Idx s = nbrDim();

    for (Size p = 0; p < s; ++p) {
      if (__vars[p] == &v) {
        Idx oldval = __vals[p];
        setFirst();
        __chgVal(p, oldval);
        return;
      }
    }

    setFirst();
  }

  // put the (D1-1,D2-1,...) lastvalue in the Instantiation for vars != v
  INLINE void Instantiation::setLastNotVar(const DiscreteVariable& v) {
    __overflow = false;
    Idx s = nbrDim();

    for (Size p = 0; p < s; ++p) {
      if (__vars[p] == &v) {
        Idx oldval = __vals[p];
        setLast();
        __chgVal(p, oldval);
        return;
      }
    }

    setLast();
  }

  // operator ++ for variable v only
  INLINE void Instantiation::incVar(const DiscreteVariable& v) {
    // get the position of the variable
    Idx cpt = __vars.pos(&v);
    // if we are in overflow, do nothing

    if (__overflow) return;

    Idx p = __vals[cpt];

    if (p + 1 == v.domainSize()) {
      __chgVal(cpt, 0);
      __overflow = true;
    } else {
      __chgVal(cpt, p + 1);
    }
  }

  // operator -- for variable v only
  INLINE void Instantiation::decVar(const DiscreteVariable& v) {
    // get the position of the variable
    Idx cpt = __vars.pos(&v);
    // if we are in overflow, do nothing

    if (__overflow) return;

    Idx p = __vals[cpt];

    if (p == 0) {
      __chgVal(cpt, v.domainSize() - 1);
      __overflow = true;
    } else {
      __chgVal(cpt, p - 1);
    }
  }

  // assign the first value in the Instantiation for var v.
  INLINE void Instantiation::setFirstVar(const DiscreteVariable& v) {
    __overflow = false;
    __chgVal(__vars.pos(&v), 0);
  }

  // assign the last value to var v.
  INLINE void Instantiation::setLastVar(const DiscreteVariable& v) {
    __overflow = false;
    __chgVal(__vars.pos(&v), v.domainSize() - 1);
  }

  // indicates whether the Instantiation has a master MultiDimAdressable
  INLINE bool Instantiation::isSlave() const { return (__master != 0); }

  // indicates wether the MultiDimAdressable* m is the master
  INLINE bool Instantiation::isMaster(const MultiDimAdressable* m) const {
    return (__master == m);
  }

  // indicates wether the MultiDimAdressable* m is the master
  INLINE bool Instantiation::isMaster(const MultiDimAdressable& m) const {
    return isMaster(&m);
  }

  // returns the sequence of DiscreteVariable
  INLINE const Sequence< const DiscreteVariable* >&
               Instantiation::variablesSequence() const {
    return __vars;
  }


  // replace 2 vars in the Instantiation
  INLINE void Instantiation::__swap(Idx i, Idx j) {
    if (i == j) return;

    __vars.swap(i, j);

    Idx v;
    v = __vals[i];
    __vals[i] = __vals[j];
    __vals[j] = v;
  }

  // reordering
  INLINE
  void
     Instantiation::reorder(const Sequence< const DiscreteVariable* >& original) {
    if (__master != nullptr) {
      GUM_ERROR(OperationNotAllowed,
                "Reordering impossible in slave instantiation");
    }

    __reorder(original);
  }

  INLINE
  void Instantiation::__reorder(
     const Sequence< const DiscreteVariable* >& original) {
    Idx max = original.size();
    Idx position = 0;
    for (Idx i = 0; i < max; ++i) {
      const DiscreteVariable* pv = original.atPos(i);

      if (contains(pv)) {
        auto p = pos(*pv);
        GUM_ASSERT(p >= position);   // this var should not be
                                     // already placed.
        __swap(position, p);
        position++;
      }
    }
  }


  // add new dim by master
  INLINE void Instantiation::addWithMaster(const MultiDimAdressable* m,
                                           const DiscreteVariable&   v) {
    if (m != __master) {
      GUM_ERROR(OperationNotAllowed, "only master can do this");
    }

    __add(v);
  }


  // adds a new var to the sequence of vars
  INLINE void Instantiation::__add(const DiscreteVariable& v) {
    __vars.insert(&v);
    __vals.push_back(0);
    __overflow = false;
  }

  // removes a variable from the sequence of vars
  INLINE void Instantiation::__erase(const DiscreteVariable& v) {
    // get the position of the variable
    Idx pos = __vars.pos(&v);
    __vars.erase(&v);
    __vals.erase(__vals.begin() + pos);
  }

  // is this empty ?
  INLINE bool Instantiation::empty() const { return __vals.empty(); }

  // Replace x by y.
  INLINE void Instantiation::_replace(const DiscreteVariable* x,
                                      const DiscreteVariable* y) {
    __vars.setAtPos(__vars.pos(x), y);
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by vectors of Idx
  INLINE Size HashFunc< Instantiation >::castToSize(const Instantiation& key) {
    Size h = Size(0);
    for (const DiscreteVariable* k :
         key.variablesSequence())   // k are unique only by address (not by name)
      h += HashFunc< const DiscreteVariable* >::castToSize(k) * Size(key.val(*k));

    return h;
  }

  /// returns a hashed key for hash tables the keys of which are represented
  /// by vectors of Idx
  INLINE Size HashFunc< Instantiation >::
              operator()(const Instantiation& key) const {
    return castToSize(key) & this->_hash_mask;
  }

  INLINE bool Instantiation::operator==(const Instantiation& other) const {
    if (inOverflow() && other.inOverflow()) return true;
    if (other.nbrDim() != nbrDim()) return false;
    for (const auto& k : variablesSequence()) {
      if (!other.contains(k)) return false;
      if (val(*k) != other.val(*k)) return false;
    }
    return true;
  }
} /* namespace gum */
