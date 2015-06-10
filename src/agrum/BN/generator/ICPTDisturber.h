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
 * @brief Interface-like class for disturbing Conditional Probability Tables.
 *
 * @author Pierre-Henri WUILLEMIN and Ariele-Paolo MAESANO
 */

#ifndef GUM_I_CPT_DISTURBER_H
#define GUM_I_CPT_DISTURBER_H

#include <agrum/config.h>
#include <agrum/multidim/potential.h>
#include <agrum/BN/BayesNet.h>

namespace gum {
  /** @class ICPTDisturber *
   * @brief Abstract class for Modifying Conditional Probability Tables.
   * @ingroup bn_group
   *
   * This abstract class is an implementation of the strategy pattern to
   * help changing CPT generation policies for the BayesNetGenerator class.
   */
  template <typename GUM_SCALAR> class ICPTDisturber {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     */
    ICPTDisturber();

    /**
     * Destructor.
     */
    virtual ~ICPTDisturber();
    /// @}

    // ############################################################################
    /// @name CPT disturbing methods
    // ############################################################################
    /**
     * Generates a CPT using floats.
     * @param varIdi The variable id parent of the CPT owner
     * @param varIdj A reference on the CPT owner
     * @param bayesNet the modified Bayesian Network
     * @param cptCopy copy of the CPT before changing size
     * @param marg of the inference before changing size
     */
    virtual void disturbReducCPT(NodeId varIdi, NodeId varIdj,
                                 BayesNet<GUM_SCALAR> &bayesNet,
                                 Potential<GUM_SCALAR> &cptCopy,
                                 Potential<GUM_SCALAR> &marg) = 0;

    /**
     * Generates a CPT using floats.
     * @param varIdi The variable id parent of the CPT owner
     * @param varIdj A reference on the CPT owner
     * @param bayesNet the modified Bayesian Network
     * @param cptCopy copy of the CPT before changing size
     * @param variation degree of variation from the initial probability
     */
    virtual void disturbAugmCPT(NodeId varIdi, NodeId varIdj,
                                BayesNet<GUM_SCALAR> &bayesNet,
                                Potential<GUM_SCALAR> &cptCopy,
                                GUM_SCALAR variation) = 0;
  };

  extern template class ICPTDisturber<float>;
  extern template class ICPTDisturber<double>;

} /* namespace gum */

#include <agrum/BN/generator/ICPTDisturber.tcc>
#endif // GUM_I_CPT_DISTURBER_H
