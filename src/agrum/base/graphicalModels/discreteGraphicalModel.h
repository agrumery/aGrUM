/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Base class for graphical models over discrete variables.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_DISCRETE_GRAPHICAL_MODEL_H
#define GUM_DISCRETE_GRAPHICAL_MODEL_H

#include <agrum/agrum.h>

#include <agrum/base/graphicalModels/graphicalModel.h>
#include <agrum/base/graphicalModels/variableNodeMap.h>

namespace gum {

  /**
   * @class DiscreteGraphicalModel
   * @headerfile discreteGraphicalModel.h
   *   <agrum/base/graphicalModels/discreteGraphicalModel.h>
   * @brief Intermediate base class for graphical models over discrete variables.
   *
   * Owns a VariableNodeMap and provides concrete implementations of the five
   * variable-accessor methods declared pure virtual in GraphicalModel
   * (variableNodeMap, variable, nodeId, idFromName, variableFromName).
   * All concrete models (BayesNet, MarkovRandomField, InfluenceDiagram, …)
   * inherit from this class via DAGmodel or UGmodel.
   */
  class DiscreteGraphicalModel: public GraphicalModel {
    public:
    /// @name Constructors / Destructors
    /// @{

    DiscreteGraphicalModel();
    virtual ~DiscreteGraphicalModel();
    DiscreteGraphicalModel(const DiscreteGraphicalModel& source);
    DiscreteGraphicalModel(DiscreteGraphicalModel&& source);

    /// @}
    /// @name Variable accessor methods
    /// @{

    /**
     * Returns a constant reference to the VariableNodeMap of this model.
     */
    const VariableNodeMap& variableNodeMap() const override;

    /**
     * Returns a constant reference over a variable given its node id.
     * @throw NotFound if no variable's id matches id.
     */
    const DiscreteVariable& variable(NodeId id) const override;

    /**
     * Returns the NodeId of a variable.
     * @throw NotFound if no variable matches var.
     */
    NodeId nodeId(const DiscreteVariable& var) const override;

    /**
     * Returns the NodeId of a variable given its name.
     * @throw NotFound if no such name exists in the model.
     */
    NodeId idFromName(std::string_view name) const override;

    /**
     * Returns a constant reference over a variable given its name.
     * @throw NotFound if no such name exists in the model.
     */
    const DiscreteVariable& variableFromName(std::string_view name) const override;

    /// @}

    protected:
    DiscreteGraphicalModel& operator=(const DiscreteGraphicalModel& source);
    DiscreteGraphicalModel& operator=(DiscreteGraphicalModel&& source);

    /// Mapping between NodeIds and discrete variables.
    VariableNodeMap varMap_;
  };

}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/base/graphicalModels/discreteGraphicalModel_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_DISCRETE_GRAPHICAL_MODEL_H */
