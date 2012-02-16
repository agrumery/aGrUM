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
 * @brief Source implementation of DefaultCPTDisturber.
 * @author Pierre-Henri WUILLEMIN and Ariele-Paolo MAESANO
 *
 */

#ifndef GUM_DEFAULT_CPT_DISTURBER_H
#define GUM_DEFAULT_CPT_DISTURBER_H

#include <cstdlib>

#include <agrum/BN/generator/abstractCPTDisturber.h>


namespace gum {


  /** @class DefaultCPTDisturber
   * @brief Class for disturbing Conditional Probability Tables.
   * @ingroup bn_group
   *
   * This class implements a CPTGenerator CPT generation algorithm.
   */
  template <typename T_DATA>
  class DefaultCPTDisturber: public AbstractCPTDisturber<T_DATA> {
  public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     */
    DefaultCPTDisturber();

    /**
     * Destructor.
     */
    virtual ~DefaultCPTDisturber();
    /// @}

    // ############################################################################
    /// @name CPT disturbing methods
    // ############################################################################
    /// @{
    /**
     * Disturb a CPT using T_DATA.
     * @param varIdi The variable id parent of the CPT owner.
     * @param varIdj The variable on the CPT owner.
     * @param bayesNet tne Bayesian Network.
     * @param cptCopy copy of the CPT before reduction.
     * @param marginal of the inference before reduction on the node varIdi.
     */

    virtual void disturbReducCPT(NodeId varIdi, NodeId varIdj, BayesNet<T_DATA> & bayesNet, Potential<T_DATA>& cptCopy, Potential<T_DATA>& marg);
    /**
     * Disturb a CPT using T_DATA.
     * @param varIdi The variable id parent of the CPT owner.
     * @param varIdj A reference on the CPT owner.
     * @param bayesNet the Bayesian Network.
     * @param cptCopy copy of the CPT before augmentation.
     * @param variation degree of variation from the initial probability .
     */
    virtual void disturbAugmCPT(NodeId varIdi, NodeId varIdj, BayesNet<T_DATA> & bayesNet, Potential<T_DATA>& cptCopy, T_DATA variation);

 
  };


} /* namespace gum */

#include <agrum/BN/generator/defaultCPTDisturber.tcc>
#endif // SIMPLECPTDISTURBER_H
