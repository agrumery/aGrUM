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


/**
 * @file
 * @brief Implementation of gum::MultiDimImplementation.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 */

// to ease IDE parser
#include <agrum/tools/multidim/implementations/multiDimImplementation.h>

namespace gum {

  // Default constructor

  template < typename GUM_SCALAR >
  INLINE MultiDimImplementation< GUM_SCALAR >::MultiDimImplementation() :
      MultiDimContainer< GUM_SCALAR >(), vars__(), slaveInstantiations__() {
    GUM_CONSTRUCTOR(MultiDimImplementation);
    internalChangeMethod__ = InternalChangeMethod__::DIRECT_CHANGE;
    internalChangeState__  = InternalChangeState__::NO_CHANGE;
    domainSize__           = 1;
  }

  // Copy constructor

  template < typename GUM_SCALAR >
  INLINE MultiDimImplementation< GUM_SCALAR >::MultiDimImplementation(
     const MultiDimImplementation< GUM_SCALAR >& from) :
      MultiDimContainer< GUM_SCALAR >(from),
      vars__(from.vars__), internalChangeMethod__(from.internalChangeMethod__),
      internalChangeState__(from.internalChangeState__),
      domainSize__(from.domainSize__) {
    GUM_CONS_CPY(MultiDimImplementation);
    GUM_ASSERT(!this->isCommitNeeded_());
  }

  // destructor

  template < typename GUM_SCALAR >
  INLINE MultiDimImplementation< GUM_SCALAR >::~MultiDimImplementation() {
    GUM_DESTRUCTOR(MultiDimImplementation);
    // unregister all remaining slave instantiations

    for (List< Instantiation* >::iterator_safe iter
         = slaveInstantiations__.beginSafe();
         iter != slaveInstantiations__.endSafe();
         ++iter)
      (*iter)->forgetMaster();
  }

  // add a new var to the sequence of vars__.

  template < typename GUM_SCALAR >
  INLINE void
     MultiDimImplementation< GUM_SCALAR >::add(const DiscreteVariable& v) {
    // check if the variable already belongs to the tuple of variables
    // of the Instantiation
    if (vars__.exists(&v)) {
      GUM_ERROR(DuplicateElement,
                "Var " << v.name() << " already exists in this instantiation");
    }
    for (const auto& w: vars__) {
      if (w->name() == v.name())
        GUM_ERROR(DuplicateElement,
                  "A var with name '" << v.name()
                                      << "' already exists in this instantiation");
    }

    domainSize__ *= v.domainSize();

    vars__.insert(&v);

    // informs all the slaves that they have to update themselves
    for (List< Instantiation* >::iterator_safe iter
         = slaveInstantiations__.beginSafe();
         iter != slaveInstantiations__.endSafe();
         ++iter) {
      (*iter)->addWithMaster(this, v);
    }

    if (isInMultipleChangeMethod_()) setNotCommitedChange__();
  }

  // removes a var from the variables of the multidimensional matrix

  template < typename GUM_SCALAR >
  INLINE void
     MultiDimImplementation< GUM_SCALAR >::erase(const DiscreteVariable& v) {
    // check that the variable does actually belong to the
    // MultiDimImplementation
    if (!vars__.exists(&v)) {
      GUM_ERROR(NotFound, "Var does not exist in this implementation");
    }

    domainSize__ /= v.domainSize();

    vars__.erase(&v);

    // informs all the slaves that they have to update themselves
    for (List< Instantiation* >::iterator_safe iter
         = slaveInstantiations__.beginSafe();
         iter != slaveInstantiations__.endSafe();
         ++iter) {
      (*iter)->eraseWithMaster(this, v);
    }

    if (isInMultipleChangeMethod_()) setNotCommitedChange__();
  }

  // adds a new var to the sequence of vars__
  template < typename GUM_SCALAR >
  INLINE MultiDimImplementation< GUM_SCALAR >&
         operator<<(MultiDimImplementation< GUM_SCALAR >& array,
                const DiscreteVariable&               v) {
    array.add(v);
    return array;
  }

  // add a Instantiation to the list of slave instantiations

  template < typename GUM_SCALAR >
  INLINE bool
     MultiDimImplementation< GUM_SCALAR >::registerSlave(Instantiation& slave) {
    // check that the Instantiation has the same variables as this
    if (slave.nbrDim() != vars__.size()) return false;

    for (Sequence< const DiscreteVariable* >::iterator_safe iter
         = vars__.beginSafe();
         iter != vars__.endSafe();
         ++iter)
      if (!slave.contains(*iter)) return false;

    slave.synchronizeWithMaster(this);

    slaveInstantiations__ += (&slave);

    return true;
  }

  // removes a Instantiation from the list of slave instantiations

  template < typename GUM_SCALAR >
  INLINE bool
     MultiDimImplementation< GUM_SCALAR >::unregisterSlave(Instantiation& slave) {
    slaveInstantiations__.eraseByVal(&slave);
    return true;
  }

