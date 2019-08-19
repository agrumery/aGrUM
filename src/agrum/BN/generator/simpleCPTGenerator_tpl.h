
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


/** @file
 * @brief Source implementation of SimpleCPTGenerator.
 *
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI and Ariele-Paolo MAESANO
 *
 */
#include <agrum/BN/generator/simpleCPTGenerator.h>

#include <ctime>

namespace gum {

  // Default constructor.
  template < typename GUM_SCALAR >
  INLINE SimpleCPTGenerator< GUM_SCALAR >::SimpleCPTGenerator() :
      ICPTGenerator< GUM_SCALAR >() {
    GUM_CONSTRUCTOR(SimpleCPTGenerator);
  }

  // Destructor.
  template < typename GUM_SCALAR >
  INLINE SimpleCPTGenerator< GUM_SCALAR >::~SimpleCPTGenerator() {
    GUM_DESTRUCTOR(SimpleCPTGenerator);
  }

  // Generates a CPT using floats.
  // @param varID The variable id of the CPT owner.
  // @param cpt A reference on the CPT to fill.

  template < typename GUM_SCALAR >
  void SimpleCPTGenerator< GUM_SCALAR >::generateCPT(
     const Idx& varId, const Potential< GUM_SCALAR >& cpt) {
    cpt.random();

    const auto&   var = cpt.variable(varId);
    Instantiation cptInst(cpt);

    for (cptInst.setFirstNotVar(var); !cptInst.end(); cptInst.incNotVar(var)) {
      GUM_SCALAR sum = (GUM_SCALAR)0;

      for (cptInst.setFirstVar(var); !cptInst.end(); cptInst.incVar(var)) {
        sum += cpt[cptInst];
      }

      for (cptInst.setFirstVar(var); !cptInst.end(); cptInst.incVar(var)) {
        cpt.set(cptInst, cpt[cptInst] / sum);
      }

      cptInst.unsetEnd();
    }
  }

} /* namespace gum */
