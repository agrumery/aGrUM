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
 * @brief Source implementation of SimpleCPTDisturber.
 *
 * @author Pierre-Henri WUILLEMIN and Ariele-Paolo MAESANO
 *
 */
#include <agrum/BN/generator/simpleCPTDisturber.h>

namespace gum {

  // Default constructor.
  template <typename GUM_SCALAR>
  INLINE SimpleCPTDisturber<GUM_SCALAR>::SimpleCPTDisturber()
      : ICPTDisturber<GUM_SCALAR>() {
    GUM_CONSTRUCTOR(SimpleCPTDisturber);
  }

  // Destructor.
  template <typename GUM_SCALAR>
  INLINE SimpleCPTDisturber<GUM_SCALAR>::~SimpleCPTDisturber() {
    GUM_DESTRUCTOR(SimpleCPTDisturber);
  }

  // Generates a CPT using GUM_SCALAR.
  // @param varID The variable id of the CPT owner.
  // @param cpt A reference on the CPT to fill.

  template <typename GUM_SCALAR>
  void SimpleCPTDisturber<GUM_SCALAR>::disturbReducCPT(
      NodeId varIdi, NodeId varIdj, BayesNet<GUM_SCALAR> &bayesNet,
      Potential<GUM_SCALAR> &cptCopy, Potential<GUM_SCALAR> &marg) {
    Instantiation i(cptCopy);
    Instantiation iCopy(cptCopy);
    Instantiation imarg(marg);

    iCopy.forgetMaster();
    iCopy.erase(bayesNet.variable(varIdi));

    for (i.setFirstNotVar(bayesNet.variable(varIdi)); !i.end();
         i.incNotVar(bayesNet.variable(varIdi)), ++iCopy) {
      GUM_SCALAR potval = 0;

      for (i.setFirstVar(bayesNet.variable(varIdi)), imarg.setFirst(); !i.end();
           i.incVar(bayesNet.variable(varIdi)), ++imarg) {
        potval += cptCopy.get(i) * marg.get(imarg);
      }

      bayesNet.cpt(varIdj).set(iCopy, potval);
    }

    bayesNet.cpt(varIdj).normalize();
  }

  template <typename GUM_SCALAR>
  void SimpleCPTDisturber<GUM_SCALAR>::disturbAugmCPT(NodeId varIdi, NodeId varIdj,
                                                      BayesNet<GUM_SCALAR> &bayesNet,
                                                      Potential<GUM_SCALAR> &cptCopy,
                                                      GUM_SCALAR variation) {
    Instantiation i(cptCopy);
    Instantiation iCopy(cptCopy);
    iCopy.forgetMaster();
    iCopy.add(bayesNet.variable(varIdi));

    for (iCopy.setFirstNotVar(bayesNet.variable(varIdi)); !iCopy.end();
         iCopy.incNotVar(bayesNet.variable(varIdi)), ++i) {
      for (iCopy.setFirstVar(bayesNet.variable(varIdi)); !iCopy.end();
           iCopy.incVar(bayesNet.variable(varIdi))) {
        bayesNet.cpt(varIdj)
            .set(iCopy, cptCopy.get(i) +
                            (GUM_SCALAR)(rand() % (Size)(variation * 100000)) /
                                100000); // TODO better to do here
      }
    }

    bayesNet.cpt(varIdj).normalize();
  }

} /* namespace gum */