  template < typename GUM_SCALAR >
  INLINE Idx MultiDimImplementation< GUM_SCALAR >::nbrDim() const {
    return vars__.size();
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimImplementation< GUM_SCALAR >::domainSize() const {
    return domainSize__;
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable&
               MultiDimImplementation< GUM_SCALAR >::variable(Idx i) const {
    return *(vars__.atPos(i));
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable& MultiDimImplementation< GUM_SCALAR >::variable(
     const std::string& name) const {
    for (const auto& v: vars__) {
      if (v->name() == name) return *v;
    }

    GUM_ERROR(NotFound,
              "'" << name << "' can not be found in the multidim structure.")
  }

  template < typename GUM_SCALAR >
  INLINE Idx
     MultiDimImplementation< GUM_SCALAR >::pos(const DiscreteVariable& v) const {
    return vars__.pos(&v);
  }

  template < typename GUM_SCALAR >
  INLINE bool MultiDimImplementation< GUM_SCALAR >::contains(
     const DiscreteVariable& v) const {
    return vars__.exists(&v);
  }

  // returns a const ref to the sequence of DiscreteVariable*

  template < typename GUM_SCALAR >
  INLINE const Sequence< const DiscreteVariable* >&
               MultiDimImplementation< GUM_SCALAR >::variablesSequence() const {
    return vars__;
  }

  // is this empty ?
  template < typename GUM_SCALAR >
  INLINE bool MultiDimImplementation< GUM_SCALAR >::empty() const {
    GUM_ASSERT(!this->isCommitNeeded_());
    return vars__.empty();
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::beginMultipleChanges() {
    internalChangeMethod__ = InternalChangeMethod__::MULTIPLE_CHANGE;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::endMultipleChanges() {
    if (internalChangeState__ == InternalChangeState__::NOT_COMMITTED_CHANGE) {
      commitMultipleChanges_();
      internalChangeState__ = InternalChangeState__::NO_CHANGE;
    }

    internalChangeMethod__ = InternalChangeMethod__::DIRECT_CHANGE;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::endMultipleChanges(
     const GUM_SCALAR& x) {
    if (internalChangeState__ == InternalChangeState__::NOT_COMMITTED_CHANGE) {
      commitMultipleChanges_(x);
      internalChangeState__ = InternalChangeState__::NO_CHANGE;
    }

    internalChangeMethod__ = InternalChangeMethod__::DIRECT_CHANGE;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::commitMultipleChanges_() {
    // empty!
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::commitMultipleChanges_(
     const GUM_SCALAR&) {
    // empty!
  }

  // get the actual change method of *this
  template < typename GUM_SCALAR >
  INLINE bool
     MultiDimImplementation< GUM_SCALAR >::isInMultipleChangeMethod_() const {
    return (internalChangeMethod__ == InternalChangeMethod__::MULTIPLE_CHANGE);
  }

  // get the actual state of *this
  template < typename GUM_SCALAR >
  INLINE bool MultiDimImplementation< GUM_SCALAR >::isCommitNeeded_() const {
    return (internalChangeState__ == InternalChangeState__::NOT_COMMITTED_CHANGE);
  }

  // Returns a constant reference over the list of slaved instantiations.
  template < typename GUM_SCALAR >
  INLINE const List< Instantiation* >&
               MultiDimImplementation< GUM_SCALAR >::slaves_() const {
    return slaveInstantiations__;
  }

  // get the actual state of *this
  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::setNotCommitedChange__() {
    internalChangeState__ = InternalChangeState__::NOT_COMMITTED_CHANGE;
  }

  // get the actual state of *this
  template < typename GUM_SCALAR >
  INLINE float MultiDimImplementation< GUM_SCALAR >::compressionRate() const {
    return ((float)1) - (float)realSize() / (float)domainSize();
  }

  // returns a basename to be used for default operators
  template < typename GUM_SCALAR >
  const std::string& MultiDimImplementation< GUM_SCALAR >::basename() const {
    static const std::string str = "MultiDimImplementation";
    return str;
  }

  template < typename GUM_SCALAR >
  INLINE void
     MultiDimImplementation< GUM_SCALAR >::replace_(const DiscreteVariable* x,
                                                    const DiscreteVariable* y) {
    vars__.setAtPos(vars__.pos(x), y);

    for (List< Instantiation* >::iterator_safe iter
         = slaveInstantiations__.beginSafe();
         iter != slaveInstantiations__.endSafe();
         ++iter) {
      (**iter).replace(*x, *y);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimImplementation< GUM_SCALAR >::invert_(Idx p1, Idx p2) {
    vars__.swap(p1, p2);
  }

  // for friendly displaying the content of the array
  template < typename GUM_SCALAR >
  INLINE std::ostream&
         operator<<(std::ostream&                               out,
                const MultiDimImplementation< GUM_SCALAR >& array) {
    return out << static_cast< const MultiDimContainer< GUM_SCALAR >& >(array);
  }


  // protected access to content_
  template < typename GUM_SCALAR >
  INLINE MultiDimImplementation< GUM_SCALAR >*
         MultiDimImplementation< GUM_SCALAR >::content() {
    return this;
  }

  // protected access to content_
  template < typename GUM_SCALAR >
  INLINE const MultiDimImplementation< GUM_SCALAR >*
               MultiDimImplementation< GUM_SCALAR >::content() const {
    return this;
  }
} /* namespace gum */
