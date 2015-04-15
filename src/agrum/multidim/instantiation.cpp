/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr   *
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
#include <sstream>
#include <agrum/multidim/multiDimAdressable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  void Instantiation::__init(MultiDimAdressable *master) {
    // for speed issues
    const Sequence<const DiscreteVariable *> &v = master->variablesSequence();
    __vars.resize(v.size());
    __vals.reserve(v.size());
    // fill the instantiation

    for (const auto var : v)
      __add(*var);

    if (master)
      actAsSlave(master->getMasterRef());
  }

  /// constructor for a Instantiation contained into a MultiDimInterface

  Instantiation::Instantiation(MultiDimAdressable &d)
      : __master(0), __overflow(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Instantiation);
    __init(&d);
  }

  Instantiation::Instantiation(const MultiDimAdressable &d)
      : __master(0), __overflow(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Instantiation);
    __init(const_cast<MultiDimAdressable *>(&d));
  }

  /// constructor for a Instantiation contained into a MultiDimInterface

  Instantiation::Instantiation(MultiDimAdressable *d)
      : __master(0), __overflow(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Instantiation);

    if (d)
      __init(d);
  }

  /// constructor for a Instantiation contained into a MultiDimInterface
  /** this constructor is needed in order to allow creation of Instantiation(this)
   * in MultiDimAdressable and below */

  Instantiation::Instantiation(const MultiDimAdressable *const_d)
      : __master(0), __overflow(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Instantiation);

    if (const_d)
      __init(const_cast<MultiDimAdressable *>(const_d));
  }

  /// copy constructor

  Instantiation::Instantiation(const Instantiation &aI, const bool notifyMaster)
      : MultiDimInterface(), __master(0), __overflow(false) {
    // for debugging purposes
    GUM_CONS_CPY(Instantiation);
    // copy the content of aI
    __vars = aI.__vars;
    __vals = aI.__vals;
    __overflow = aI.__overflow;

    if (aI.__master && notifyMaster)
      actAsSlave(*aI.__master);
  }

  // operator=
  Instantiation &Instantiation::operator=(const Instantiation &aI) {
    if (__master) {
      if (!aI.isMaster(__master)) { // aI as the same master.
        if (nbrDim() != aI.nbrDim()) {
          GUM_ERROR(OperationNotAllowed, "in slave Instantiation");
        }

        for (Idx i = 0; i < nbrDim(); i++) {
          if ((!contains(aI.variable(i))) || (!aI.contains(variable(i)))) {
            GUM_ERROR(OperationNotAllowed, "in slave Instantiation");
          }
        }
      }

      setVals(aI);
    } else {
      // copy the content of aI
      __vars = aI.__vars;
      __vals = aI.__vals;
      __overflow = aI.__overflow;

      if (aI.__master)
        actAsSlave(*aI.__master);
    }

    return *this;
  }

  /// Gives a string version of a Instantiation
  std::string Instantiation::toString() const {
    std::stringstream sstr;
    // check if the value of the instantiation is correct

    if (__overflow) {
      sstr << "<invalid>";
    }

    sstr << "<";

    bool first = true;

    for (const auto var : __vars) {
      if (!first)
        sstr << "|";

      first = false;
      sstr << var->name() << ":" << var->label(val(*var));
    }

    sstr << ">";

    return sstr.str();
  }

  /// give a Id value for Hamming distance
  Idx Instantiation::hamming() const {
    Idx res = 0;

    for (const auto var : __vars)
      res += val(*var);

    return res;
  }

  void Instantiation::setValsFrom(
      const HashTable<const DiscreteVariable *, const DiscreteVariable *> &map,
      const Instantiation &external) {
    for (const auto &elt : map) {
      const DiscreteVariable &var = *elt.second;

      try {
        Idx val = external.val(*elt.first);

        try {
          chgVal(var, val);
        } catch (NotFound &) {
          GUM_ERROR(NotFound, var.name() << " : missing variable in instantiation");
        }
      } catch (NotFound &) {
        GUM_ERROR(NotFound, var.name()
                                << " : missing variable in external instantiation");
      }
    }
  }

  /// an operator for user-friendly displaying the content of a Instantiation

  std::ostream &operator<<(std::ostream &aStream, const Instantiation &i) {
    aStream << i.toString();
    return aStream;
  }

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
