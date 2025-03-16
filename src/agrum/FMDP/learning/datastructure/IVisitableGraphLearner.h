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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/**
 * @file
 * @brief Headers of the Learning Strategy interface.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */


// =========================================================================
#ifndef GUM_INTERFACE_VISITABLE_GRAPH_LEARNER_H
#define GUM_INTERFACE_VISITABLE_GRAPH_LEARNER_H
// =========================================================================
// =========================================================================
// =========================================================================
#include <agrum/base/multidim/implementations/multiDimFunctionGraph.h>

// =========================================================================
// =========================================================================

namespace gum {

  /**
   * @class IVisitableGraphLearner IVisitableGraphLearner.h
   * <agrum/FMDP/SDyna/IVisitableGraphLearner.h>
   * @brief Interface for manipulating FMDP learner
   * @ingroup fmdp_group
   *
   */
  class IVisitableGraphLearner {
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{

    public:
    // ==========================================================================
    /// Destructor (virtual and empty since it's an interface)
    // ==========================================================================
    virtual ~IVisitableGraphLearner() {}

    /// @}


    // ###################################################################
    /// @name Visit Methods
    // ###################################################################
    /// @{

    public:
    // ==========================================================================
    ///
    // ==========================================================================
    virtual NodeId root() const = 0;

    // ==========================================================================
    ///
    // ==========================================================================
    virtual bool isTerminal(NodeId ni) const = 0;

    // ==========================================================================
    ///
    // ==========================================================================
    virtual const DiscreteVariable* nodeVar(NodeId ni) const = 0;

    // ==========================================================================
    ///
    // ==========================================================================
    virtual NodeId nodeSon(NodeId ni, Idx modality) const = 0;

    // ==========================================================================
    ///
    // ==========================================================================
    virtual Idx nodeNbObservation(NodeId ni) const = 0;

    virtual void insertSetOfVars(MultiDimFunctionGraph< double >*) const = 0;


    /// @}
  };
}   // namespace gum
#endif   // GUM_INTERFACE_VISITABLE_GRAPH_LEARNER_H
