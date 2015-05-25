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
#include <agrum/multidim/conditionalProbabilityFunction.h>

namespace gum {

  /**
   * Template implementation of CPF.
   */

  // Default constructor.
  // A MultiDimArray is used for representing the CPF.
  template <typename GUM_SCALAR>
  INLINE CPF<GUM_SCALAR>::CPF()
      : MultiDimDecorator<GUM_SCALAR>(new MultiDimArray<GUM_SCALAR>()) {
    GUM_CONSTRUCTOR(CPF);
    __condMap = new HashTable<const DiscreteVariable *, bool>();
  }

  // Constructor with user defined content.
  // The content will be deleted with this class.
  // @param content The implementation used for representing the CPF.
  template <typename GUM_SCALAR>
  INLINE CPF<GUM_SCALAR>::CPF(MultiDimImplementation<GUM_SCALAR> *content)
      : MultiDimDecorator<GUM_SCALAR>(content) {
    GUM_CONSTRUCTOR(CPF);
    __condMap = new HashTable<const DiscreteVariable *, bool>();
  }

  // Copy constructor.
  template <typename GUM_SCALAR>
  INLINE CPF<GUM_SCALAR>::CPF(const CPF<GUM_SCALAR> &from)
      : MultiDimDecorator<GUM_SCALAR>(from) {
    GUM_CONS_CPY(CPF);
    __condMap = new HashTable<const DiscreteVariable *, bool>(*(from.__condMap));
  }

  // Destructor.
  template <typename GUM_SCALAR> INLINE CPF<GUM_SCALAR>::~CPF() {
    GUM_DESTRUCTOR(CPF);
    delete __condMap;
  }

  // Getters & setters

  // Add a new conditioned variable.
  // @param v The new conditioned variable added.
  // @throw DuplicateElement
  template <typename GUM_SCALAR>
  INLINE void CPF<GUM_SCALAR>::add(const DiscreteVariable &var) {
    MultiDimDecorator<GUM_SCALAR>::add(var);
    __condMap->insert(&var, true);
  }

  //  Removes a dimension.
  //  @param var The variable which is removed.
  //  @throws NotFound If var isn't in this.
  //  @throws OperationNotAllowed If var can't be removed.
  template <typename GUM_SCALAR>
  INLINE void CPF<GUM_SCALAR>::erase(const DiscreteVariable &var) {
    MultiDimDecorator<GUM_SCALAR>::erase(var);
    __condMap->erase(&var);
  }

  // Adds a conditioning variable.
  // @param var The variable added as a conditioning variable.
  // @param isCond If true then var is added as a conditioned variable,
  //               otherwise it is added as a conditioning variable.
  template <typename GUM_SCALAR>
  INLINE void CPF<GUM_SCALAR>::add(const DiscreteVariable &var, bool isConditioned) {
    MultiDimDecorator<GUM_SCALAR>::add(var);
    __condMap->insert(&var, isConditioned);
  }

  // Returns true if the variable is a conditioned variable.
  template <typename GUM_SCALAR>
  INLINE bool CPF<GUM_SCALAR>::isConditioned(const DiscreteVariable &var) const {
    return (*__condMap)[&var];
  }

  // Returns a Instantiation initialized
  // over the conditioned variables.
  //
  // The methods parse all the sequence of variables to build the
  // Instantiation (i.e. it's not fast).
  template <typename GUM_SCALAR>
  INLINE Instantiation CPF<GUM_SCALAR>::conditionedVars() {
    Instantiation inst;

    for (HashTable<const DiscreteVariable *, bool>::iterator_safe iter =
             __condMap->beginSafe();
         iter != __condMap->endSafe(); ++iter) {
      if (iter.val())
        inst.add(*(iter.key()));
    }

    return inst;
  }

  // Returns a constant Instantiation initialized
  // over the conditioned variables.
  //
  // The methods parse all the sequence of variables to build the
  // Instantiation (i.e. it's not fast).
  template <typename GUM_SCALAR>
  INLINE const Instantiation CPF<GUM_SCALAR>::conditionedVars() const {
    Instantiation inst;

    for (HashTable<const DiscreteVariable *, bool>::iterator_safe iter =
             __condMap->beginSafe();
         iter != __condMap->endSafe(); ++iter) {
      if (iter.val())
        inst.add(*(iter.key()));
    }

    return inst;
  }

  // Returns a Instantiation initialized
  // over the conditioning variables.
  //
  // The methods parse all the sequence of variables to build the
  // Instantiation (i.e. it's not fast).
  template <typename GUM_SCALAR>
  INLINE Instantiation CPF<GUM_SCALAR>::conditioningVars() {
    Instantiation inst;

    for (HashTable<const DiscreteVariable *, bool>::iterator_safe iter =
             __condMap->beginSafe();
         iter != __condMap->endSafe(); ++iter) {
      if (!iter.val())
        inst.add(*(iter.key()));
    }

    return inst;
  }

  // Returns a constant Instantiation initialized
  // over the conditioning variables.
  //
  // The methods parse all the sequence of variables to build the
  // Instantiation (i.e. it's not fast).
  template <typename GUM_SCALAR>
  INLINE const Instantiation CPF<GUM_SCALAR>::conditioningVars() const {
    Instantiation inst;

    for (HashTableIteratorSafe<const DiscreteVariable *, bool> iter =
             __condMap->beginSafe();
         iter != __condMap->endSafe(); ++iter) {
      if (!iter.val())
        inst.add(*(iter.key()));
    }

    return inst;
  }

  // @name Various methods

  // Used by operator<< for displaying the content of CPF.
  //
  // The result differs from the one of a MultiDimDecorator since
  // conditioned variables will be places at the left and conditioning variables
  // at the right.
  //
  // @return Returns a string with the content of this CPF.
  template <typename GUM_SCALAR>
  INLINE const std::string CPF<GUM_SCALAR>::toString(const Instantiation *i) const {
    std::stringstream left;
    std::stringstream right;

    for (HashTable<const DiscreteVariable *, bool>::iterator_safe iter =
             __condMap->beginSafe();
         iter != __condMap->endSafe(); ++iter) {
      if (iter.val()) {
        left << i->val(*(iter.key())) << " ";
      } else {
        right << " " << i->val(*(iter.key()));
      }
    }

    std::stringstream sum;
    sum << "[" << left.str() << " |" << right.str() << "]";
    return sum.str();
  }

  template <typename GUM_SCALAR>
  INLINE CPF<GUM_SCALAR> *CPF<GUM_SCALAR>::newFactory() const {
    return new CPF<GUM_SCALAR>(static_cast<MultiDimImplementation<GUM_SCALAR> *>(
        this->getContent()->newFactory()));
  }

  template <typename GUM_SCALAR>
  INLINE void CPF<GUM_SCALAR>::_swap(const DiscreteVariable *x,
                                     const DiscreteVariable *y) {
    __condMap->insert(y, (*__condMap)[x]);
    __condMap->erase(x);
    MultiDimDecorator<GUM_SCALAR>::getContent()._swap(x, y);
  }

} /* namespace gum */
