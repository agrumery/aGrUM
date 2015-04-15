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
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_INF_DIAG_GENERATOR_H
#define GUM_INF_DIAG_GENERATOR_H

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <iostream>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/generator/UTGenerator.h>
#include <agrum/ID/generator/simpleUTGenerator.h>

namespace gum {

  /**
   * @class InfluenceDiagramGenerator influenceDiagramGenerator.h
   *<agrum/ID/generator/influenceDiagramGenerator.h>
   * @brief Class for generating influence diagram.
   * @ingroup id_group
   *
   * This class randomly generates an influence diagram given four parameters:
   * the number of nodes,the probability of adding an arc between two nodes,
   * the proportion of chance node and the proportion of utility node (the
   * proportion of decision node is deduce from thos two)
   */
  template <typename GUM_SCALAR> class InfluenceDiagramGenerator {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{
    /**
     * Default constructor.
     * Use the SimpleCPTGenerator for generating the IDs CPT.
     * Use the SimpleUTGenerator for generating the IDs UT.
     */
    InfluenceDiagramGenerator();

    /**
     * Use this constructor if you want to use a different policy for generating
     * CPT than the default one.
     * The cptGenerator will be erased when the destructor is called.
     * @param cptGenerator The policy used to generate CPT.
     */
    InfluenceDiagramGenerator(ICPTGenerator<GUM_SCALAR> *cptGenerator);

    /**
     * Use this constructor if you want to use a different policy for generating
     * UT than the default one.
     * The utGenerator will be erased when the destructor is called.
     * @param utGenerator The policy used to generate UT.
     */
    InfluenceDiagramGenerator(UTGenerator *utGenerator);

    /**
     * Use this constructor if you want to use a different policy for generating
     * both CPT & UT than the defaults ones.
     * The cptGenerator and utGenerator will be erased when the destructor is called.
     * @param cptGenerator The policy used to generate CPT.
     * @param utGenerator The policy used to generate UT.
     */
    InfluenceDiagramGenerator(ICPTGenerator<GUM_SCALAR> *cptGenerator,
                              UTGenerator *utGenerator);

    /**
     * Destructor.
     */
    ~InfluenceDiagramGenerator();
    /// @}

    // ############################################################################
    /// @name ID generation methods
    // ############################################################################
    /// @{
    /**
     * Generates an influence diagram using floats.
     * @param nbrNodes The number of nodes in the generated ID.
     * @param arcDensity The probability of adding an arc between two nodes.
     * @param chanceNodeDensity The proportion of chance node
     * @param utilityNodeDensity The proportion of utility node
     * @param max_modality Each DRV has from 2 to max_modality modalities
     * @return A IDs randomly generated.
     */
    InfluenceDiagram<GUM_SCALAR> *generateID(Size nbrNodes, float arcDensity,
                                             float chanceNodeDensity,
                                             float utilityNodeDensity,
                                             int max_modality = 2);

    /// @}
    private:
    // Check if a temporal order exists and creates ones if not
    void __checkTemporalOrder(InfluenceDiagram<GUM_SCALAR> *infdiag);
    // The Conditional Probability Table generator
    ICPTGenerator<GUM_SCALAR> *__cptGenerator;
    // The Utility Table generator
    UTGenerator *__utGenerator;
  };

} /* namespace gum */

#include <agrum/ID/generator/influenceDiagramGenerator.tcc>
#endif /* GUM_INF_DIAG_GENERATOR_H */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
