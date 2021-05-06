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
 * @brief Implementation of gum::Instantiation.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 */

#include <agrum/tools/multidim/implementations/multiDimAdressable.h>
#include <agrum/tools/multidim/instantiation.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/multidim/instantiation_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  // Default constructor
  Instantiation::Instantiation() : master__(nullptr), overflow__(false) {
    GUM_CONSTRUCTOR(Instantiation);
  }

  // destructor
  Instantiation::~Instantiation() {
    GUM_DESTRUCTOR(Instantiation);
    // unregister the Instantiation from its master__

    if (master__) master__->unregisterSlave(*this);
  }

  void Instantiation::init__(MultiDimAdressable* master) {
    // for speed issues
    GUM_ASSERT(master != nullptr);

    const Sequence< const DiscreteVariable* >& v = master->variablesSequence();
    vars__.resize(v.size());
    vals__.reserve(v.size());
    // fill the instantiation

    for (const auto var: v)
      add__(*var);

    actAsSlave(master->getMasterRef());
  }

  // constructor for a Instantiation contained into a MultiDimInterface
  Instantiation::Instantiation(MultiDimAdressable& d) :
      master__(0), overflow__(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Instantiation);
    init__(&d);
  }

  Instantiation::Instantiation(const MultiDimAdressable& d) :
      master__(0), overflow__(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Instantiation);
    init__(const_cast< MultiDimAdressable* >(&d));
  }

  // constructor for a Instantiation contained into a MultiDimInterface
  Instantiation::Instantiation(MultiDimAdressable* d) :
      master__(0), overflow__(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Instantiation);

    if (d) init__(d);
  }

  // constructor for a Instantiation contained into a MultiDimInterface this
  // constructor is needed in order to allow creation of Instantiation(this) in
  // MultiDimAdressable and below
  Instantiation::Instantiation(const MultiDimAdressable* const_d) :
      master__(0), overflow__(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Instantiation);

    if (const_d) init__(const_cast< MultiDimAdressable* >(const_d));
  }

  // copy constructor
  Instantiation::Instantiation(const Instantiation& aI, const bool notifyMaster) :
      MultiDimInterface(), master__(0), overflow__(false) {
    // for debugging purposes
    GUM_CONS_CPY(Instantiation);
    // copy the content of aI
    vars__     = aI.vars__;
    vals__     = aI.vals__;
    overflow__ = aI.overflow__;

    if (aI.master__ && notifyMaster) actAsSlave(*aI.master__);
  }

  // operator=
  Instantiation& Instantiation::operator=(const Instantiation& aI) {
    if (master__) {
      if (!aI.isMaster(master__)) {   // aI as the same master.
        if (nbrDim() != aI.nbrDim()) {
          GUM_ERROR(OperationNotAllowed, "in slave Instantiation")
        }

        for (Idx i = 0; i < nbrDim(); i++) {
          if ((!contains(aI.variable(i))) || (!aI.contains(variable(i)))) {
            GUM_ERROR(OperationNotAllowed, "in slave Instantiation")
          }
        }
      }

      setVals(aI);
    } else {
      // copy the content of aI
      vars__     = aI.vars__;
      vals__     = aI.vals__;
      overflow__ = aI.overflow__;

      if (aI.master__) actAsSlave(*aI.master__);
    }

    return *this;
  }

  // Gives a string version of a Instantiation
  std::string Instantiation::toString() const {
    std::stringstream sstr;
    // check if the value of the instantiation is correct

    if (overflow__) { sstr << "<invalid>"; }

    sstr << "<";

    bool first = true;

    for (const auto var: vars__) {
      if (!first) sstr << "|";

      first = false;
      sstr << var->name() << ":" << var->label(val(*var));
    }

    sstr << ">";

    return sstr.str();
  }

  // give a Id value for Hamming distance
  Idx Instantiation::hamming() const {
    Idx res = 0;

    for (const auto var: vars__)
      res += val(*var);

    return res;
  }

  void Instantiation::setValsFrom(
     const HashTable< const DiscreteVariable*, const DiscreteVariable* >& map,
     const Instantiation& external) {
    for (const auto& elt: map) {
      const DiscreteVariable& var = *elt.second;

      try {
        Idx val = external.val(*elt.first);

        try {
          chgVal(var, val);
        } catch (NotFound&) {
          GUM_ERROR(NotFound,
                    var.name() << " : missing variable in instantiation");
        }
      } catch (NotFound&) {
        GUM_ERROR(NotFound,
                  var.name() << " : missing variable in external instantiation");
      }
    }
  }

  void Instantiation::masterChangeNotification__(Idx varPos,
                                                 Idx newVal,
                                                 Idx oldVal) const {
    if (master__)
      master__->changeNotification(*this, vars__[varPos], oldVal, newVal);
  }

  void Instantiation::masterFirstNotification__() const {
    if (master__) master__->setFirstNotification(*this);
  }

  void Instantiation::masterIncNotification__() const {
    if (master__) master__->setIncNotification(*this);
  }
  void Instantiation::masterLastNotification__() const {
    if (master__) master__->setLastNotification(*this);
  }
  void Instantiation::masterDecNotification__() const {
    if (master__) master__->setDecNotification(*this);
  }

  // deassociate the master MultiDimAdressable, if any
  bool Instantiation::forgetMaster() {
    if (master__) {
      master__->unregisterSlave(*this);
      master__ = nullptr;
    }
    return true;
  }
  // force the variables sequence order to be the same as the master one
  void Instantiation::synchronizeWithMaster(const MultiDimAdressable* m) {
    if (m != master__) {
      GUM_ERROR(OperationNotAllowed, "only master can do this")
    }

    reorder__(master__->variablesSequence());
  }
  // erase new dim by master
  void Instantiation::eraseWithMaster(const MultiDimAdressable* m,
                                      const DiscreteVariable&   v) {
    if (m != master__) {
      GUM_ERROR(OperationNotAllowed, "only master can do this")
    }

    erase__(v);

    if (master__) master__->setChangeNotification(*this);
  }

  // tries to register the Instantiation to a MultiDimAdressable
  bool Instantiation::actAsSlave(MultiDimAdressable& aMD) {
    // if master__ : not allowed
    if (master__ != nullptr) {
      GUM_ERROR(OperationNotAllowed, "in slave Instantiation")
    }

    master__ = &aMD;

    // perform the registration
    if (aMD.registerSlave(*this)) {
      return true;
    } else {
      master__ = nullptr;
      return false;
    }
  }

  // an operator for user-friendly displaying the content of a Instantiation
  std::ostream& operator<<(std::ostream& aStream, const Instantiation& i) {
    aStream << i.toString();
    return aStream;
  }

} /* namespace gum */
