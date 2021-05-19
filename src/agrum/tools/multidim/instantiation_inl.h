/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 */

#include <agrum/tools/multidim/implementations/multiDimAdressable.h>

namespace gum {

  // indicates whether a given variable belongs to the Instantiation
  INLINE bool Instantiation::contains(const DiscreteVariable& v) const {
    return vars__.exists(&v);
  }

  INLINE bool Instantiation::contains(const std::string& name) const {
    return contains(variable(name));
  }

  // indicates whether a given variable belongs to the Instantiation
  INLINE bool Instantiation::contains(const DiscreteVariable* v) const {
    return vars__.exists(v);
  }

  // modifies internally the value of a given variable of the sequence
  INLINE void Instantiation::chgVal__(Idx varPos, Idx newVal) {
    Idx oldVal     = vals__[varPos];
    vals__[varPos] = newVal;

    masterChangeNotification__(varPos, newVal, oldVal);
  }

  // modifies the value of a given variable of the sequence (external function)
  INLINE Instantiation& Instantiation::chgVal(const DiscreteVariable& v,
                                              Idx                     newVal) {
    try {
      // check that the variable does belong to the instantiation and that the
      // new
      // value is possible.
      Idx varPos = vars__.pos(&v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v.domainSize()) { GUM_ERROR(OutOfBounds, "") }

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      chgVal__(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "instantiation does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v.name())
    }
  }

  INLINE Instantiation& Instantiation::chgVal(const DiscreteVariable* v,
                                              Idx                     newVal) {
    try {
      // check that the variable does belong to the instantiation and that the
      // new
      // value is possible.
      Idx varPos = vars__.pos(v);   // throws NotFound if v doesn't belong to this

      if (newVal >= v->domainSize()) { GUM_ERROR(OutOfBounds, "") }

      // if we were in overflow, indicate that we are not anymore
      overflow__ = false;

      chgVal__(varPos, newVal);

      return *this;
    } catch (NotFound&) {
      std::string name = "instantiation does not contain this DiscreteVariable: ";
      GUM_ERROR(NotFound, name + v->name())
    }
  }

  // modifies the value of a given variable of the sequence (external function)
  INLINE Instantiation& Instantiation::chgVal(Idx varPos, Idx newVal) {
    // check that the variable does belong to the instantiation and that the new
    // value is possible.
    if (vals__.size() <= varPos) { GUM_ERROR(NotFound, "") }

    if (newVal >= vars__[varPos]->domainSize()) { GUM_ERROR(OutOfBounds, "") }

    // if we were in overflow, indicate that we are not anymore
    overflow__ = false;

    chgVal__(varPos, newVal);

    return *this;
  }

  INLINE Instantiation& Instantiation::chgVal(const std::string& var, Idx newVal) {
    return chgVal(variable(var), newVal);
  }

  INLINE Instantiation& Instantiation::chgVal(const std::string& var,
                                              const std::string& newVal) {
    const auto& vv  = variable(var);
    Idx         pos = vv.index(newVal);
    return chgVal(vv, pos);
  }

  // adds a new var to the sequence of vars
  INLINE void Instantiation::add(const DiscreteVariable& v) {
    // if master__ : not allowed
    if (master__) { GUM_ERROR(OperationNotAllowed, "in slave Instantiation") }

    // check if the variable already belongs to the tuple of variables
    // of the Instantiation
    if (vars__.exists(&v)) {
      GUM_ERROR(DuplicateElement,
                "Var <" << v.name() << "> already exists in this instantiation")
    }

    for (const auto& vv: vars__) {
      if (vv->name() == v.name()) {
        GUM_ERROR(InvalidArgument,
                  "Var with name <" << v.name()
                                    << "> already exists in this instantiation");
      }
    }

    // actually add the new dimension
    add__(v);
  }

  // removes a variable from the sequence of vars
  INLINE void Instantiation::erase(const DiscreteVariable& v) {
    // if master__ : not allowed
    if (master__) { GUM_ERROR(OperationNotAllowed, "in slave Instantiation") }

    // check that the variable does actually belong to the Instantiation
    if (!vars__.exists(&v)) {
      GUM_ERROR(NotFound, "Var does not exist in this instantiation")
    }

    // actually delete the dimension
    erase__(v);
  }

  INLINE void Instantiation::erase(const std::string& name) {
    erase(variable(name));
  }

  // removes everything
  INLINE void Instantiation::clear() {
    if (master__) { GUM_ERROR(OperationNotAllowed, "in slave Instantiation") }

    vars__.clear();
    vals__.clear();
  }

  // @brief returns the product of the size of the domains of the variables
  // belonging to the matrix
  INLINE Size Instantiation::domainSize() const {
    Size s = 1;

    for (const auto var: vars__)
      s *= var->domainSize();

    return s;
  }

  // returns the index of a var
  INLINE Idx Instantiation::pos(const DiscreteVariable& k) const {
    return vars__.pos(&k);
  }

  // returns the number of vars in the sequence
  INLINE Idx Instantiation::nbrDim() const { return vars__.size(); }

  // returns the current value of a given variable
  INLINE Idx Instantiation::val(Idx i) const {
    if (i >= vals__.size()) {
      GUM_ERROR(NotFound, i << " is out of bound index for the instantiation.")
    }

    return vals__[i];
  }

  // returns the current value of a given variable
  INLINE Idx Instantiation::val(const DiscreteVariable& var) const {
    return vals__[vars__.pos(&var)];
  }
  // returns the current value of a given variable
  INLINE Idx Instantiation::val(const std::string& name) const {
    return val(variable(name));
  }


  // returns the current value of a given variable
  INLINE Idx Instantiation::valFromPtr(const DiscreteVariable* pvar) const {
    return vals__[vars__.pos(pvar)];
  }

  // returns the variable at position i in the tuple
  INLINE const DiscreteVariable& Instantiation::variable(Idx i) const {
    return *(vars__.atPos(i));
  }
  // returns the variable with name in the tuple
  INLINE const DiscreteVariable&
               Instantiation::variable(const std::string& name) const {
    for (const auto& v: vars__) {
      if (v->name() == name) return *v;
    }

    GUM_ERROR(NotFound, "'" << name << "' can not be found in the instantiation.")
  }

  // indicates whether the current value of the tuple is correct or not
  INLINE bool Instantiation::inOverflow() const { return overflow__; }

  // end() just is a synonym for inOverflow()
  INLINE bool Instantiation::end() const { return inOverflow(); }

  // rend() just is a synonym for inOverflow()
  INLINE bool Instantiation::rend() const { return inOverflow(); }

  // indicates that the current value is correct even if it should be in
  // overflow
  INLINE void Instantiation::unsetOverflow() { overflow__ = false; }

  // alias for unsetOverflow
  INLINE void Instantiation::unsetEnd() { overflow__ = false; }

  // operator ++
  INLINE void Instantiation::inc() {
    Size p = nbrDim();
    if (p == 0) { overflow__ = true; }

    if (overflow__) return;
    p -= 1;
    Idx cpt = 0;
    // if we are in overflow, do nothing

    // perform the increment
    while (true) {
      Idx v = vals__[cpt];

      if (v + 1 == vars__[cpt]->domainSize()) {
        vals__[cpt] = 0;

        if (cpt == p) {
          overflow__ = true;
          masterFirstNotification__();
          return;
        } else
          ++cpt;
      } else {
        ++vals__[cpt];
        break;
      }
    }

    masterIncNotification__();
  }

  // operator --
  INLINE void Instantiation::dec() {
    Size p = nbrDim();
    if (p == 0) { overflow__ = true; }

    if (overflow__) return;
    p -= 1;
    Idx cpt = 0;
    // if we are in overflow, do nothing

    // perform the increment
    while (true) {
      Idx v = vals__[cpt];

      if (v == 0) {
        vals__[cpt] = vars__[cpt]->domainSize() - 1;

        if (cpt == p) {
          overflow__ = true;

          masterLastNotification__();

          return;
        } else
          ++cpt;
      } else {
        --vals__[cpt];
        break;
      }
    }

    masterDecNotification__();
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
    overflow__ = false;
    Size s     = nbrDim();

    for (Idx p = 0; p < s; ++p)
      vals__[p] = 0;

    masterFirstNotification__();
  }

  // put the (D1-1,D2-1,...) last value in the Instantiation
  INLINE void Instantiation::setLast() {
    overflow__ = false;
    Size s     = nbrDim();

    for (Idx p = 0; p < s; ++p)
      vals__[p] = vars__[p]->domainSize() - 1;

    masterLastNotification__();
  }

  // operator ++ limited only to the variables in i
  INLINE void Instantiation::incIn(const Instantiation& i) {
    // if i is empty, overflow and do nothing
    if (i.nbrDim() == 0) {
      overflow__ = true;
      return;
    }

    // if we are in overflow, do nothing
    if (overflow__) return;

    Size p = i.nbrDim() - 1;

    Idx i_cpt = 0;

    while (true) {
      // verify that vars__[cpt] belongs to i before incrementing its value
      const DiscreteVariable& v = i.variable(i_cpt);

      if (!contains(v)) {
        if (i_cpt == p) {
          overflow__ = true;
          return;
        } else
          ++i_cpt;
      } else {
        Idx cpt = pos(v);
        Idx iv  = vals__[cpt];

        if (iv + 1 == vars__[cpt]->domainSize()) {
          chgVal__(cpt, 0);

          if (i_cpt == p) {
            overflow__ = true;
            return;
          } else
            ++i_cpt;
        } else {
          chgVal__(cpt, iv + 1);
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

    if (overflow__) return;

    while (true) {
      // verify that vars__[cpt] belongs to i before incrementing its value
      const DiscreteVariable& v = i.variable(i_cpt);

      if (!contains(v)) {
        if (i_cpt == p) {
          overflow__ = true;
          return;
        } else
          ++i_cpt;
      } else {
        Idx cpt = pos(v);
        Idx iv  = vals__[cpt];

        if (iv == 0) {
          chgVal__(cpt, vars__[cpt]->domainSize() - 1);

          if (i_cpt == p) {
            overflow__ = true;
            return;
          } else
            ++i_cpt;
        } else {
          chgVal__(cpt, iv - 1);
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
    overflow__ = false;
    Idx s      = nbrDim();

    for (Size p = 0; p < s; ++p)
      if (i.contains(vars__[p])) chgVal__(p, 0);
  }

  // change values with those in i
  INLINE Instantiation& Instantiation::setVals(const Instantiation& i) {
    overflow__ = false;
    Idx s      = i.nbrDim();

    for (Size p = 0; p < s; ++p)
      if (contains(i.variable(p))) chgVal__(pos(i.variable(p)), i.val(p));

    return *this;
  }

  // put the (D1-1,D2-1,...) lastvalue in the Instantiation for variables in i
  INLINE void Instantiation::setLastIn(const Instantiation& i) {
    overflow__ = false;
    Idx s      = nbrDim();

    for (Size p = 0; p < s; ++p)
      if (i.contains(vars__[p])) chgVal__(p, vars__[p]->domainSize() - 1);
  }

  // operator ++ for the variables not in i
  INLINE void Instantiation::incOut(const Instantiation& i) {
    Size p   = nbrDim() - 1;
    Idx  cpt = 0;
    // if we are in overflow, do nothing

    if (overflow__) return;

    while (true) {
      if (i.contains(vars__[cpt])) {
        if (cpt == p) {
          overflow__ = true;
          return;
        } else
          ++cpt;
      } else {
        Idx v = vals__[cpt];

        if (v + 1 == vars__[cpt]->domainSize()) {
          chgVal__(cpt, 0);

          if (cpt == p) {
            overflow__ = true;
            return;
          } else
            ++cpt;
        } else {
          chgVal__(cpt, v + 1);
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

    if (overflow__) return;

    while (true) {
      if (i.contains(vars__[cpt])) {
        if (cpt == p) {
          overflow__ = true;
          return;
        } else
          ++cpt;
      } else {
        Idx v = vals__[cpt];

        if (v == 0) {
          chgVal__(cpt, vars__[cpt]->domainSize() - 1);

          if (cpt == p) {
            overflow__ = true;
            return;
          } else
            ++cpt;
        } else {
          chgVal__(cpt, v - 1);
          return;
        }
      }
    }
  }

  // put the (0,0,...) first val in the Instantiation for the variables not in
  // i
  INLINE void Instantiation::setFirstOut(const Instantiation& i) {
    overflow__ = false;
    Idx s      = nbrDim();

    for (Size p = 0; p < s; ++p)
      if (!i.contains(vars__[p])) chgVal__(p, 0);
  }

  // put the (D1-1,D2-1,...) lastvalue in the Instantiation for vars not in i
  INLINE void Instantiation::setLastOut(const Instantiation& i) {
    overflow__ = false;
    Idx s      = nbrDim();

    for (Size p = 0; p < s; ++p)
      if (!i.contains(vars__[p])) chgVal__(p, vars__[p]->domainSize() - 1);
  }

  // operator ++ for vars which are not v.
  INLINE void Instantiation::incNotVar(const DiscreteVariable& v) {
    Size p   = nbrDim() - 1;
    Idx  cpt = 0;
    // if we are in overflow, do nothing

    if (overflow__) return;

    while (true) {
      if (vars__[cpt] == &v) {
        if (cpt == p) {
          overflow__ = true;
          return;
        } else
          ++cpt;
      } else {
        Idx iv = vals__[cpt];

        if (iv + 1 == vars__[cpt]->domainSize()) {
          chgVal__(cpt, 0);

          if (cpt == p) {
            overflow__ = true;
            return;
          } else
            ++cpt;
        } else {
          chgVal__(cpt, iv + 1);
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

    if (overflow__) return;

    while (true) {
      if (vars__[cpt] == &v) {
        if (cpt == p) {
          overflow__ = true;
          return;
        } else
          ++cpt;
      } else {
        Idx iv = vals__[cpt];

        if (iv == 0) {
          chgVal__(cpt, vars__[cpt]->domainSize() - 1);

          if (cpt == p) {
            overflow__ = true;
            return;
          } else
            ++cpt;
        } else {
          chgVal__(cpt, iv - 1);
          return;
        }
      }
    }
  }

  // assign the (0,0,...) first value to variables which are not v.
  INLINE void Instantiation::setFirstNotVar(const DiscreteVariable& v) {
    overflow__ = false;
    Idx s      = nbrDim();

    for (Size p = 0; p < s; ++p) {
      if (vars__[p] == &v) {
        Idx oldval = vals__[p];
        setFirst();
        chgVal__(p, oldval);
        return;
      }
    }

    setFirst();
  }

  // put the (D1-1,D2-1,...) lastvalue in the Instantiation for vars != v
  INLINE void Instantiation::setLastNotVar(const DiscreteVariable& v) {
    overflow__ = false;
    Idx s      = nbrDim();

    for (Size p = 0; p < s; ++p) {
      if (vars__[p] == &v) {
        Idx oldval = vals__[p];
        setLast();
        chgVal__(p, oldval);
        return;
      }
    }

    setLast();
  }

  // operator ++ for variable v only
  INLINE void Instantiation::incVar(const DiscreteVariable& v) {
    // get the position of the variable
    Idx cpt = vars__.pos(&v);
    // if we are in overflow, do nothing

    if (overflow__) return;

    Idx p = vals__[cpt];

    if (p + 1 == v.domainSize()) {
      chgVal__(cpt, 0);
      overflow__ = true;
    } else {
      chgVal__(cpt, p + 1);
    }
  }

  // operator -- for variable v only
  INLINE void Instantiation::decVar(const DiscreteVariable& v) {
    // get the position of the variable
    Idx cpt = vars__.pos(&v);
    // if we are in overflow, do nothing

    if (overflow__) return;

    Idx p = vals__[cpt];

    if (p == 0) {
      chgVal__(cpt, v.domainSize() - 1);
      overflow__ = true;
    } else {
      chgVal__(cpt, p - 1);
    }
  }

  // assign the first value in the Instantiation for var v.
  INLINE void Instantiation::setFirstVar(const DiscreteVariable& v) {
    overflow__ = false;
    chgVal__(vars__.pos(&v), 0);
  }

  // assign the last value to var v.
  INLINE void Instantiation::setLastVar(const DiscreteVariable& v) {
    overflow__ = false;
    chgVal__(vars__.pos(&v), v.domainSize() - 1);
  }

  // indicates whether the Instantiation has a master MultiDimAdressable
  INLINE bool Instantiation::isSlave() const { return (master__ != nullptr); }

  // indicates wether the MultiDimAdressable* m is the master
  INLINE bool Instantiation::isMaster(const MultiDimAdressable* m) const {
    return (master__ == m);
  }

  // indicates wether the MultiDimAdressable* m is the master
  INLINE bool Instantiation::isMaster(const MultiDimAdressable& m) const {
    return isMaster(&m);
  }

  // returns the sequence of DiscreteVariable
  INLINE const Sequence< const DiscreteVariable* >&
               Instantiation::variablesSequence() const {
    return vars__;
  }


  // replace 2 vars in the Instantiation
  INLINE void Instantiation::swap__(Idx i, Idx j) {
    if (i == j) return;

    vars__.swap(i, j);

    Idx v;
    v         = vals__[i];
    vals__[i] = vals__[j];
    vals__[j] = v;
  }

  // reordering
  INLINE
  void
     Instantiation::reorder(const Sequence< const DiscreteVariable* >& original) {
    if (master__ != nullptr) {
      GUM_ERROR(OperationNotAllowed,
                "Reordering impossible in slave instantiation")
    }

    reorder__(original);
  }

  INLINE
  void Instantiation::reorder__(
     const Sequence< const DiscreteVariable* >& original) {
    Idx max      = original.size();
    Idx position = 0;
    for (Idx i = 0; i < max; ++i) {
      const DiscreteVariable* pv = original.atPos(i);

      if (contains(pv)) {
        auto p = pos(*pv);
        GUM_ASSERT(p >= position);   // this var should not be
                                     // already placed.
        swap__(position, p);
        position++;
      }
    }
  }


  // add new dim by master
  INLINE void Instantiation::addWithMaster(const MultiDimAdressable* m,
                                           const DiscreteVariable&   v) {
    if (m != master__) {
      GUM_ERROR(OperationNotAllowed, "only master can do this")
    }

    add__(v);
  }


  // adds a new var to the sequence of vars
  INLINE void Instantiation::add__(const DiscreteVariable& v) {
    vars__.insert(&v);
    vals__.push_back(0);
    overflow__ = false;
  }

  // removes a variable from the sequence of vars
  INLINE void Instantiation::erase__(const DiscreteVariable& v) {
    // get the position of the variable
    Idx pos = vars__.pos(&v);
    vars__.erase(&v);
    vals__.erase(vals__.begin() + pos);
  }

  // is this empty ?
  INLINE bool Instantiation::empty() const { return vals__.empty(); }

  // Replace x by y.
  INLINE void Instantiation::replace_(const DiscreteVariable* x,
                                      const DiscreteVariable* y) {
    vars__.setAtPos(vars__.pos(x), y);
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
  INLINE Size
     HashFunc< Instantiation >::operator()(const Instantiation& key) const {
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
