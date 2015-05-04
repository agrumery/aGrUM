
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
 * @brief Class allowing creation of Decision Diagram
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_DIAGRAM_FACTORY_H
#define GUM_MULTI_DIM_DECISION_DIAGRAM_FACTORY_H

#include <agrum/core/algorithms/approximationPolicy/approximationPolicy.h>
#include <agrum/core/algorithms/approximationPolicy/exactPolicy.h>

#include <agrum/multidim/multiDimDecisionDiagramFactoryBase.h>
#include <agrum/multidim/multiDimDecisionDiagram.h>

namespace gum {

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  class MultiDimDecisionDiagram;

  /**
   * @class MultiDimDecisionDiagramFactoryBase multiDimDecisionDiagramFactoryBase.h
   *<agrum/multidim/multiDimDecisionDiagramFactoryBase.h>
   * @brief Class implementingting an algebraic decision diagram factory
   * @ingroup multidim_group
   *
   */
  template <typename GUM_SCALAR,
            template <class> class IApproximationPolicy = ExactPolicy>
  class MultiDimDecisionDiagramFactory
      : public MultiDimDecisionDiagramFactoryBase<GUM_SCALAR>,
        public IApproximationPolicy<GUM_SCALAR> {

    public:
    // ===========================================================================
    /// @name Constructors, Destructors.
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    MultiDimDecisionDiagramFactory();

    /**
     * clone constructor.
     * @arg md is used for initialize approximation policy
     */
    MultiDimDecisionDiagramFactory(const IApproximationPolicy<GUM_SCALAR> &md);

    /**
     * Destructor.
     * @warning : this will not destroy properties on node. They have to be removed
     * on multidim destruction
     */
    ~MultiDimDecisionDiagramFactory();

    /// @}

    /**
     * Returns the multidimDecisionDiagram made
     */
    MultiDimDecisionDiagramBase<GUM_SCALAR> *
    getMultiDimDecisionDiagram(bool fillWithDefaultArc = true,
                               GUM_SCALAR defaultValue = 0, bool doCompress = false);

    /// Convert value to approximation representation
    inline GUM_SCALAR fromExact(const GUM_SCALAR &value) const {
      return IApproximationPolicy<GUM_SCALAR>::fromExact(value);
    };

    private:
    void __mergedNode(NodeId from, NodeId to);
  };

} /* namespace gum */

#include <agrum/multidim/multiDimDecisionDiagramFactory.tcc>

#endif /* GUM_MULTI_DIM_DECISION_DIAGRAM_FACTORY_H */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
