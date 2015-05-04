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

/**
 * @file
 * @brief Source implementation of SimpleCPTDisturber.
 * @author Pierre-Henri WUILLEMIN and Ariele-Paolo MAESANO
 *
 */

#ifndef GUM_SIMPLE_CPT_DISTURBER_H
#define GUM_SIMPLE_CPT_DISTURBER_H

#include <cstdlib>

#include <agrum/config.h>
#include <agrum/BN/generator/ICPTDisturber.h>

namespace gum {

  /** @class SimpleCPTDisturber
   * @brief Class for disturbing Conditional Probability Tables.
   * @ingroup bn_group
   *
   * This class implements a CPTGenerator CPT generation algorithm.
   */
  template <typename GUM_SCALAR>
  class SimpleCPTDisturber : public ICPTDisturber<GUM_SCALAR> {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     */
    SimpleCPTDisturber();

    /**
     * Destructor.
     */
    virtual ~SimpleCPTDisturber();
    /// @}

    // ############################################################################
    /// @name CPT disturbing methods
    // ############################################################################
    /// @{
    /**
     * Disturb a CPT using GUM_SCALAR when removing parent varIdi.
     * @param varIdi The variable id parent of the CPT owner.
     * @param varIdj The variable on the CPT owner.
     * @param bayesNet the Bayesian Network.
     * @param cptCopy copy of the CPT before reduction.
     * @param marg of the inference before reduction on the node varIdi.
     */
    virtual void disturbReducCPT(NodeId varIdi, NodeId varIdj,
                                 BayesNet<GUM_SCALAR> &bayesNet,
                                 Potential<GUM_SCALAR> &cptCopy,
                                 Potential<GUM_SCALAR> &marg);

    /**
     * Disturb a CPT using GUM_SCALAR when inserting a new parent varIdi.
     * @param varIdi The variable id parent of the CPT owner.
     * @param varIdj A reference on the CPT owner.
     * @param bayesNet the Bayesian Network.
     * @param cptCopy copy of the CPT before augmentation.
     * @param variation degree of variation from the initial probability.
     */
    virtual void disturbAugmCPT(NodeId varIdi, NodeId varIdj,
                                BayesNet<GUM_SCALAR> &bayesNet,
                                Potential<GUM_SCALAR> &cptCopy,
                                GUM_SCALAR variation);
  };

  extern template class SimpleCPTDisturber<float>;
  extern template class SimpleCPTDisturber<double>;
} /* namespace gum */

#include <agrum/BN/generator/simpleCPTDisturber.tcc>
#endif // SIMPLECPTDISTURBER_H
