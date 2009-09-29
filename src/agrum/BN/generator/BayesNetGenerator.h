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
 * @brief Class for generating bayesian netwroks.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_BAYES_NET_GENERATOR_H
#define GUM_BAYES_NET_GENERATOR_H

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/CPTGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>


namespace gum {


  /**
   * @class BayesNetGenerator BayesNetGenerator.h <agrum/BN/generator/BayesNetGenerator.h>
   * @brief Class for generating bayesian networks.
   * @ingroup bn_group
   *
   * This class randomly generates a bayesian network given two parameters:
   * the number of nodes and the probability of adding an arc between two nodes.
   */
  class BayesNetGenerator {
  public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     * Use the SimpleCPTGenerator for generating the BNs CPT.
     */
    BayesNetGenerator();

    /**
     * Use this constructor if you want to use a different policy for generating
     * CPT than the default one.
     * The cptGenerator will be erased when the destructor is called.
     * @param cptGenerator The policy used to generate CPT.
     */
    BayesNetGenerator(CPTGenerator* cptGenerator);

    /**
     * Destructor.
     */
    ~BayesNetGenerator();
    /// @}

    // ############################################################################
    /// @name BN generation methods
    // ############################################################################
    /// @{
    /**
     * Generates a bayesian network using floats.
     * @param nbrNodes The number of nodes in the generated BN.
     * @param density The probability of adding an arc between two nodes.
     * @param max_modality Each DRV has from 2 to max_modality modalities
     * @return A BNs randomly generated.
     */
    BayesNet<float>* generateBNF(Size nbrNodes, float density,int max_modality=2);

    /**
     * Generates a bayesian network using doubles.
     * @param nbrNodes The number of nodes in the generated BN.
     * @param density The probability of adding an arc between two nodes.
     * @param max_modality Each DRV has from 2 to max_modality modalities
     * @return A BNs randomly generated.
     */
    BayesNet<double>* generateBND(Size nbrNodes, float density,int max_modality=2);
    /// @}
  private:
    // The Conditional Probability Table generator
    CPTGenerator* __cptGenerator;
  };


} /* namespace gum */


#endif /* GUM_BAYES_NET_GENERATOR_H */

