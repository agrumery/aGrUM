
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <agrum/tools/multidim/implementations/multiDimAdressable.h>
#include <agrum/tools/multidim/setInst.h>
#include <sstream>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/multidim/setInst_inl.h>
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  void SetInst::init__(MultiDimAdressable* master) {
    // for speed issues
    const Sequence< const DiscreteVariable* >& v = master->variablesSequence();
    vars__.resize(v.size());
    vals__.reserve(v.size());
    // fill the SetInst

    for (const auto var: v)
      add__(*var);

    // if ( master ) actAsSlave( master->getMasterRef() );
  }

  // constructor for a SetInst contained into a MultiDimInterface

  SetInst::SetInst(MultiDimAdressable& d) : /*master__( 0 ),*/ overflow__(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(SetInst);
    init__(&d);
  }

  SetInst::SetInst(const MultiDimAdressable& d) :
      /*master__( 0 ),*/ overflow__(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(SetInst);
    init__(const_cast< MultiDimAdressable* >(&d));
  }

  // constructor for a SetInst contained into a MultiDimInterface

  SetInst::SetInst(MultiDimAdressable* d) : /*master__( 0 ),*/ overflow__(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(SetInst);

    if (d) init__(d);
  }

  // constructor for a SetInst contained into a MultiDimInterface
  /** this constructor is needed in order to allow creation of SetInst(this)
   * in MultiDimAdressable and below */

  SetInst::SetInst(const MultiDimAdressable* const_d) :
      /*master__( 0 ),*/ overflow__(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(SetInst);

    if (const_d) init__(const_cast< MultiDimAdressable* >(const_d));
  }

  // copy constructor

  SetInst::SetInst(const SetInst& aI) :
      /* MultiDimInterface(), master__( 0 ),*/ overflow__(false) {
    // for debugging purposes
    GUM_CONS_CPY(SetInst);
    // copy the content of aI
    vars__ = aI.vars__;
    vals__ = aI.vals__;
    overflow__ = aI.overflow__;

    // if ( aI.master__ && notifyMaster ) actAsSlave( *aI.master__ );
  }

  SetInst::SetInst(const Instantiation& aI) :
      /* MultiDimInterface(), master__( 0 ),*/ overflow__(false) {
    // for debugging purposes
    GUM_CONS_CPY(SetInst);
    const Sequence< const DiscreteVariable* >& v = aI.variablesSequence();
    vars__.resize(v.size());
    //__vals.reserve( v.size() );
    // fill the SetInst

    for (const auto var: v) {
      add__(*var);
      vals__[vars__.pos(var)] = (Size(1) << (aI.val(*var)));
    }
  }

  // operator=
  SetInst& SetInst::operator=(const SetInst& aI) {
    // copy the content of aI
    vars__ = aI.vars__;
    vals__ = aI.vals__;
    overflow__ = aI.overflow__;

    return *this;
  }

  // Gives a string version of a SetInst
  std::string SetInst::toString() const {
    std::stringstream sstr;
    // check if the value of the SetInst is correct

    if (overflow__) { sstr << "<invalid>"; }

    sstr << "<";

    Sequence< const DiscreteVariable* >::iterator_safe iter = vars__.begin();

    if (iter != vars__.end()) {
      std::stringstream sstr2;
      sstr2.str("");
      Size si = variable(iter.pos()).domainSize();
      Size valb = vals(iter.pos());

      while (si-- != 0) {
        std::stringstream sstr4;
        sstr4 << ((valb & 1) ? "1" : "0") << sstr2.str();
        valb >>= 1;
        sstr2.str("");
        ;
        sstr2 << sstr4.str();
      }

      sstr << variable(iter.pos()).name() << ":" << sstr2.str();
      ++iter;

      while (iter != vars__.end()) {
        std::stringstream sstr3;
        sstr3 << "";
        si = variable(iter.pos()).domainSize();

        valb = vals(iter.pos());

        while (si-- != 0) {
          std::stringstream sstr4;

          sstr4 << ((valb & 1) ? "1" : "0") << sstr3.str();
          valb >>= 1;
          sstr3.str("");
          sstr3 << sstr4.str();
        }

        sstr << "|" << variable(iter.pos()).name() << ":" << sstr3.str();
        ++iter;
      }
    }

    sstr << ">";

    return sstr.str();
  }

  // an operator for user-friendly displaying the content of a SetInst
  std::ostream& operator<<(std::ostream& aStream, const SetInst& i) {
    aStream << i.toString();
    return aStream;
  }
  gum::SetInst& operator<<(gum::SetInst& inst, const gum::DiscreteVariable& i) {
    inst.add(i);
    return inst;
  }
  gum::SetInst& operator>>(gum::SetInst& inst, const gum::DiscreteVariable& i) {
    inst.erase(i);
    return inst;
  }

  void gum::SetInst::assign_values(
     gum::Bijection< const gum::DiscreteVariable*, const gum::DiscreteVariable* >&
                         bij,
     const gum::SetInst& i,
     gum::SetInst&       j) {
    try {
      for (const auto var: i.variablesSequence())
        j.chgVal(bij.second(var), i.val(var));
    } catch (gum::NotFound&) {
      GUM_ERROR(gum::NotFound, "missing variable in bijection or SetInst");
    }
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
