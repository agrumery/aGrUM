/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief Class for generating Bayesian networks.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */
#ifndef GUM_INF_DIAG_GENERATOR_H
#define GUM_INF_DIAG_GENERATOR_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/ID/generator/simpleUTGenerator.h>
#include <agrum/ID/influenceDiagram.h>

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
  template < typename GUM_SCALAR >
  class InfluenceDiagramGenerator {
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
    explicit InfluenceDiagramGenerator(ICPTGenerator< GUM_SCALAR >* cptGenerator);

    /**
     * Use this constructor if you want to use a different policy for generating
     * UT than the default one.
     * The utGenerator will be erased when the destructor is called.
     * @param utGenerator The policy used to generate UT.
     */
    explicit InfluenceDiagramGenerator(UTGenerator* utGenerator);

    /**
     * Use this constructor if you want to use a different policy for generating
     * both CPT & UT than the defaults ones.
     * The cptGenerator and utGenerator will be erased when the destructor is
     * called.
     * @param cptGenerator The policy used to generate CPT.
     * @param utGenerator The policy used to generate UT.
     */
    InfluenceDiagramGenerator(ICPTGenerator< GUM_SCALAR >* cptGenerator, UTGenerator* utGenerator);

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
    InfluenceDiagram< GUM_SCALAR >* generateID(Size       nbrNodes,
                                               GUM_SCALAR arcDensity,
                                               GUM_SCALAR chanceNodeDensity,
                                               GUM_SCALAR utilityNodeDensity,
                                               Size       max_modality = 2);

    /// @}

    private:
    // Check if a temporal order exists and creates ones if not
    void _checkTemporalOrder_(InfluenceDiagram< GUM_SCALAR >* infdiag);
    // The Conditional Probability Table generator
    ICPTGenerator< GUM_SCALAR >* _cptGenerator_;
    // The Utility Table generator
    UTGenerator* _utGenerator_;
  };

} /* namespace gum */

#include <agrum/ID/generator/influenceDiagramGenerator_tpl.h>
#endif /* GUM_INF_DIAG_GENERATOR_H */
