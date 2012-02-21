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
 * @brief Abstract class for disturbing Conditional Probability Tables.
 *
 * @author Pierre-Henri WUILLEMIN and Ariele-Paolo MAESANO
 */

#ifndef GUM_ABSTRACT_CPT_DISTURBER_H
#define GUM_ABSTRACT_CPT_DISTURBER_H



#include <agrum/multidim/potential.h>
#include <agrum/BN/BayesNet.h>
//#include <agrum/BN/generator/MCBayesNetGenerator.h>



namespace gum {


  /** @class CPTDisturb *
   * @brief Abstract class for Modifying Conditional Probability Tables.
   * @ingroup bn_group
   *
   * This abstract class is an implementation of the strategy pattern to
   * help changing CPT generation policies for the BayesNetGenerator class.
   */
  template <typename T_DATA>
  class AbstractCPTDisturber {
  public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     */
    AbstractCPTDisturber();

    /**
     * Destructor.
     */
    virtual ~AbstractCPTDisturber();
    /// @}

    // ############################################################################
    /// @name CPT disturbing methods
    // ############################################################################
    /// @{
    /**
     * Generates a CPT using floats.
     * @param varIdi The variable id parent of the CPT owner.
     * @param varIdj A reference on the CPT owner.
     * @param bayesNet the modified Bayesian Network.
     * @param cpt_copy copy of the CPT before changing size
     * @param marginal of the inference before changing size.
     */
    virtual void disturbReducCPT(NodeId varIdi, NodeId varIdj, BayesNet<T_DATA>& bayesNet, Potential<T_DATA>& cptCopy, Potential<T_DATA>& marg)=0;
    virtual void disturbAugmCPT( NodeId varIdi, NodeId varIdj, BayesNet<T_DATA>& bayesNet, Potential<T_DATA>& cptCopy, T_DATA variation)=0;

 
    /// @}
  };


} /* namespace gum */

#include<agrum/BN/generator/abstractCPTDisturber.tcc>
#endif // CPTDISTURBER_H
