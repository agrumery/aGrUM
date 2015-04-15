
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
 * @brief
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_DIAGRAM_H
#define GUM_MULTI_DIM_DECISION_DIAGRAM_H

#include <agrum/core/algorithms/approximationPolicy/approximationPolicy.h>
#include <agrum/core/algorithms/approximationPolicy/exactPolicy.h>

#include <agrum/multidim/multiDimDecisionDiagramBase.h>
#include <agrum/multidim/multiDimDecisionDiagramFactory.h>

namespace gum {

  template <typename GUM_SCALAR, template <class> class IApproximationPolicy>
  class MultiDimDecisionDiagramFactory;

  /**
   * @class MultiDimDecisionDiagram multiDimDecisionDiagram.h
   *<agrum/multidim/multiDimDecisionDiagram.h>
   * @brief Class implementingting a decision diagram model
   * For description of general methods, please refer to multidimdecisiondiagrambase
   * For specific implementation and use of multidimdecisiondiagram this is the right
   *place :
   * The idea is that approximation policy is given as a template class to this class
   *( please
   * refer to agrum/core/algorithms/approximationPolicy.h for more details on such
   *classes ). MultiDimDecisionDiagram inherits from
   * this class.
   * A default classe exists : Exact Policy whixh commit no approximation on value
   *given. As a default
   * you just have to declare MultiDimDecisionDiagram<GUM_SCALAR> and play with.
   * If you want to use a different approximation pattern, just
   *MultiDimDecisionDiagram<GUM_SCALAR, CLASS_DESCRIBING_NEW_PATTERN>.
   * Therefor, if do operation on MultiDimDecisionDiagram the return type will likely
   *be MultiDimDecisionDiagramBase
   * @ingroup multidim_group
   *
   */
  template <typename GUM_SCALAR,
            template <class> class IApproximationPolicy = ExactPolicy>

  class MultiDimDecisionDiagram : public MultiDimDecisionDiagramBase<GUM_SCALAR>,
                                  public IApproximationPolicy<GUM_SCALAR> {

    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    MultiDimDecisionDiagram();

    /**
     * Copy constructor.
     */
    MultiDimDecisionDiagram(
        const MultiDimDecisionDiagram<GUM_SCALAR, IApproximationPolicy> &md);

    /**
     * Constructor with approximation initialisation
     */
    MultiDimDecisionDiagram(const IApproximationPolicy<GUM_SCALAR> &md);

    /**
     * Destructor.
     */
    ~MultiDimDecisionDiagram();

    /// @}

    // ===========================================================================
    /// @name Various Methods
    // ===========================================================================
    /// @{

    /**
     * This method creates a clone of this object, without its content
     * (including variable), you must use this method if you want to ensure
     * that the generated object has the same type than the object containing
     * the called newFactory()
     * For example :
     *   MultiDimArray<double> y;
     *   MultiDimContainer<double>* x = y.newFactory();
     * Then x is a MultiDimArray<double>*
     *
     * @warning you must desallocate by yourself the memory
     * @return an empty clone of this object with the same type
     */
    MultiDimContainer<GUM_SCALAR> *newFactory() const;

    ///@}

    // ===========================================================================
    /// @name Approximation Handling functions
    // ===========================================================================
    /// @{

    /**
     * Returns a factory that used same approximation pattern
     */
    virtual MultiDimDecisionDiagramFactoryBase<GUM_SCALAR> *getFactory() const;

    /**
     * Returns a factory that used same approximation pattern
     * Allows to set parameter for that approximation
     */
    virtual MultiDimDecisionDiagramFactoryBase<GUM_SCALAR> *
    getFactory(const ApproximationPolicy<GUM_SCALAR> &md) const;

    /// Convert value to approximation representation
    inline GUM_SCALAR fromExact(const GUM_SCALAR &value) const {
      return IApproximationPolicy<GUM_SCALAR>::fromExact(value);
    };

    ///@}

    protected:
  };

} /* namespace gum */

#include <agrum/multidim/multiDimDecisionDiagram.tcc>

#endif /* GUM_MULTI_DIM_DECISION_DIAGRAM_H */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
