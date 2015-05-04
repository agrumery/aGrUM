/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
  template <typename GUM_SCALAR>
  INLINE SimpleCPTGenerator<GUM_SCALAR>::SimpleCPTGenerator()
      : ICPTGenerator<GUM_SCALAR>() {
    GUM_CONSTRUCTOR(SimpleCPTGenerator);
  }

  // Destructor.
  template <typename GUM_SCALAR>
  INLINE SimpleCPTGenerator<GUM_SCALAR>::~SimpleCPTGenerator() {
    GUM_DESTRUCTOR(SimpleCPTGenerator);
  }

  // Generates a CPT using floats.
  // @param varID The variable id of the CPT owner.
  // @param cpt A reference on the CPT to fill.

  template <typename GUM_SCALAR>
  void
  SimpleCPTGenerator<GUM_SCALAR>::generateCPT(const Idx &varId,
                                              const Potential<GUM_SCALAR> &cpt) {
    std::vector<GUM_SCALAR> v;

    for (Size i = 0; i < cpt.domainSize(); ++i) {
      v.push_back((GUM_SCALAR)rand());
    }

    cpt.fillWith(v);
    Instantiation varInst;
    varInst.add(cpt.variable(varId));
    Instantiation cptInst(cpt);

    for (cptInst.setFirstOut(varInst); !cptInst.end(); cptInst.incOut(varInst)) {
      GUM_SCALAR sum = (GUM_SCALAR)0;

      for (cptInst.setFirstIn(varInst); !cptInst.end(); cptInst.incIn(varInst)) {
        sum += cpt[cptInst];
      }

      for (cptInst.setFirstIn(varInst); !cptInst.end(); cptInst.incIn(varInst)) {
        cpt.set(cptInst, cpt[cptInst] / sum);
      }

      cptInst.unsetEnd();
    }
  }

} /* namespace gum */
